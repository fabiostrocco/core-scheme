#include <stdlib.h>
#include <stdio.h>
#include "lexer.h"
#include "error.h"

namespace cscheme {
  
  Token* Lexer::CurrentToken() {
    ASSERT(track_ == 1 || track_ == 2);
    if(track_ == 1) {
      return p_token_;
    } else if(track_ == 2) {
      return pp_token_;
    }
  
    return NULL;
  };
  
  void Lexer::BackTrack(int units) {
    ASSERT(units <= 2 && units >= 0);
    track_ += units;
    ASSERT(track_ <= 2);
  }

  Token* Lexer::NextToken() {
    ASSERT(pos_ >= 0);
    ASSERT(line_ >= 0);
    if(track_ == 1) {
      track_ = 0;
      ASSERT(p_token_ != NULL);
      return p_token_;
    } else if(track_ == 2) {
      track_ = 1;
      ASSERT(p_token_ != NULL);
      return pp_token_;
    }

    ASSERT(track_ == 0);

    char curr_char;
    string curr_token = "";
    if(eof_state_) return UpdateList(new EOFToken(prefix_, pos_, pos_, line_));

    int initial_position_ = pos_;

    /* Ignore white spaces */

    do { 
      curr_char = Eat();
      /* Ignore inline comments */
      while(curr_char == ';') {
	while(curr_char != '\n') {
	  if(curr_char == EOF) {
	    return UpdateList(new EOFToken(prefix_, pos_, pos_, line_));
	  }
	  curr_char = Eat();
	}
	curr_char = Eat();
      }
    } while(Empty(curr_char));

    if(curr_char == EOF) {
      return UpdateList(new EOFToken(prefix_, pos_, pos_, line_));
    }

    if(curr_char == '(' || curr_char == '[')
      return UpdateList(new BraceToken(prefix_, "(", pos_, pos_ + 1, line_));
    if(curr_char == ')' || curr_char == ']')
      return UpdateList(new BraceToken(prefix_, ")", pos_, pos_ + 1, line_));

    curr_token += curr_char;

    // String lexing mode
    // " c* " where c is any character except " and \n
    if(curr_char == '\"') {
      curr_token = "";
      do {
	curr_char = Eat();
	if(curr_char == '\n') {
	  PrintError("new line not allowed within strings");
	}
	if(curr_char == EOF) {
	  PrintError("\" expected before reaching the end of file");
	}
	if(curr_char != '\"') curr_token += curr_char;
      } while(curr_char != '\"');
      return UpdateList(new StringToken(prefix_, curr_token, pos_, pos_+1, line_));
    }
    
    if(IsID(curr_char)) {
      while(IsID(curr_char)) {
	curr_char = Eat();
	if(IsID(curr_char)) curr_token += curr_char;
      }
      Back();

      if(curr_token.compare("define") == 0 || 
	 curr_token.compare("load") == 0 ||
	 curr_token.compare("lambda") == 0 || 
	 curr_token.compare("set!") == 0 ||
	 curr_token.compare("if") == 0) {
	return UpdateList(new KeywordToken(prefix_, curr_token, initial_position_, pos_, line_));
      } else {
	return UpdateList(new IDToken(prefix_, curr_token, initial_position_, pos_, line_));
      }
    } else if(IsNumber(curr_char)) {
      while(IsNumber(curr_char)) {
	curr_char = Eat();
	if(IsNumber(curr_char)) curr_token += curr_char;
      }
      Back();

      return UpdateList(new NumberToken(prefix_, curr_token, initial_position_, pos_, line_));
    } else if(curr_char == EOF) {
      return UpdateList(new EOFToken(prefix_, pos_, pos_, line_));
    }

    PrintError("unrechognized token " + curr_token);
    return UpdateList(new EOFToken(prefix_, pos_, pos_, line_));
  }

  void Lexer::PrintError(std::string message) {
    cerr << prefix_ << ":" << line_ << ":" << pos_ << ": error: " << message << endl;
    exit(EXIT_LEXER_ERROR);
  }

  void Lexer::Close() {
    fclose(source_);
  }

  bool Lexer::Empty(char ch) {
    return ch ==' ' || ch =='\t' || ch =='\n';
  }

  char Lexer::Eat() {
    char result;
    if(is_back_) {
      is_back_ = false;
      return current_;
    }

    result = getc(source_);
    pos_++;
    if(result == '\n') {
      pos_ = 0;
      line_++;
    }
    current_ = result;

    if(result == EOF) {
      eof_state_ = true;
    }
    return result;
  }

  void Lexer::Back() { is_back_ = true; }

  bool Lexer::IsID(char ch) {
    return (ch <= 'Z' && ch >= 'A') ||
      (ch <= 'z' && ch >= 'a') ||
      ch == '+' || ch == '-' || 
      ch == '*' || ch == '/' ||
      ch == '!' || ch == '=' || ch == '<' || 
      ch == '>' || ch == '&' || ch == '|' || ch == '?';
  }

  bool Lexer::IsNumber(char ch) {
    return (ch <= '9' && ch >= '0');
  }

  Token* Lexer::UpdateList(Token* token) {
    pp_token_ = p_token_;
    p_token_ = token;
    return token;
  }


};
