#include <stdio.h>
#include <stdlib.h>
#include "options.h"
#include "error.h"

void CommandLineOptions::Init(char* f_name, Mode mode, bool is_debug) {
  this->f_name_ = f_name; 
  this->file_ = NULL;
  this->mode_ = mode;
  cscheme::SetDebug(is_debug);
}

char* CommandLineOptions::GetFileName() { return f_name_; }
FILE* CommandLineOptions::GetFile() {
  if(file_ == NULL) {
    file_ = fopen(f_name_, "r");
  }

  return file_;
}

bool CommandLineOptions::FileExists() { return GetFile() != NULL; }
bool CommandLineOptions::IsPrintFormatted() { return mode_ == PRINT; }
bool CommandLineOptions::IsCompileToLLVM() { return mode_ == COMPILE; }
bool CommandLineOptions::IsPrintInfo() { return mode_ == INFO; }
bool CommandLineOptions::IsInterpreter() { return mode_ == INTERPRETER; }
void CommandLineOptions::PrintInfo() {
  cout << "Usage: csc-comp [--phase=option] [--debug] source" << endl;
  cout << "Where source is the source program file name";
  cout << "and options includes:" << endl;
  cout << "\t analysis          create lexical analysis results" << endl;
  cout << "\t llvm              create llvm compiled file" << endl;
  cout << "\t interpreter       run the interpreter" << endl;
}
