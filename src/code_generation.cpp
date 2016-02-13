/**
 * CONSTRAINTS:
 *   1. Only global variables can be assigned
 *   2. Arithmetic operations are not variadic. For example (+ 1 2 3) is not allowed,
 *      and it has to be desugared into (+ 1 (+ 2 3))
 *
 * RUNTIME STRUCTURE
 *
 *   - numbers have type i32
 *   - functions have type compatible with i32
 *   - parameters have type i32
 *   - global variables have type i32*
 *   - set! returns 0
 * 
 */

#include "parser.h"
#include "code_generation.h"
#include "analysis.h"
#include "error.h"

namespace cscheme {

  static llvm::Type* create_data_type() {
    return Type::getInt64Ty(getGlobalContext());
  }

  /*TheModule contains all the main functions to generate LLVM IR code*/
  static Module *module;
  /*Builder contains some utility*/
  static IRBuilder<> builder(getGlobalContext());
  /*Mapping (from LLVM) scheme_variable_name |-> LLVM variable element*/
  static std::map<std::string, Value*> named_values;
  /*Data type*/
  static llvm::Type* data_type = create_data_type();

  //Main function's basick block for global variables initialization
  //static BasicBlock *defBlock = NULL;

  class CodeGeneratorVisitor: public SimpleAstVisitor {
  public:
    CodeGeneratorVisitor() {}

    /*
     * For each load directive, it generates new instructions, corresponding to the
     * compilation unit.
     */
    virtual void* VisitLoadDirective(AstLoadDirective* import, void* arg) {
      return import->GetProgram()->Accept(this, arg);
    }

    /*Each visit method retuns an instance of Value*
     *
     * A value is either a constant, variable or a function reference.
     * Each emit phase returns a value that contains the entire expression evaluation.
     *
     */
    virtual void* VisitDefinition(AstDefinition* definition, void* arg) {
      Constant *zero = Constant::getNullValue(IntegerType::getInt32Ty(getGlobalContext()));
      GlobalVariable* global_variable = new GlobalVariable(*module,
							   Type::getInt32Ty(getGlobalContext()),
							   /*isConstant=*/false,
							   GlobalValue::CommonLinkage,
							   zero,
							   definition->GetVariable()->GetAccess()->ToString());


      named_values[definition->GetVariable()->GetAccess()->GetIdentifier()] = global_variable;
      global_variable->setName(definition->GetVariable()->GetAccess()->GetIdentifier());
      // Constant Definitions
      //builder.SetInsertPoint(defBlock);
      Value* value = static_cast<Value*>(definition->GetBody()->Accept(this, NULL));
      //builder.SetInsertPoint(defBlock);
      builder.CreateStore(value, global_variable);
      return NULL;
    }

    /**
     * A number is simply converted into a LLVM float number
     */
    virtual void* VisitNumberExpression(AstNumberExpression* number_expression, void* arg) {
      return ConstantInt::get(getGlobalContext(),
			      APInt(32, atoi(number_expression->GetNumber()->GetText().c_str())));
    }

    /**
     * A variable reference is converted into the corresponding LLVM variable. The access phase resolve the
     * concrete name (ex. for function is is a variable containing a function pointer, for local variable it is a register)
     */
    virtual void* VisitVariableExpression(AstVariableExpression* variable_expression, void* arg) {
      Access *access = variable_expression->GetAccess();
      string llvm_var_name = access->GetIdentifier();
      Value *llvm_var = named_values[llvm_var_name];

      if (llvm_var != NULL) {
	//Global access is mutable (set! can be applied to global variables in our language)
	//therefore a dereference instruction must be generated.
	if(access->IsGlobal()) {
	  llvm_var = builder.CreateLoad(llvm_var, llvm_var_name.c_str());
	}
	return llvm_var;
      } else {
	PrintError(variable_expression, 
		   "Unknown variable name "
		   + variable_expression->GetID()->GetText());
	return NULL;
      }
    }

