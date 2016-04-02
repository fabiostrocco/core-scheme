/**
 * This file contains code to parse and represent the command line option 
 */

#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

/** 
 * Program running mode.
 */
enum Mode { INFO, PRINT, COMPILE, INTERPRETER };

/**
 * Informations about the command line arguments.
 */
class CommandLineOptions {
public:
  CommandLineOptions(int argc, char** argv) {
    if(argc == 1) {
      Init(NULL, INFO, false);
    } else if(argc == 2) {
      Init(argv[1], COMPILE, false);
    } else {
      char* option = argv[1];
      bool debug = argc == 4 && strcmp(argv[2], "--debug") == 0;
      char* name = debug ? argv[3] : argv[2];
      if(strcmp(option, "--mode=analysis") == 0) {
	Init(name, PRINT, debug);
      } else if(strcmp(option, "--mode=llvm") == 0) {
	Init(name, COMPILE, debug);
      } else if(strcmp(option, "--mode=interpreter") == 0) {
        Init(name, INTERPRETER, debug);
      } else {
	Init(NULL, INFO, debug);
      }
    }
  }
  
char* GetFileName();
FILE* GetFile();
bool FileExists();
bool IsPrintFormatted();
bool IsCompileToLLVM();
bool IsPrintInfo();
bool IsInterpreter();
void PrintInfo();

private:
  char* f_name_;
  Mode mode_;
  FILE* file_;
  void Init(char *f_name, Mode mode, bool is_debug);
};
