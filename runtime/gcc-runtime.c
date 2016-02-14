#include <stdio.h>
#include <stdlib.h>

/* LLVM GCC Interface */

void gcc_print(int value, int type) {
  if(type == -2) {
    printf("%d\n", value);
  } else if(type == -1) {
    printf("#<void>\n");
  } else {
    printf("#<procedure>\n");
  }
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
  fprintf(stderr, "Runtime error: expected %s, %s was found instead\n",
          expected_string, value_string);
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
