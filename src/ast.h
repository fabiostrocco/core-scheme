/** 
 * This file contains the AST, Visitor, and Access definitions (the latter mapping variables to
 * an internal representation suitable for intermediate languages).
 */

#include <string>
#include <list>
#include <vector>
#include <sstream>
#include "tokens.h"

using namespace tokens;
using namespace std;

namespace ast {

  /**
   * The following class are pre-defined to allow forward referencing, since they are mutually recursively defined with SimpleAstVisitor 
   */
  class AstCompilationUnit;
  class AstLoadDirective;
  class AstDefinition;
  class AstExpression;
  class AstNumberExpression;
  class AstVariableExpression;
  class AstLambdaAbstraction;
  class AstLambdaApplication;
  class AstSetExpression;
  class AstIfExpression;

  /**
   * Simple non recursive Ast Visitor 
   */
  class SimpleAstVisitor {
  public:
    virtual void* VisitLoadDirective(AstLoadDirective* load, void* arg) = 0;
    virtual void* VisitDefinition(AstDefinition* definition, void* arg) = 0;
    virtual void* VisitNumberExpression(AstNumberExpression* number_expression, void* arg) = 0;
    virtual void* VisitVariableExpression(AstVariableExpression* variable_expression, void* arg) = 0;
    virtual void* VisitLambdaAbstraction(AstLambdaAbstraction* lambda_expression, void* arg) = 0;
    virtual void* VisitLambdaApplication(AstLambdaApplication* lambda_application, void* arg) = 0;
    virtual void* VisitIfExpression(AstIfExpression* if_expression, void* arg) = 0;
    virtual void* VisitSetExpression(AstSetExpression* set_expression, void* arg) = 0;
  };

  /**
   * Specifies an access to a variable, that is
   * how to access to such variable from LLVM or the interpreter.
   *
   * There are three kinds of access
   * 1. Local variables: each local variable has a numeric access, representing its
   *    offset in the stack
   * 2. Global access: each global variable has a string access, representing a
   *    suitable name for an intermediate language, such as LLVM, or an interpreter.
   * 3. External access: like global access, but the variable is not defined in the AST;
   *    it is a predefined variable (such as the write and read functions) instead.
   */
  class Access {
  public:
    virtual string ToString() = 0;
    virtual string GetIdentifier() = 0;
    virtual bool IsGlobal() = 0;
  };

  /**
   * Local function parameter access.
   */
  class LocalAccess: public Access {
  public:
  LocalAccess(int offset) : offset_(offset) {}
    
    int GetOffset();
    virtual string ToString();
    virtual string GetIdentifier();
    virtual bool IsGlobal();

  private:
    int offset_;
  };

  /**
   * Global variable access, or external access.
   */
  class GlobalAccess : public Access {
  public:
  GlobalAccess(string name, bool def = false) : def_(def) {
      this->name_ = Canonical(name);
    }
    string GetName();
    virtual string ToString();
    virtual string GetIdentifier();
    virtual bool IsGlobal();
    
  private:
    string name_;
    bool def_;

    string Canonical(string str);
  };

  /**
   * Represents a generic AST node.
   */
  class AstNode {
  public:
  AstNode(LineInfo info) : info_(info) {}

    virtual void* Accept(SimpleAstVisitor* visitor, void* args) = 0;
    virtual LineInfo GetInfo() {return info_;}

  private:
    LineInfo info_;
  };

  /** 
   * Represents a generic AST expression node.
   */
  class AstExpression : public AstNode {
  public:
  AstExpression(LineInfo info) : AstNode(info) {}
    virtual bool IsNumber() = 0;
    virtual bool IsLambda() = 0;
  };

  /**
   * (load [string path_])
   */
  class AstLoadDirective : public AstNode {
  public:
  AstLoadDirective(string path, LineInfo info) : path_(path), AstNode(info) {}
  
    virtual string GetPath();
    virtual FILE* GetFile();
    virtual void SetProgram(AstCompilationUnit* program);
    virtual AstCompilationUnit* GetProgram();
    virtual void* Accept(SimpleAstVisitor* visitor, void* args);

  private:  
    string path_;
    AstCompilationUnit* program_;
    
  };

  /**
   * (define [AstVariableExpression variable_] [AstExpression body_])
   */
  class AstDefinition : public AstNode {
  public:
  AstDefinition(AstVariableExpression* variable, AstExpression* body, LineInfo info):
    variable_(variable), body_(body), AstNode(info) {}

    AstVariableExpression* GetVariable() { return variable_; }

    AstExpression* GetBody() { return body_; }

    virtual void* Accept(SimpleAstVisitor* visitor, void* args) {
      return visitor->VisitDefinition(this, args);
    }

  private:  
    AstVariableExpression* variable_;
    AstExpression* body_;

  };

