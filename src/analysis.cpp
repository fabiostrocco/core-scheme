#import "parser.h"
#import "analysis.h"

using namespace ast;

namespace cscheme {
  
  /**
   * Provides instructions on how to bind external variables
   */
  class Binding {
  public:
    Binding(string name, string fName, int arguments) :
      name_(name), fname_(fName), arguments_(arguments) {}

    string GetName() {return name_;}
    string GetFunctionName() {return fname_;}
    int GetArguments() {return arguments_;}

  private:
    //The access name
    string name_;
    //The corresponding function name
    string fname_;
    //The number of arguments
    int arguments_;
  };
  
  /**
   * Performs lexical analysis.
   */
  class AnalysisVisitor : public SimpleAstVisitor {
  public:
    AnalysisVisitor() {}

    virtual void* VisitLoadDirective(AstLoadDirective* import, void* arg) {
      import->GetProgram()->Accept(this, arg);
      return NULL;
    }

    virtual void* VisitDefinition(AstDefinition* definition, void* arg) {
      Environment* env = static_cast<Environment*>(arg);
      global_environment_.AddEntry(definition->GetVariable()->GetID(), false, definition);
      definition->GetVariable()->SetAccess(Lookup(definition->GetVariable()->GetID()->GetText(), *env, definition->GetVariable()));
      definition->GetBody()->Accept(this, arg);
      return NULL;
    }

    virtual void* VisitNumberExpression(AstNumberExpression* number_expression, void* arg) {
      return NULL;
    }

    virtual void* VisitVariableExpression(AstVariableExpression* variable_expression, void* arg) {
      Environment* env = static_cast<Environment*>(arg);
      Access* acc = Lookup(variable_expression->GetID()->GetText(), *env, variable_expression);
      ASSERT(acc != NULL);
      variable_expression->SetAccess(acc);
      return NULL;
    }

    virtual void* VisitLambdaAbstraction(AstLambdaAbstraction* lambda_expression, void* arg) {
      Environment* env = new Environment();
      env->ResetLocalCounter();
      for(AstVariableExpression* var : *lambda_expression->GetParameters()) {
	env->AddEntry(var->GetID(), true, var);
	var->SetAccess(Lookup(var->GetID()->GetText(), *env, var));
      }

      lambda_expression->GetBody()->Accept(this, env);
      return NULL;
    }

    virtual void* VisitLambdaApplication(AstLambdaApplication* lambda_application, void* arg) {
      lambda_application->GetFunction()->Accept(this, arg);

      for(AstExpression* exp : *lambda_application->GetArguments()) {
	exp->Accept(this, arg);
      }

      return NULL;
    }

    virtual void* VisitIfExpression(AstIfExpression* if_expression, void* arg) {
      if_expression->GetCondition()->Accept(this, arg);
      if_expression->GetTrueBranch()->Accept(this, arg);
      if_expression->GetFalseBranch()->Accept(this, arg);
      return NULL;
    }

    virtual void* VisitSetExpression(AstSetExpression* set_expression, void* arg) {
      Environment* env = static_cast<Environment*>(arg);
      set_expression->GetVariable()->Accept(this, arg);
      set_expression->GetValue()->Accept(this, arg);
      return NULL;
    }

  private:
    Environment global_environment_ = Environment();
    Access* Lookup(string name, Environment local_environment, AstNode* node) {
      Access* result;

      if((result = local_environment[name]) == NULL && (result = global_environment_[name]) == NULL) {
	PrintError(node, "Variable " + name + " not found");
      }

      return result;
    }
  };

  void Analyzer::Analyze() {
    AnalysisVisitor* visitor = new AnalysisVisitor();
    unit_->Accept(visitor, new Environment());
  }
  
};

