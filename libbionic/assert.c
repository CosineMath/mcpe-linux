
#include <stdio.h>
#include <stdlib.h>

void __assert2(const char* file, int line, const char* function, const char* failed_expression) {
  fprintf(stderr, "%s:%d: %s: assertion \"%s\" failed", file, line, function, failed_expression);
  exit(-1);
}
