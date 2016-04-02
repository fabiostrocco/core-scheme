/* This file contains the code for code generation */

/*
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
 * */

#include "llvm/Analysis/Verifier.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Analysis/Passes.h>
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"

using namespace llvm;

namespace cscheme {

  class CodeGenerator {
  public:
  CodeGenerator(AstCompilationUnit* unit) : unit(unit) {
      Init();
    }

    void Init();
    void Compile();
    void Print();

  private:
    AstCompilationUnit* unit;
    llvm::Function* emit_main();
    void emit_rti_prototypes();
    void emit_mappings();
    void emit_initialization_call();
  };

};
