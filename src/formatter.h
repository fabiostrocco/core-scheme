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
