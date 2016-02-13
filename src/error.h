/* 
 * Defines the application error code and print error functions.
 * Do not forget to include "ast.h" and <iostream> when using this header.
 */

using namespace std;

namespace cscheme {

#define EXIT_OK 0
#define EXIT_FILE_NOT_FOUND 1
#define EXIT_BUG 2
#define EXIT_LEXER_ERROR 3
#define EXIT_PARSE_ERROR 4
#define EXIT_FILE_RESOLUTION_FAILED 5
#define EXIT_WRONG_INPUT 6

  /** 
   * Print an error on the standard error, indicating a bug.
   */
  void Bug(string message);
};
