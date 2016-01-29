/* This file contains code to parse and represent the command line option */

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
      Init(NULL, INFO);
    } else if(argc == 2) {
      Init(argv[1], COMPILE);
    } else {
      char* option = argv[1];
      char* name = argv[2];
      if(strcmp(option, "--mode=analysis") == 0) {
	Init(name, PRINT);
      } else if(strcmp(option, "--mode=llvm") == 0) {
	Init(name, COMPILE);
      } else if(strcmp(option, "--mode=interpreter") == 0) {
        Init(name, INTERPRETER);
      } else {
	Init(NULL, INFO);
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
void CloseFile();
void PrintInfo();

private:
  char* f_name_;
  Mode mode_;
  FILE* file_;
  void Init(char *f_name, Mode mode);
};
