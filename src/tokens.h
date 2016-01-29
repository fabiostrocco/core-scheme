#include <string>
#include <iostream>

#define T_ID "ID"
#define STRING "STRING"
#define KEYWORD "KEYWORD"
#define NUMBER "NUMBER"
#define BRACE "BRACE"
#define T_EOF "EOF"

using namespace std;

namespace tokens {

  /**
   * Instance of this class contains information to locate a part of source code. This class is used to map AST nodes or tokens into source file locations 
   */
  class LineInfo {
  public:
  LineInfo(string source_name, int pos_start, int pos_end, int line) : source_name_(source_name),
      pos_start_(pos_start),
      pos_end_(pos_end),
      line_(line) {}
    LineInfo() {}
    int GetPosStart();
    int GetPosEnd();
    int GetLine();
    string GetSourceName();
    
  private:
    string source_name_;        
    int pos_start_;
    int pos_end_;
    int line_;
  };

  /**
   * Interface for tokens 
   */
  class Token {
  public:
  Token(string source, string text, int pos_start, int pos_end, int line) : text_(text), info_(LineInfo(source, pos_start, pos_end, line)) {}

    virtual string GetText() {
      return text_;
    }

    virtual LineInfo GetLineInfo() { return info_; }

    virtual bool Eof() = 0;

    virtual void Print() {
      cout << text_ << endl;
    }

    /** ID | NUMBER | STRING | KEYWORD | BRACE | EOF*/
    virtual string TokenType() = 0;

  private:
    string text_;
    LineInfo info_;

  };

  /** 
   * Identifier token 
   */
  class IDToken : public Token {
  public:
  IDToken(string source, string text, int pos_start, int pos_end, int line) : 
    Token(source, text, pos_start, pos_end, line) 
      {}
    virtual bool Eof();
    virtual string TokenType();
  };

  /**
   * Numeric token (represents natural numbers)  
   */
  class NumberToken : public Token {
  public:
  NumberToken(string source, string text, int pos_start, int pos_end, int line) : 
    Token(source, text, pos_start, pos_end, line) 
      {}
    virtual bool Eof();
    virtual string TokenType();
  };

  /**
   * String token (backspace characters not allowed) 
   */
  class StringToken : public Token {
  public:
  StringToken(string source, string text, int pos_start, int pos_end, int line) : 
    Token(source, text, pos_start, pos_end, line) 
      {}
    virtual bool Eof();
    virtual string TokenType();
  };

  /**
   * This token represnts all the keywords in the language (e.g. "define", "lambda", etc.) 
   */
  class KeywordToken : public Token {
  public:
  KeywordToken(string source, string text, int pos_start, int pos_end, int line) : 
    Token(source, text, pos_start, pos_end, line) 
      {}
    virtual bool Eof();
    virtual string TokenType();
  };

  /**
   * Open and close brace.  
   */
  class BraceToken : public Token {
  public:
  BraceToken(string source, string text, int pos_start, int pos_end, int line) : 
    Token(source, text, pos_start, pos_end, line) 
      {}
    bool Open();
    bool Closed();
    virtual bool Eof();
    virtual string TokenType();
  };

  /**
   * End of file token. 
   */
  class EOFToken : public Token {
  public:
  EOFToken(string source, int pos_start, int pos_end, int line) : 
    Token(source, T_EOF, pos_start, pos_end, line) 
      {}
    virtual bool Eof();
    virtual string TokenType();
  };

}
