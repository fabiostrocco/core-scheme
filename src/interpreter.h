/**
 * The following file implements the cscheme interpreter.
 * Please note that this implementation differs from the compiler in 
 * the following points

 * 1. Global variables can be assigned with set!, the compiler does not support this feature
 * 2. functions are not compatible with i32, preventing functions as boolean values and numeric operations involving
 function pointers. The compiler translates functions with a number, that is the function pointer.
 * 3. It is not possible to use non numeric boolean conditions, i.e. conditions produced by pure numbers or numeric operations, 
 and it is not possible to call functions with wrong arity
 * 4. set! returns unit, it returns 0 when compiled with the compiler 
 */

#define STACK_SIZE 65535

namespace cscheme {


  class Interpreter {
  private:
    AstCompilationUnit* unit;

  public:
  Interpreter(AstCompilationUnit* unit) : unit(unit) {}

    void Run();
  };

};
