#include "mcc.h"

Token* token;

int main(void) {
  FILE *input, *output;
  token = NULL;
  line = 1;
  input = fopen("testfile.txt", "r");
  //exit(0);
  //output = fopen("error.txt", "w");
  gen_mips_init();
  lex_init(input);
  //test();
  program();
  gen_mips_end();
  fclose(input);
  //fclose(output);
  return 0;
}
