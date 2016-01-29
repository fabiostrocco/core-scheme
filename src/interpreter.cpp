#include <map>

#include "parser.h"
#include "interpreter.h"
#include "error.h"

namespace cscheme {

  void Debug(string s) {
    cout << s << endl; 
  }

  class AstValue;

  class RuntimeEnvironmentInt {
  public:
    virtual void CreateAllocationFrame(int size) = 0;
    virtual void DestroyAllocationFrame() = 0;
    virtual void Assign(Access* access, AstValue* node)  = 0;
  };

  /**
   * This class represents the super-class of all the runtime values.
   * Runtime values v are represented in the AST form.
   *
   * v ::= AstNodeValue | DefaultFunctionValue | UnitValue | SpecialValue 
   * AstNodeValue ::= number | function
   * SpecialValue ::= BasePointerValue | NumericValue
   * DefulatFunctionValue ::= the representation of a predefined function
   */
  class AstValue {
  public:
    /**
     * Return true if the value represents a valid boolean condition, false
     * otherwise.
     */
    virtual bool IsValidCondition() = 0;

    /**
     * Return true if the value represents a valid number, false
     * otherwise.
     */
    virtual bool IsNumber() = 0;

    /**
     * Return true if the value represents a valid lambda abstraction, false
     * otherwise.
     */
    virtual bool IsLambda() = 0;

    /**
     * Return the boolean value associated with this variable.
     */
    virtual bool GetCondition() = 0;

    /**
     * Return the number associated with this variable.
     */
    virtual int GetNumber() = 0;

    /**
     * Evaluates the current value (assuming it is a function) given an environment and
     * a list of arguments.
     */
    virtual AstValue* Eval(RuntimeEnvironmentInt* env, vector<AstValue*> arguments) = 0;
  };

  /** 
   * This class represetns an AST value, i.e. number or lambda abstraction.
   */
  class AstNodeValue : public AstValue { 
  public:
    AstNodeValue(AstNode* node, SimpleAstVisitor* visitor) : 
      inner_node_(node), visitor_(visitor) {}

    virtual bool IsValidCondition() {
      AstExpression* node = static_cast<AstExpression*>(inner_node_);
      return node->IsNumber();
    }
  
    virtual bool IsNumber() {
      AstExpression* node = static_cast<AstExpression*>(inner_node_);
      return node->IsNumber();
    }

    virtual bool IsLambda() {
      AstExpression* node = static_cast<AstExpression*>(inner_node_);
      return node->IsLambda();
    }
  
    virtual bool GetCondition() {
      AstNumberExpression* num = static_cast<AstNumberExpression*>(inner_node_);
      return GetNumber() != 0;
    }

    virtual int GetNumber() {
      AstNumberExpression* num = static_cast<AstNumberExpression*>(inner_node_);
      string numstr = num->GetNumber()->GetText();
      stringstream str(numstr);
      int res = 0;
      if (str >> res) {
	return res;
      } else {
	Bug("NUMBER token is not a number");
      }

      return -1;
    }

    virtual AstValue* Eval(RuntimeEnvironmentInt* env, vector<AstValue*> arguments) {
      AstLambdaAbstraction* function = static_cast<AstLambdaAbstraction*>(inner_node_);
      int frame_size = arguments.size();
      env->CreateAllocationFrame(frame_size);
      vector<Access*> parameters = function->GetParameterAccessList();
    
      if(parameters.size() != arguments.size()) {
	PrintError(function, "Number of declared parameters must be the same as the number of arguments while calling functions");
      }

      for(int i = 0 ; i < frame_size ; i++) {
	env->Assign(parameters[i], arguments[i]);
      }

      void* result = function->GetBody()->Accept(visitor_, NULL);
      env->DestroyAllocationFrame();

      return static_cast<AstValue*>(result);
    }

  private:
    AstNode* inner_node_;
    SimpleAstVisitor* visitor_;
  };

  /**
   * Represents a numeric value computed from an arithmetic operation.
   */
  class NumericValue : public AstValue {
  public:
    NumericValue(int number) : number_(number) {}

