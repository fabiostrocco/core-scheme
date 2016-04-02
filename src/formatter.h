/** 
 * This file defines a pretty printer, converting an AST into a formatted
 * cscheme code, including analysis informations (such as the offset of
 * local variables in the stack).
 */

#define TAB "\t"
#define NOTAB new string("")

using namespace ast;

namespace cscheme {

  class Formatter {
  public:
  Formatter(AstCompilationUnit* unit) : unit_(unit) {}

    void Print();

  private:
    AstCompilationUnit* unit_;

  };

};
