#include <string>
#include <iostream>
#include "error.h"

namespace cscheme {

  bool debug_mode = 0;

  void Bug(string message) {
    cerr << "BUG: " << message << endl;
    exit(EXIT_BUG);
  }

  void CSMAssert(bool predicate, string error_message, string file_name, int line) { 
    if(predicate == false) { 
      cerr << "ASSERTION FAILED " << file_name << "::" << line << ": " << error_message << endl; 
      exit(EXIT_BUG);
    } 
  }

  void SetDebug(bool debug) {
    debug_mode = debug;
  }
  
  bool GetDebug() {
    return debug_mode;
  }
};