    virtual bool IsValidCondition() { return true; }

    virtual bool IsNumber() { return true; }

    virtual bool IsLambda() { return false; }

    virtual bool GetCondition() { return number_ != 0; }

    virtual int GetNumber() { return number_; }

    virtual AstValue* Eval(RuntimeEnvironmentInt* env, vector<AstValue*> arguments) { 
      Bug("cannot call numeric value, function expected");
      return NULL; 
    }

  private:
    int number_;
  };

  /**
   * Represetns a unit value, i.e. a value produced by non integer variables.
   */
  class UnitValue : public AstValue {
  private:
    UnitValue() {}

  public:
    static UnitValue* Create() {
      static UnitValue* instance;
      if(instance == NULL) {
	instance = new UnitValue();
      }

      return instance;

    }

    virtual bool IsValidCondition() {
      return false;
    }

    virtual bool IsNumber() { return false; }

    virtual bool IsLambda() { return false; }
  
    virtual bool GetCondition() {
      Bug("getCondition() should not be called for unit values");
      return true;
    }

    virtual int GetNumber() { return 0; }

    virtual AstValue* Eval(RuntimeEnvironmentInt* env, vector<AstValue*> arguments) {
      PrintError(NULL, "Cannot call the unit value");
      return NULL;
    }
  };

  class DefaultFunctionValue : public AstValue {
  public:
    DefaultFunctionValue(string name) : name_(name) {}
    virtual bool IsValidCondition() { return false;  }

    virtual bool IsNumber() { return false; }
  
    virtual bool IsLambda() { return true; }

    virtual bool GetCondition() {
      Bug("getCondition() should not be called for default function values values");
      return true;
    }

    virtual int GetNumber() { return 0; }

    virtual AstValue* Eval(RuntimeEnvironmentInt* env, vector<AstValue*> arguments) {
      // Compute variadic operations
      if(name_ == "plus" || name_ == "minus" || name_ == "times" || name_ == "divide") {
	return new NumericValue(ComputeNumericExpression(arguments, name_));
      }

      if(name_ == "equals" || name_ == "less" || name_ == "greater") {
	if(arguments.size() == 2) {
	  return new NumericValue(ComputeComparison(arguments[0], arguments[1], name_) ? 1 : 0);
	} else {
	  PrintError(NULL, "Comparison operation " + name_ + " expects exactly 2 arguments");
	}
      }

      if(name_ == "input") {
	if(arguments.size() != 0) {
	  PrintError(NULL, "Input function should have 0 arguments");
	}
	string input;
	int res = 0;
	getline(cin, input);
	stringstream str(input);
	if (str >> res) {
	  return new NumericValue(res);
	} else {
	  cerr << "The input is not an integer" << endl;
	  exit(EXIT_WRONG_INPUT);
	}
      }

      if(name_ == "output") {
	if(arguments.size() == 1) {
	  AstValue* arg0 = arguments[0];
	  if(arg0->IsNumber()) {
	    cout << arg0->GetNumber() << endl;
	  } else if(arg0->IsLambda()) {
	    cout << "#<procedure>" << endl;
	  } else {
	    PrintError(NULL, "Output argument should be a number or a lambda");
	  }
	} else {
	  PrintError(NULL, "Output function should have exactly 1 argument");
	}
	return UnitValue::Create();
      }

      Bug("unrechognized default operation " + name_);
      return NULL;
    }

    /* Auxiliary methods */

    int ComputeNumericExpression(vector<AstValue*> arguments, string operation) {
      int acc = -1;

      for(AstValue* arg : arguments) {
	if(arg->IsNumber()) {
	  if(acc == -1) {
	    acc = arg->GetNumber();
	  } else {
	    if(operation == "plus") {
	      acc += arg->GetNumber();
	    } else if(operation == "minus") {
	      acc -= arg->GetNumber();
	    } else if(operation == "times") {
	      acc *= arg->GetNumber();
	    } else if(operation == "divide") {
	      acc /= arg->GetNumber();
	    } else {
	      Bug("unknown operation " + operation);
	    }
	  }
	} else {
	  PrintError(NULL, "Numeric value expected in arithmetic operation " + name_);
	}
      }

      if(acc == -1) {
	if(operation == "plus" || operation == "minus") acc = 0;
	else acc = 1;
      }
    
      return acc;
    }

