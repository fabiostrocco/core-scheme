#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include "ast.h"

using namespace std;
using namespace tokens;

namespace cscheme {

  /** 
   * This is the lexer class. It supports 2-level backtracking (use method BackTrack(1) or BackTrack(2)). The NextToken() method step 
   * forawrd and return one token 
   */
  class Lexer {
  public:
    Lexer(FILE* source, string prefix) {
      this->source_ = source;
      this->prefix_ = prefix;
      this->is_back_ = false;
      this->pos_ = 0;
      this->line_ = 0;
      this->eof_state_ = false;
    }
  
    Token* CurrentToken();
    void BackTrack(int units);
    Token* NextToken();
    void PrintError(std::string message);

  private:
    bool Empty(char ch);
    char Eat();
    void Back();
    bool IsID(char ch);
    bool IsNumber(char ch);
    Token* UpdateList(Token* token);

    FILE* source_;
    string prefix_;
    char current_;
    bool is_back_;
    int pos_;
    int line_;
    bool eof_state_;
    Token* p_token_ = NULL;
    Token* pp_token_ = NULL;
    int track_ = 0; //0 = eat next token, 1 = return pToken, 2 = return ppToken
  };

};
