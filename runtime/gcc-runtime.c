#include <stdio.h>
#include <stdlib.h>

/* LLVM GCC Interface */

int gcc_print(int x) {
  return printf("%d\n", x);
}

int gcc_input() {
  int x;
  if(!scanf("%d", &x)) {
    fprintf(stderr, "The input is not an integer\n");
    exit(1);
  }
  return x;
}

char* to_type_string(int type);

void gcc_rti_error(int value_type, int expected_type) {
  char* value_string = to_type_string(value_type);
  char* expected_string = to_type_string(expected_type);
  fprintf(stderr, "Runtime erorr: expected %s, %s was found instead\n",
          value_string, expected_string);
  exit(4);
}

/* Private functions */

#define MAX_STRING 512

char* to_type_string(int type) {
  char* str = (char*) malloc(sizeof(char) * MAX_STRING);
  if(type == -2) {
    sprintf(str, "%s", "number");
  } else if(type == -1) {
    sprintf(str, "%s", "unit");
  } else {
    sprintf(str, "function with %d arguments", type);
  }

  return str;
}