  /**
   * Instances of this class represent a source file.
   * [AstLoadDirective imports_] list
   * [AstDefinition definitions_] list
   * [AstExpression] main_
   */
  class AstCompilationUnit : public AstNode {
  public:
  AstCompilationUnit(list<AstLoadDirective*>* imports, list<AstDefinition*>* definitions, AstExpression* main, LineInfo info)
    : imports_(imports), definitions_(definitions), main_(main), AstNode(info) {}
    list<AstLoadDirective*>* GetImports() { return imports_; }
    list<AstDefinition*>* GetDefinitions() { return definitions_; }
    AstExpression* GetMain() { return main_; }
    
    virtual void* Accept(SimpleAstVisitor* visitor, void* args);
    
  private:
    list<AstLoadDirective*>* imports_;
    list<AstDefinition*>* definitions_;
    AstExpression* main_;
  };

  /**
   * [int n_]
   */
  class AstNumberExpression : public AstExpression {
  public:
  AstNumberExpression(NumberToken* n, LineInfo info) : n_(n), AstExpression(info) {}
    
    NumberToken* GetNumber() { return n_; }
    virtual void* Accept(SimpleAstVisitor* visitor, void* args) {
      return visitor->VisitNumberExpression(this, args);
    }
    
    virtual bool IsNumber() { return true; }
    virtual bool IsLambda() { return false; }
    
  private:
    NumberToken* n_;
  };

  /** 
   * Represetns a variable. This node also contains access informations.
   * [IDToken id_]
   */
  class AstVariableExpression : public AstExpression {
  public:
  AstVariableExpression(IDToken* id, LineInfo info) : id_(id), AstExpression(info) {}
  
    IDToken* GetID() { return id_; }
    void SetAccess(Access* access) {
      this->access_ = access;
    }

    Access* GetAccess() {
      return access_;
    }

    virtual void* Accept(SimpleAstVisitor* visitor, void* args) {
      return visitor->VisitVariableExpression(this, args);
    }

    virtual bool IsNumber() { return false; }
    virtual bool IsLambda() { return false; }

  private:
    IDToken* id_;
    Access* access_;
  };

  /**
   * (lambda ([AstVariableExpression parameters_] list) [AstExpression body_])
   */
  class AstLambdaAbstraction : public AstExpression {
  public:
  AstLambdaAbstraction(list<AstVariableExpression*>* parameters,
                       AstExpression* body, LineInfo info) : parameters_(parameters),
      body_(body), AstExpression(info) {}

    list<AstVariableExpression*>* GetParameters() { return parameters_; }
    vector<Access*> GetParameterAccessList();
    AstExpression* GetBody() { return body_; }
    virtual void* Accept(SimpleAstVisitor* visitor, void* args) {
      return visitor->VisitLambdaAbstraction(this, args);
    }
    
    virtual bool IsNumber() { return false; }
    virtual bool IsLambda() { return true; }

  private:
    list<AstVariableExpression*>* parameters_;
    AstExpression* body_;
  };

  /** 
   * ([AstExpression fun_] [AstExpression args_] list)
   */
  class AstLambdaApplication : public AstExpression {
  public:
  AstLambdaApplication(AstExpression* fun, list<AstExpression*>* args, LineInfo info)
    : fun_(fun), args_(args), AstExpression(info) {}

    AstExpression* GetFunction() { return fun_; }
    list<AstExpression*>* GetArguments() { return args_; }

    virtual void* Accept(SimpleAstVisitor* visitor, void* args) {
      return visitor->VisitLambdaApplication(this, args);
    }

    virtual bool IsNumber() { return false; }
    virtual bool IsLambda() { return false; }

  private:
    AstExpression* fun_;
    list<AstExpression*>* args_;
  };

  /**
   * (set! [AstVariableExpression var_] [AstExpression value_])
   */
  class AstSetExpression : public AstExpression {
  public:
  AstSetExpression(AstVariableExpression* var, AstExpression* value, LineInfo info) :
    var_(var), value_(value), AstExpression(info) {}

    AstVariableExpression* GetVariable() { return var_; }
    AstExpression* GetValue() { return value_; }

    virtual void* Accept(SimpleAstVisitor* visitor, void* args) {
      return visitor->VisitSetExpression(this, args);
    }

    virtual bool IsNumber() { return false; }
    virtual bool IsLambda() { return false; }

  private:
    AstVariableExpression* var_;
    AstExpression* value_;
  };

  /** 
   * (if [AstExpression condition] [AstExpression true_branch_] [AstExpression false_branch_])
   */
  class AstIfExpression : public AstExpression {
  public:
  AstIfExpression(AstExpression* condition, AstExpression* true_branch,
		  AstExpression* false_branch, LineInfo info) : condition_(condition),
      true_branch_(true_branch),
      false_branch_(false_branch),
      AstExpression(info) {}

    AstExpression* GetCondition() { return condition_; }
    AstExpression* GetTrueBranch() { return true_branch_; }
    AstExpression* GetFalseBranch() { return false_branch_; }

    virtual void* Accept(SimpleAstVisitor* visitor, void* args) {
      return visitor->VisitIfExpression(this, args);
    }

    virtual bool IsNumber() { return false; }
    virtual bool IsLambda() { return false; }

  private:
    AstExpression* condition_;
    AstExpression* true_branch_;
    AstExpression* false_branch_;
  };

};