    bool ComputeComparison(AstValue* left, AstValue* right, string operation) {
      if(left->IsNumber() && right->IsNumber()) {
	if(operation == "equals") {
	  return (left->GetNumber() == right->GetNumber());
	} else if(operation == "less") {
	  return (left->GetNumber() < right->GetNumber());
	} else if(operation == "greater") {
	  return (left->GetNumber() > right->GetNumber());
	} else {
	  Bug("unknown operation " + operation); 
	}
      } else {
	PrintError(NULL, "Numeric value expected in arithmetic operation " + name_);
      }
    
      return false;
    }

  private:
    string name_;
  };

  /** 
   * Represents a base pointer entry in the stack.
   * This kind of values should only occur in the stack, not in the global environment.
   */
  class BasePointerValue : public AstValue {
  public:
    BasePointerValue(int value) : value_(value) {}
    int GetValue() { return value_; }
    virtual bool IsValidCondition() {
      return false;
    }

    //A base pointer is not a valid number, and if read by some variable
    //by accident, it would be a bug.
    virtual bool IsNumber() { return false; }
  
    virtual bool GetCondition() {
      Bug("GetCondition() should not be called for base pointer values");
      return true;
    }

    virtual int GetNumber() { return 0; }

    virtual bool IsLambda() { return false; }

    virtual AstValue* Eval(RuntimeEnvironmentInt* env, vector<AstValue*> arguments) {
      Bug("cannot call base pointer value");
      return NULL;
    }

  private:
    int value_;
  };

  /*
   * Represents the runtime environmnets, a variable to value mapping.
   */
  class RuntimeEnvironment : public RuntimeEnvironmentInt {
    //Stack structure
    //The stack contains AST values (numbers and functions), special functions, and numberic values to store the old base pointer.
    //Note that the stack does not contain any function pointer or other reference.
    /*
      |  ...                |
      |  TOP_VALUE <--- SP  |
      |  ...                |                the space between BP and SP is used so 
      |  ...                |                that the variable "access" is stored in BP + access->GetOffset()
      |  BOT_VALUE          |
      |  OLD_BP <-- BP      |
      |  ...                |
      |  ... <--- OLD_BP    |
      |  ...                |
      |  0 <--- 0           |
      -----------------------
    */

  public:
    RuntimeEnvironment() {
      global_ = map<string, AstValue*>();
      //Adding +,-,*,/,=,<,>,input,output
      global_["default_plus"] = new DefaultFunctionValue("plus");
      global_["default_minus"] = new DefaultFunctionValue("minus");
      global_["default_times"] = new DefaultFunctionValue("times");
      global_["default_divide"] = new DefaultFunctionValue("divide");
      global_["default_equals"] = new DefaultFunctionValue("equals");
      global_["default_less"] = new DefaultFunctionValue("less");
      global_["default_greater"] = new DefaultFunctionValue("greater");
      global_["default_input"] = new DefaultFunctionValue("input");
      global_["default_output"] = new DefaultFunctionValue("output");
    }

    /*
     * Bind the access passed as parameter with the value passed as parameter.
     */
    void Assign(Access* access, AstValue* node) {
      if(access->IsGlobal()) {
	global_[access->GetIdentifier()] = node;
      } else {
	LocalAccess* loc = static_cast<LocalAccess*>(access);
	stack_[bp_ + 1 + loc->GetOffset()] = node;
      }
    }

    /*
     * Create a new allocation frame. 
     */
    void CreateAllocationFrame(int size) {
      if(bp_ + size + 1 >= STACK_SIZE) {
	PrintError(NULL, "Stack Overflow");
      }
      stack_[sp_ + 1] = new BasePointerValue(bp_);
      bp_ = sp_ + 1;
      sp_ = bp_ + 1 + size;
    }

