#include "tokens.h"

namespace tokens {

  /* CLASS LineInfo */

  int LineInfo::GetPosStart() { return pos_start_; }
  int LineInfo::GetPosEnd() { return pos_end_; }
  int LineInfo::GetLine() { return line_; }
  string LineInfo::GetSourceName() { return source_name_; }


  /* CLASS IdToken */

  bool IDToken::Eof() { return false; }
  string IDToken::TokenType() { return T_ID; }

  /* CLASS NumberToken */

  bool NumberToken::Eof() { return false; }
  string NumberToken::TokenType() { return NUMBER; }

  /* CLASS StringToken */

  bool StringToken::Eof() { return false; }
  string StringToken::TokenType() { return STRING; }

  /* CLASS KeyworkToken */

  bool KeywordToken::Eof() { return false; }
  string KeywordToken::TokenType() { return KEYWORD; }
  
  /* CLASS BraceToken */

  bool BraceToken::Open() { return GetText() == "(" || GetText() == "["; }
  bool BraceToken::Closed() { return GetText() == ")" || GetText() == "]"; }
  bool BraceToken::Eof() { return false; }
  string BraceToken::TokenType() { return BRACE; }

  /* CLASS EOFToken */

  bool EOFToken::Eof() { return true; }
  string EOFToken::TokenType() { return T_EOF; }

};
