/** 
 * This file contains the lexical analyzer, used to map varibles in the AST to
 * an internal representation used for LLVM compilation or for the interpreter.
 * The internal representation is provided by the class Access.
 */
#include <map>
#include <vector>
#include "error.h"

/*
 * CONSTRAINTS:
 *   1. Escaping variables are not supported, i.e. (lambda (x) (lambda (y) (+ x y))) is illegal
 */

using namespace ast;

namespace cscheme {

  /** 
   * Binding variable |-> static scoping informatons
   */
  class Environment {
  public:
    Environment() {
      ResetLocalCounter();
      mapping_ = map<string, Access*>();
      //Adding +,-,*,/,=,<,>,read,write
      mapping_["+"] = new GlobalAccess("plus", true);
      mapping_["-"] = new GlobalAccess("minus", true);
      mapping_["*"] = new GlobalAccess("times", true);
      mapping_["/"] = new GlobalAccess("divide", true);
      mapping_["="] = new GlobalAccess("equals", true);
      mapping_["<"] = new GlobalAccess("less", true);
      mapping_[">"] = new GlobalAccess("greater", true);
      mapping_["read"] = new GlobalAccess("input", true);
      mapping_["write"] = new GlobalAccess("output", true);
      ASSERT(counter_ == 0);
    }

    void ResetLocalCounter() { counter_ = 0; }

    static vector<string> GetExternalBindings() {
      vector<string> result;
      Environment env = Environment();
      result.push_back(env.mapping_["+"]->GetIdentifier());
      result.push_back(env.mapping_["-"]->GetIdentifier());
      result.push_back(env.mapping_["*"]->GetIdentifier());
      result.push_back(env.mapping_["/"]->GetIdentifier());
      result.push_back(env.mapping_["="]->GetIdentifier());
      result.push_back(env.mapping_["<"]->GetIdentifier());
      result.push_back(env.mapping_[">"]->GetIdentifier());
      result.push_back(env.mapping_["read"]->GetIdentifier());
      result.push_back(env.mapping_["write"]->GetIdentifier());
      return result;
    }
    
    void AddEntry(IDToken* id, bool local, AstNode* node) {
      ASSERT(counter_ >= 0);
      auto key = id->GetText();
      if(mapping_.count(key) != 0) {
	PrintError(node, "Duplicated variable " + id->GetText());
      }

      if(local) {
	mapping_[key] = new LocalAccess(counter_++);
      } else {
	mapping_[key] = new GlobalAccess(key);
      }
    }

    Access* operator [](string key) {
      return mapping_[key];
    }

  private:    
    map<string, Access*> mapping_;
    //Counter for local variables (generates fresh names)
    int counter_ = 0;
  };

  
  class Analyzer {
  public:
  Analyzer(AstCompilationUnit* unit) : unit_(unit) {}
   
    void Analyze();
    
  private:
    AstCompilationUnit* unit_;
  };
};
