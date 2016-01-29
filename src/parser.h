/* 
   Syntax
   -----------------------------------------------------------------------
   P ::= L* D* e
   L ::= (load "PATH")
   D ::= (define x e)

   e ::= N | x | (lambda (x...x) e) | (e ... e) | (if e e e) | (set! x e)

   default: +,-,*,/,=,<,>,read,write

*/

#include <string>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <unordered_set>
#include <list>
#include "lexer.h"

using namespace std;
using namespace tokens;
using namespace ast;

namespace cscheme {

  /* Global functions  */

  /** 
   * Print an error that occurs during the compilation phase.
   * The node paramater represents the target node.
   * For example, if a syntax error occur at the node [(if 1 2)], then
   * this function will print informations to localize such node.
   */
  void PrintError(AstNode* node, string message);

  /**
   * Parses a source file
   *
   * P ::= L* D* e
   */
  class Parser {    
  public:
  Parser(Lexer lexer) : lexer_(lexer) {}
    
    AstCompilationUnit* Parse(bool force_main);
    
    /**
     * Load the libraries (if any) referenced by this compilation unit.
     */
    void Resolve(AstCompilationUnit* unit, unordered_set<string> visited);
  
  private:
    //Parse ID e
    AstDefinition* ParseDefinition(LineInfo info);

    //e ::= N | x | (lambda (x...x) e) | (e ... e) | (if e e e) | (set! x e)
    AstExpression* ParseExpression(LineInfo info);

    // (lambda >(x...x) e)
    AstLambdaAbstraction* ParseLambdaAbstraction(LineInfo info);

    // (if >e e e)
    AstIfExpression* ParseIfExpression(LineInfo info);

    // (>e e...e)
    AstLambdaApplication* ParseLambdaApplication(LineInfo info);

    // (set! >x e)
    AstSetExpression* ParseSetExpression(LineInfo info);

  private:
    Lexer lexer_;
    Token* token_;
    Token* NextToken() { token_ = lexer_.NextToken(); return token_; }
    void BackTrack(int unit) { lexer_.BackTrack(unit); token_ = lexer_.CurrentToken(); }
  };

};
