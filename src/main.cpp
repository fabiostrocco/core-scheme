#include <iostream>
#include <string>
#include <list>
#include <stdio.h>
#include <typeinfo>
#include <string.h>

#include "parser.h"
#include "formatter.h"
#include "analysis.h"
#include "options.h"
#include "interpreter.h"
#include "code_generation.h"
#include "error.h"

/**
 RESTRICTION IN THE COMPILER: to keep the compiler simple, the following restrictions have been applied

 1. Escaping variable are not supported. Ex. (lambda (x) (lambda (y) x)) is not supported
 2. There is no explicit check for function calls. Ex. (1 2) will raise a segmentation fault error
 3. set! is a function that returns 0
 4. A function is a number itself, representing the function pointer. So the (ouptut (lambda (x) x)) will
    print the reference of lambda.
 5. Arithmetic functions are not variadic. For example: (+ 1 2 3) is not allowed, but (+ 1 (+ 2 3)) is allowed

 The former set of restrictions does not apply to the interpreter.
 */

using namespace std;
using namespace cscheme;

int main(int argc, char** argv) {
  CommandLineOptions options = CommandLineOptions(argc, argv);

  if(options.IsPrintInfo()) {
    options.PrintInfo();
    return EXIT_OK;
  }
  
  char *file_name = options.GetFileName();

  if(!options.FileExists()) {
    cerr << "File not found " << file_name << endl;
    return EXIT_FILE_NOT_FOUND;
  }
  FILE* source = options.GetFile();

  Lexer lexer = Lexer(source, argv[2]);
  Parser* parser = new Parser(lexer);
  AstCompilationUnit* unit = parser->Parse(true);
  unordered_set<string> visited;
  visited.insert(file_name);
  parser->Resolve(unit, visited);


  Analyzer* analyzer = new Analyzer(unit);
  analyzer->Analyze();

  if(options.IsPrintFormatted()) {
    Formatter* formatter = new Formatter(unit);
    formatter->Print();
    cout << endl;
  } else if(options.IsCompileToLLVM()) {
    CodeGenerator* generator = new CodeGenerator(unit);
    generator->Compile();
    generator->Print();
  } else if(options.IsInterpreter()) {
    Interpreter* interpreter = new Interpreter(unit);
    interpreter->Run();
  } else {
    options.PrintInfo();
  }

  options.CloseFile();
  
  return EXIT_OK;
}
