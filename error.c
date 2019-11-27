#include "mcc.h"

void error(char* s) {
  printf("%s %d %s\n", s,token->kind, token->data);
  exit(0);
  return;
}

void errort(char* s) {
  printf("%s %d %s\n", s,token->kind, token->data);
  exit(-1);
  return;
}