    virtual void* VisitLambdaAbstraction(AstLambdaAbstraction* lambda_expression, void* arg) {
      BasicBlock *current_block = builder.GetInsertBlock();
      /*----------------------- STEP 1: Create function signature -------------------------------*/

      // Make the function type: (int x ... x int) -> int where the number of input type is the same
      // as the arity
      vector<Type*> Doubles(lambda_expression->GetParameters()->size(), Type::getInt32Ty(getGlobalContext()));
      FunctionType* f_type = FunctionType::get(Type::getInt32Ty(getGlobalContext()),
					       Doubles, false);
      //Create the function
      Function* fun = Function::Create(f_type, Function::ExternalLinkage, "inline_function", module);

      // If F already has a body, reject this
      //That should never happen, it would be a bug
      if (!fun->empty()) {
	Bug("redefinition of function");
	return 0;
      }

      // Set names for all arguments.
      unsigned idx = 0;
      std::list<AstVariableExpression*>::iterator it = lambda_expression->GetParameters()->begin();
      for (Function::arg_iterator ai = fun->arg_begin(); idx != lambda_expression->GetParameters()->size();
	   ++ai, ++idx) {
	ai->setName((*it)->GetAccess()->ToString());
	// Add arguments to variable symbol table.
	named_values[(*it)->GetAccess()->ToString()] = ai;
	it++;
      }

      /*----------------------- STEP 2: emit return expression and create return instruction -------------------------------*/
      //Create a basic block named entry in the function "F"
      BasicBlock* ret_block = BasicBlock::Create(getGlobalContext(), "entry", fun);
      //Set the current block where to generate instructions as the entry block
      builder.SetInsertPoint(ret_block);
      Value* ret_val = static_cast<Value*>(lambda_expression->GetBody()->Accept(this, NULL));
      //Create return "retVal" instruction
      builder.CreateRet(ret_val);
      //Just a sanity check to catch bug in the compiler (if the code generation is wrong)
      verifyFunction(*fun);
      //Restore the insert point to the previous basic block
      builder.SetInsertPoint(current_block);
      PtrToIntInst *res = new PtrToIntInst::PtrToIntInst(fun, Type::getInt32Ty(getGlobalContext()), "functionPointer", current_block);
      return res;
    }

    virtual void* VisitLambdaApplication(AstLambdaApplication* lambda_application, void* arg) {
      /*----------------------- STEP 1: Evaluate function call site and arguments (call-by-balue, left-to-right) -------------------------------*/

      //Evaluate call site
      AstExpression *function_expression = lambda_application->GetFunction();
      Value *llvm_function = static_cast<Value*>(function_expression->Accept(this, NULL));
      vector<Value*> llvm_arguments;

      //Evaluate function arguments
      for(AstExpression *exp : * (lambda_application->GetArguments())) {
	llvm_arguments.push_back(static_cast<Value*>(exp->Accept(this, NULL)));
      }

      /*----------------------- STEP 2: Build the function type -------------------------------*/
      vector<Type*> Doubles(lambda_application->GetArguments()->size(), Type::getInt32Ty(getGlobalContext()));
      FunctionType* f_type = FunctionType::get(Type::getInt32Ty(getGlobalContext()),
					       Doubles, false);
      //For a function call (e e_1 ... e_n) the function type f_type = (i32, ..., i32 n times) -> i32
      builder.GetInsertBlock();
      //If f_type = (i32, ..., i32 n times) -> i32 pType = (i32, ..., i32 n times) -> i32 *
      PointerType* p_type = PointerType::get(f_type, 0);
      //Create cast llvmFunction from i32 to PT
      Value *call_site = builder.CreateIntToPtr(llvm_function, p_type, "callee");

      /*----------------------- STEP 3: Generate function call-------------------------------*/
      Value *res = builder.CreateCall(call_site, llvm_arguments, "callResult");
      return res;
    }

