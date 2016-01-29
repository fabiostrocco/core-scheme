#include <string>
#include <vector>
#include "ast.h"
#include "error.h"

using namespace std;

namespace ast {

  /* CLASS LocalAccess */
 
  string LocalAccess::ToString() {
    ostringstream ostr;
    ostr << offset_;
    return "local_" + ostr.str();
  }
  string LocalAccess::GetIdentifier() {
    return ToString();
  }
  bool LocalAccess::IsGlobal() {
    return false;
  }
  int LocalAccess::GetOffset() {
    return offset_;
  }

  /* CLASS GlobalAccess */

  string GlobalAccess::GetName() {return (def_ ? "default_" : "global_") + name_;}
  string GlobalAccess::ToString() {return "Global(" + GetName() + ")";}
  string GlobalAccess::GetIdentifier() {return GetName();}
  bool GlobalAccess::IsGlobal() {return true;}
  string GlobalAccess::Canonical(string str) {
    string res = "";
    int idx = 0;
    const char *it = str.c_str();

    while(*it != '\0') {
      if((*it >= 'a' && *it <= 'z') ||
	 (*it >= 'A' && *it <= 'Z')) {
	res = res + *it;
      } else {
	res = res + to_string(*it);
      }
      it++;
    }

    return res;
  }


  /* CLASS AstLoadDirective */

  string AstLoadDirective::GetPath() { return path_; }
  
  FILE* AstLoadDirective::GetFile() {
    const char* path_chr_a = static_cast<const char*>(path_.c_str());
    FILE* source = fopen(path_chr_a, "r");
    
    if(source == NULL) {
      cerr << "Cannot Resolve file " << path_ << endl;
      exit(EXIT_FILE_RESOLUTION_FAILED);
    }

    return source;
  }

  void AstLoadDirective::SetProgram(AstCompilationUnit* program) {
    this->program_ = program;
  }

  AstCompilationUnit* AstLoadDirective::GetProgram() {
    return program_;
  }

  void* AstLoadDirective::Accept(SimpleAstVisitor* visitor, void* args) {
    return visitor->VisitLoadDirective(this, args);
  }

  /* CLASS AstCompilationUnit */
  void* AstCompilationUnit::Accept(SimpleAstVisitor* visitor, void* args) {
    void* res = NULL;

    for(AstLoadDirective* load : *imports_) {
      res = load->Accept(visitor, args);
    }

    for (AstDefinition* def : *definitions_) {
      res = def->Accept(visitor, args);
    }

    if(main_ != NULL) {
      res = main_->Accept(visitor, args);
    }

    return res;
  }

  /* CLASS AstLambdaAbstraction*/
  vector<Access*> AstLambdaAbstraction::GetParameterAccessList() {
    vector<Access*> result;
    for(AstVariableExpression* parameter : *parameters_) {
      result.push_back(parameter->GetAccess());
    }
    return result;
  }

};
