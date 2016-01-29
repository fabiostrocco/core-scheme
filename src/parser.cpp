/* 
   Syntax
   -----------------------------------------------------------------------
   P ::= L* D* e
   L ::= (load "PATH")
   D ::= (define x e)

   e ::= N | x | (lambda (x...x) e) | (e ... e) | (if e e e) | (set! x e)

   default: +,-,*,/,=,<,>,read,write

*/

#include "parser.h"
#include "error.h"

using namespace std;
using namespace ast;

namespace cscheme {

  void PrintError(AstNode* node, string message) {
    if(node == NULL) {
      cerr <<"Error: " << message << endl;
    } else {
      LineInfo info = node->GetInfo();
      cerr << info.GetSourceName() << ":" << info.GetLine() << ":" << info.GetPosStart() << ": error: " << message << endl;
    }
    exit(EXIT_PARSE_ERROR);
  }

  /*
   * PARSER IMPLEMENTATION
   */

  AstCompilationUnit* Parser::Parse(bool force_main) {
    LineInfo info;
    list<AstLoadDirective*>* imports = new list<AstLoadDirective*>();
    list<AstDefinition*>* defs = new list<AstDefinition*>();
    bool skip = false;
    
    NextToken();
    info = token_->GetLineInfo();
    if(token_->Eof()) {
      if(force_main) {
	lexer_.PrintError("Empty file not allowed: main expression is required");
      } else {
	lexer_.PrintError("Empty file not allowed: at least a main expression, variable declaration, or library import is required");
      }
    }

    if(!skip && NextToken()->Eof()) {
      BackTrack(2);
      skip = true;
    }

    //Parse L*
    if(!skip) {
      while(token_->GetText().compare("load") == 0) {
	if(NextToken()->TokenType().compare(STRING) == 0) {
	  imports->push_back(new AstLoadDirective(token_->GetText(), info));
	} else {
	  lexer_.PrintError("String token expected, " + token_->TokenType() + " found");
	}

	if(NextToken()->GetText().compare(")") != 0) {
	  lexer_.PrintError(") expected, " + token_->GetText() + " found");
	}
	
	if(NextToken()->Eof()) { 
	  if(force_main) {
	    lexer_.PrintError("Main expression expected, EOF found"); 
	  } else {
	    skip = true;
	    break;
	  }
	}
	info = token_->GetLineInfo();
	if(NextToken()->Eof()) {
	  BackTrack(2);
	  skip = true;
	  break;
	}
      }
    }
    
    //Parse D*
    if(!skip) {
      while(token_->GetText().compare("define") == 0) {
	defs->push_back(ParseDefinition(info));
	if(NextToken()->GetText().compare(")") != 0) {
	  lexer_.PrintError(") expected, " + token_->GetText() + " found");
	}
	
	if(NextToken()->Eof()) { 
	  if(force_main) {
	    lexer_.PrintError("Main expression expected, EOF found");
	  } else {
	    skip = true;
	    break;
	  }
	}
	info = token_->GetLineInfo();
	if(NextToken()->Eof()) {
	  BackTrack(2);
	  skip = true;
	  break;
	}
      }
      
      if(!skip) BackTrack(2);
    }

    //Parse e
    if(!token_->Eof()) {
      AstExpression* exp = ParseExpression(info);
      AstCompilationUnit* unit = new AstCompilationUnit(imports, defs, exp, LineInfo());
      return unit;
    } else {
      if(force_main) {
	lexer_.PrintError("Main expression expected, EOF found");
      } else {
	AstCompilationUnit* unit = new AstCompilationUnit(imports, defs, NULL, LineInfo());
	return unit;
      }
      return NULL;
    }
  }

  /*
   * Load the libraries (if any) referenced by this compilation unit.
   */
  void Parser::Resolve(AstCompilationUnit* unit, unordered_set<string> visited) {
    list<AstLoadDirective*>* imports = unit->GetImports();
    for(AstLoadDirective* import : *imports) {
      string path = import->GetPath();
      if(visited.count(path) != 0) {
	lexer_.PrintError("Cyclic reference " + path);
      }
      visited.insert(path);
      FILE* source = import->GetFile();
      Lexer lexer = Lexer(source, path);
      Parser* parser = new Parser(lexer);
      AstCompilationUnit* lib = parser->Parse(false);
      import->SetProgram(lib);
      parser->Resolve(lib, visited);
    }
  }