    /**
     * The conversion proceed as follows
     * Emit [exp1] instructions, store result in cond
     * Emit cmp cond 0 instruction
     * Create 3 basic blocks: then_branch, else_branch, merge_branch. These blocks are initially empty
     *
     * Tell LLVM to insert the furter emitted instructions in then_branch
     *
     * Emit [exp2] that will be therefore emitted under then_branch
     * Emit jmp merge_branch, that is still empty because it will be emitted later, but we need the label to emit this
     * instruction
     *
     * Tell LLVM to inster the furter emitted instructions in else_branch
     * Emit [exp3] that will be therefore emitted under else_branch
     * Emit jmp merge_branch
     */
    virtual void* VisitIfExpression(AstIfExpression* if_expression, void* arg) {
      //Evaluate the condition
      Value* cond = static_cast<Value*>(if_expression->GetCondition()->Accept(this, arg));

      /*----------------------- STEP 1: Create condition branch and generate true-false branch instructions-------------*/
      //Convert condition to a bool by comparing equal to 0
      //compareRes contains the comparison result
      Value *compare_res = static_cast<Value*>(builder.CreateICmpEQ(cond,
								    ConstantInt::get(getGlobalContext(), APInt(32, 0)), "ifcond"));

      //Get the current enclosing function, where to add the if-then-else basic blocks
      Function *enclosing_function = builder.GetInsertBlock()->getParent();

      // Create blocks for the then and else cases.  Insert the 'then' block at the
      // end of the function.
      BasicBlock *then_branch = BasicBlock::Create(getGlobalContext(), 
						   "then",
						   enclosing_function);
      BasicBlock *else_branch = BasicBlock::Create(getGlobalContext(), "else");
      BasicBlock *merge_branch = BasicBlock::Create(getGlobalContext(), "ifcont");

      //Create the if-then-else instruction set. Only thenBranch is currently added, but
      //the entire instruction set is scheduled fore emission (the builder does the actual instruction
      //emission)
      builder.CreateCondBr(compare_res, else_branch, then_branch);

      /*----------------------- STEP 2: Create and populate then branch basic block -------------*/

      /*Emit then basic block*/
      // Emit then value.
      // Each further generated instruction will be inserted into the thenBranch basic block
      builder.SetInsertPoint(then_branch);

      //Populates the then branch
      Value *then_value = static_cast<Value*>(if_expression->GetTrueBranch()->Accept(this, arg));
      //builder.SetInsertPoint(thenBranch);
      //Creates jump to merge branch (then end)
      builder.CreateBr(merge_branch);
      // Codegen of 'Then' can change the current block, update thenBranch for the PHI.
      then_branch = builder.GetInsertBlock();

      /*----------------------- STEP 3: Create and poplate else branch basic block -------------*/
      /*Emit else basic block*/
      enclosing_function->getBasicBlockList().push_back(else_branch);
      builder.SetInsertPoint(else_branch);
      Value *else_value = static_cast<Value*>(if_expression->GetFalseBranch()->Accept(this, arg));
      //builder.SetInsertPoint(else_branch);
      builder.CreateBr(merge_branch);
      // Codegen of 'Else' can change the current block, update elseBranch for the PHI.
      else_branch = builder.GetInsertBlock();

      /*Emit the merge basic block*/
      // Emit merge block.
      enclosing_function->getBasicBlockList().push_back(merge_branch);
      builder.SetInsertPoint(merge_branch);
      PHINode* phi = builder.CreatePHI(Type::getInt32Ty(getGlobalContext()), 2, "iftmp");

      phi->addIncoming(then_value, then_branch);
      phi->addIncoming(else_value, else_branch);

      //Return the Phi node, that is either valued to the then or the else branch
      return phi;
    }

    virtual void* VisitSetExpression(AstSetExpression* set_expression, void* arg) {
      Access *access = set_expression->GetVariable()->GetAccess();
      if(!access->IsGlobal()) {
	PrintError(set_expression, "Cannot use set! with a local variable during compilation");
      }

      string id = access->GetIdentifier();
      Value *var = named_values[id];
      Value *value = static_cast<Value*>(set_expression->GetValue()->Accept(this, NULL));
      builder.CreateStore(value, var);
      return Constant::getNullValue(IntegerType::getInt32Ty(getGlobalContext()));;
    }