    /*
     * Destroy an allocation frame.
     */
    void DestroyAllocationFrame() {
      sp_ = bp_ - 1;
      BasePointerValue* old_bp = static_cast<BasePointerValue*>(stack_[bp_]);
      sp_ = bp_ - 1;
      bp_ = old_bp->GetValue();
    }

    /*
     * Read and return the value associated with the access given as parameter.
     */
    AstValue* Read(Access* access) {
      if(access->IsGlobal()) {
	return global_[access->GetIdentifier()];
      } else {
	LocalAccess* loc = static_cast<LocalAccess*>(access);
	return stack_[bp_ + 1 + loc->GetOffset()];
      }
    }

  private:
    //Global variables mapping
    map<string, AstValue*> global_;
    //Function call stack
    AstValue** stack_ = new AstValue*[STACK_SIZE];
    //Stack pointer (always point to the top element of the stack)
    int sp_ = -1;
    //Base pointer (always point to the first element of the active execution frame)
    int bp_ = 0;
  };

  class InterpreterVisitor: public SimpleAstVisitor {
  public:
    InterpreterVisitor() {}

    virtual void* VisitLoadDirective(AstLoadDirective* import, void* arg) {
      return import->GetProgram()->Accept(this, arg);
    }

    virtual void* VisitDefinition(AstDefinition* definition, void* arg) {
      //Evaluate the definition right-hand side
      AstValue* result = static_cast<AstValue*>(definition->GetBody()->Accept(this, arg));
      Access* var = definition->GetVariable()->GetAccess();
      //Assign the result of the right-hand side to the variable
      env_->Assign(var, result);
      //Return UNIT
      return UnitValue::Create();
    }

    virtual void* VisitNumberExpression(AstNumberExpression* number_expression, void* arg) {
      return new AstNodeValue(number_expression, this);
    }

    virtual void* VisitVariableExpression(AstVariableExpression* variable_expression, void* arg) {
      return env_->Read(variable_expression->GetAccess());
    }

    virtual void* VisitLambdaAbstraction(AstLambdaAbstraction* lambda_expression, void* arg) {
      return new AstNodeValue(lambda_expression, this);
    }

    virtual void* VisitLambdaApplication(AstLambdaApplication* lambda_application, void* arg) {
      //Evaluate the function
      AstValue* function = static_cast<AstValue*>(lambda_application->GetFunction()->Accept(this, arg));

      vector<AstValue*> arguments;

      if(function->IsLambda()) {
	//Evaluates all the elements in the oreder as they appear in the AST, i.e. from left to right.
	for(AstExpression* exp : *lambda_application->GetArguments()) {
	  arguments.push_back(static_cast<AstValue*>(exp->Accept(this, arg)));
	}

	//Evaluate the function into an AstExpression, then evaluate the expression into a AstValue.
	return function->Eval(env_, arguments);
      } else {
	PrintError(lambda_application, "Attempt to call a non-functional value");
      }

      return NULL;
    }

    virtual void* VisitIfExpression(AstIfExpression* if_expression, void* arg) {
      //Evaluate the condition
      AstValue* condition = static_cast<AstValue*>(if_expression->GetCondition()->Accept(this, arg));
      if(condition->IsValidCondition()) {
	//If the condition is true evaluate the true branch
	if(condition->GetCondition()) {
	  return if_expression->GetTrueBranch()->Accept(this, arg);      	
	  //If the condition is fale evaluate the false branch
	} else {
	  return if_expression->GetFalseBranch()->Accept(this, arg);
	}
      } else {
	PrintError(if_expression, "The value is not a valid boolean");
      }

      return NULL;
    }

    virtual void* VisitSetExpression(AstSetExpression* set_expression, void* arg) {
      AstValue* value = static_cast<AstValue*>(set_expression->GetValue()->Accept(this, arg));
      Access* var = set_expression->GetVariable()->GetAccess();
      env_->Assign(var, value);
      return UnitValue::Create();
    }

  private:
    RuntimeEnvironment *env_ = new RuntimeEnvironment();
  };

  void Interpreter::Run() {
    InterpreterVisitor* visitor = new InterpreterVisitor();
    unit->Accept(visitor, NULL);
  }

};
