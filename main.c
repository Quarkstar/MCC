#include "mcc.h"

Token* token;

int main(void) {
  FILE *input, *output;
  token = NULL;
  line = 1;
  input = fopen("testfile.txt", "r");
  output = fopen("error.txt", "w");
  
  lex_init(input);
  //test();
  program();
  fclose(input);
  fclose(output);
  return 0;
}
