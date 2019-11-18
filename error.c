#include "mcc.h"

void error(char* s) {
  printf("%s %d %s\n", s,token->kind, token->data);
  exit(-1);
  return;
}