  private:
    /// CreateEntryBlockAlloca - Create an alloca instruction in the entry block of
    /// the function.  This is used for mutable variables etc.
    static AllocaInst* CreateEntryBlockAlloca(Function *function,
					      const std::string &var_name) {
      IRBuilder<> TmpB(&function->getEntryBlock(),
		       function->getEntryBlock().begin());
      return TmpB.CreateAlloca(Type::getDoubleTy(getGlobalContext()), 0,
			       var_name.c_str());
    }
  };


  /* CLASS CodeGenerator */

  void CodeGenerator::Init() {
    module = new Module("CoreScheme Backend", getGlobalContext());      
  }

  void CodeGenerator::Compile() {
    create_data_type();
    Function* main = emit_main();
    BasicBlock* entry_block = BasicBlock::Create(getGlobalContext(), "main_entry", main);

    /*Emit instructions*/
    builder.SetInsertPoint(entry_block);
    emit_mappings();
    emit_rti_prototypes();
    emit_initialization_call();
    Value* result = static_cast<Value*>(unit->Accept(new CodeGeneratorVisitor(), NULL));
      
    //Returns the expression result
    builder.CreateRet(result);
  }
  
  void CodeGenerator::Print() {
    module->dump();
  }

  Function* CodeGenerator::emit_main() {
    std::vector<llvm::Type*> main_arg_types;

    FunctionType* main_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(getGlobalContext()), 
						      main_arg_types,
						      false);

    llvm::Function* func = llvm::Function::Create(main_type, 
						  llvm::Function::ExternalLinkage, 
						  Twine("main"), module);
    func->setCallingConv(CallingConv::C);
    return func;
  }
  
  void CodeGenerator::emit_mappings() {
    vector<string> bindings = Environment::GetExternalBindings();
    Constant* zero = Constant::getNullValue(IntegerType::getInt32Ty(getGlobalContext()));
    for(string b : bindings) {
      GlobalVariable* global_variable = new GlobalVariable(*module, 
    							   Type::getInt32Ty(getGlobalContext()),
    							   false,
    							   GlobalValue::ExternalLinkage,
    							   NULL,
    							   b, NULL);
      named_values[b] = global_variable;
    }
  }

  void CodeGenerator::emit_rti_prototypes() {
    /* Emit rti(data_type value, int type) function */
    vector<llvm::Type*> rti_arg_types;
    rti_arg_types.push_back(data_type);
    rti_arg_types.push_back(Type::getInt32Ty(getGlobalContext()));

    FunctionType* rti_type = FunctionType::get(Type::getVoidTy(getGlobalContext()),
					       rti_arg_types, true);
    Function* func = Function::Create(rti_type, Function::ExternalLinkage, Twine("rti"), module);
    func->setCallingConv(llvm::CallingConv::C);

    /* Emit convert(data_type value) function */
    vector<llvm::Type*> convert_arg_types;
    convert_arg_types.push_back(data_type);

    FunctionType* convert_type = FunctionType::get(Type::getInt32Ty(getGlobalContext()),
					       convert_arg_types, true);
    Function* func2 = Function::Create(convert_type, Function::ExternalLinkage, Twine("convert"), module);
    func2->setCallingConv(llvm::CallingConv::C);

    /* Emit create(int value, int type) function */
    vector<llvm::Type*> create_arg_types;
    create_arg_types.push_back(Type::getInt32Ty(getGlobalContext()));
    create_arg_types.push_back(Type::getInt32Ty(getGlobalContext()));

    FunctionType* create_type = FunctionType::get(data_type,
					       create_arg_types, true);
    Function* func3 = Function::Create(create_type, Function::ExternalLinkage, Twine("create"), module);
    func3->setCallingConv(llvm::CallingConv::C);
  }

  void CodeGenerator::emit_initialization_call() {
    vector<Type*> init_types;

    FunctionType* init_type = FunctionType::get(Type::getInt32Ty(getGlobalContext()), init_types, true);
    Function* func = Function::Create(init_type,
				      Function::ExternalLinkage, Twine("init"), module);
    func->setCallingConv(llvm::CallingConv::C);
    vector<Value*> args;
    builder.CreateCall(func, args, "emptyResult");
  }

};

