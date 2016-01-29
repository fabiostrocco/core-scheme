#include <string>
#include <iostream>
#include "error.h"

namespace cscheme {

  void Bug(string message) {
    cerr << "BUG: " << message << endl;
    exit(EXIT_BUG);
  }

};