  //Parse ID e
  AstDefinition* Parser::ParseDefinition(LineInfo info) {
    Token* id = NextToken();
    if(token_->TokenType().compare(T_ID) != 0) {
      lexer_.PrintError("ID expected, " + token_->GetText() + " found");
    }

    AstExpression* body = ParseExpression(token_->GetLineInfo());
    AstDefinition* def = new AstDefinition(new AstVariableExpression(static_cast<IDToken*>(id), info), body, info);
    return def;
  }

  //e ::= N | x | (lambda (x...x) e) | (e ... e) | (if e e e) | (set! x e)
  AstExpression* Parser::ParseExpression(LineInfo info) {
    NextToken();
    if(token_->Eof()) {
      lexer_.PrintError("Expression expected, EOF found");
      return NULL;
    }

    if(token_->TokenType().compare(NUMBER) == 0) {
      return new AstNumberExpression(static_cast<NumberToken*>(token_), info);
    } else if(token_->TokenType().compare(T_ID) == 0) {
      return new AstVariableExpression(static_cast<IDToken*>(token_), info);
    } else if(token_->GetText().compare("(") == 0) {
      NextToken();
      if(token_->GetText().compare("lambda") == 0) {
	return ParseLambdaAbstraction(info);
      } else if(token_->GetText().compare("if") == 0) {
	return ParseIfExpression(info);
      } else if(token_->GetText().compare("set!") == 0) {
	return ParseSetExpression(info);
      } else {
	BackTrack(1);
	return ParseLambdaApplication(info);
      }
    } else {
      lexer_.PrintError("( expected, " + token_->GetText() + " found");
    }

    return NULL;
  }

  // (lambda >(x...x) e)
  AstLambdaAbstraction* Parser::ParseLambdaAbstraction(LineInfo info) {
    list<AstVariableExpression*>* vars = new list<AstVariableExpression*>();
    NextToken();
    if(token_->GetText().compare("(") == 0) {
      NextToken();
      while(token_->TokenType().compare(T_ID) == 0) {
	vars->push_back(new AstVariableExpression(static_cast<IDToken*>(token_), info));
	NextToken();
      }
    } else {
      lexer_.PrintError("( expected, " + token_->GetText() + " found");
      return NULL;
    }

    if(token_->GetText().compare(")") != 0) {
      lexer_.PrintError(") expected, " + token_->GetText() + " found");
      return NULL;
    }

    AstExpression* exp = ParseExpression(token_->GetLineInfo());
    if(NextToken()->GetText().compare(")") != 0) {
      lexer_.PrintError(") expected, " + token_->GetText() + " found");
      return NULL;
    }

    return new AstLambdaAbstraction(vars, exp, info);
  }

  // (if >e e e)
  AstIfExpression* Parser::ParseIfExpression(LineInfo info) {
    AstExpression *cond, *trBranch, *flBranch;

    cond = ParseExpression(token_->GetLineInfo());
    trBranch = ParseExpression(token_->GetLineInfo());
    flBranch = ParseExpression(token_->GetLineInfo());
    NextToken();
    if(token_->GetText().compare(")") != 0) {
      lexer_.PrintError(") expected, " + token_->GetText() + " found");
      return NULL;
    }

    return new AstIfExpression(cond, trBranch, flBranch, info);
  }

  // (>e e...e)
  AstLambdaApplication* Parser::ParseLambdaApplication(LineInfo info) {
    AstExpression* f = ParseExpression(info);
    list<AstExpression*>* args = new list<AstExpression*>();
    NextToken();
    while(token_->GetText().compare(")") != 0 && !token_->Eof()) {
      BackTrack(1);
      AstExpression* exp = ParseExpression(token_->GetLineInfo());
      args->push_back(exp);
      NextToken();
    }

    if(token_->GetText().compare(")") != 0) {
      lexer_.PrintError(") expected, " + token_->GetText() + " found");
      return NULL;
    }

    return new AstLambdaApplication(f, args, info);
  }

  // (set! >x e)
  AstSetExpression* Parser::ParseSetExpression(LineInfo info) {
    AstVariableExpression* var;
    AstExpression* value;

    NextToken();

    if(token_->TokenType().compare(T_ID) == 0) {
      var = new AstVariableExpression(static_cast<IDToken*>(token_), info);
    } else {
      lexer_.PrintError("ID expected, " + token_->GetText() + " found");
      return NULL;
    }

    value = ParseExpression(info);

    NextToken();
    if(token_->GetText().compare(")") != 0) {
      lexer_.PrintError(") expected, " + token_->GetText() + " found");
      return NULL;
    }

    return new AstSetExpression(var, value, info);
  }

};
