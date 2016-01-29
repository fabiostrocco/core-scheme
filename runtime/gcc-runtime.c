#include <stdio.h>
#include <stdlib.h>

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
