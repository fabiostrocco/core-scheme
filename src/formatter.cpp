#include <iostream>
#import "parser.h"
#include "formatter.h"

using namespace std;

namespace cscheme {

  /*
   * Format the source code and prints variable access informations.
   */
  class FormatterVisitor : public SimpleAstVisitor {
  public:
    FormatterVisitor() {}

    virtual void* VisitLoadDirective(AstLoadDirective* import, void* arg) {
      cout << "Import: " << import->GetPath() << endl;
      import->GetProgram()->Accept(this, NOTAB);
      cout << "END OF IMPORT" << endl;
      return NULL;
    }

    virtual void* VisitDefinition(AstDefinition* definition, void* arg) {
      string* args = static_cast<string*>(arg);
      string* tab = new string(*args + TAB);
      PrintCode("(define " + definition->GetVariable()->GetID()->GetText() + " ");
      PrintIndent(tab);
      definition->GetBody()->Accept(this, tab);
      PrintCode(")\n\n");
      return NULL;
    }

    virtual void* VisitNumberExpression(AstNumberExpression* number_expression, void* arg) {
      string* args = static_cast<string*>(arg);
      PrintCode(number_expression->GetNumber()->GetText());
      return NULL;
    }

    virtual void* VisitVariableExpression(AstVariableExpression* variable_expression, void* arg) {
      string* args = static_cast<string*>(arg);
      PrintCode("<var(" + variable_expression->GetID()->GetText() + ") : " +
		variable_expression->GetAccess()->ToString() + ">");
      return NULL;
    }

    virtual void* VisitLambdaAbstraction(AstLambdaAbstraction* lambda_expression, void* arg) {
      string* args = static_cast<string*>(arg);
      PrintCode("(lambda (");
      int index = 0;

      for(AstVariableExpression* var : *lambda_expression->GetParameters()) {
	PrintCode(var->GetID()->GetText() + (index == 0 ? "" : " "));
	index++;
      }

      string* tab = new string((*args) + TAB);
      PrintCode(")");
      PrintIndent(tab);
      lambda_expression->GetBody()->Accept(this, tab);
      PrintCode(")");
      return NULL;
    }

    virtual void* VisitLambdaApplication(AstLambdaApplication* lambda_application, void* arg) {
      string* args = static_cast<string*>(arg);
      PrintCode("(");
      lambda_application->GetFunction()->Accept(this, arg);
      PrintCode(" ");
    
      for(AstExpression* exp : *lambda_application->GetArguments()) {
	exp->Accept(this, arg);
	PrintCode(" ");
      }
    
      PrintCode(")");
      return NULL;
    }

    virtual void* VisitIfExpression(AstIfExpression* if_expression, void* arg) {
      string* args = static_cast<string*>(arg);
      string* tab = new string(*args + "    ");
      PrintCode("(if ");
      if_expression->GetCondition()->Accept(this, args);
      PrintIndent(tab);
      if_expression->GetTrueBranch()->Accept(this, tab);
      PrintIndent(tab);
      if_expression->GetFalseBranch()->Accept(this, tab);
      PrintCode(")");
      return NULL;
    }

    virtual void* VisitSetExpression(AstSetExpression* set_expression, void* arg) {
      string* args = static_cast<string*>(arg);
      PrintCode("(set! ");
      set_expression->GetVariable()->Accept(this, arg);
      PrintCode(" ");
      set_expression->GetValue()->Accept(this, arg);
      PrintCode(")");
      return NULL;
    }

  private:
    void PrintCode(string st) { cout << st; }
    void PrintIndent(string* tab) { cout << endl << *tab; }

  };

  void Formatter::Print()  {
    FormatterVisitor* visitor = new FormatterVisitor();
    unit_->Accept(visitor, new string(""));
  }

};
