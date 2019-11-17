#include "mcc.h"

static char identity[][20] = {
    "IDENFR",  "INTCON",  "CHARCON", "STRCON",   "CONSTTK",  "INTTK",
    "CHARTK",  "VOIDTK",  "MAINTK",  "IFTK",     "ELSETK",   "DOTK",
    "WHILETK", "FORTK",   "SCANFTK", "PRINTFTK", "RETURNTK", "PLUS",
    "MINU",    "MULT",    "DIV",     "LSS",      "LEQ",      "GRE",
    "GEQ",     "EQL",     "NEQ",     "ASSIGN",   "SEMICN",   "COMMA",
    "LPARENT", "RPARENT", "LBRACK",  "RBRACK",   "LBRACE",   "RBRACE"};

static char *int_to_string(int val) {
  int len = 0, tmp = val;
  if (tmp <= 0) {
    tmp = -tmp;
    len++;
  }
  while(tmp > 0) {
    len++;
    tmp /= 10;
  }
  printf("%d %d\n",len,val);
  char *ans = malloc((len+1)*sizeof(char));
  sprintf(ans,"%d",val);
  return ans;
}

static char *new_label(void) {
  static int count = 0;
  StringBuilder *sb = new_sb();
  sb_append_n(sb, "$L", 2);
  sb_append(sb, int_to_string(++count));
  char *ans = sb_get(sb);
  free(sb);
  return ans;
}

static char *new_tmp(void) {
  static int count = 0;
  StringBuilder *sb = new_sb();
  sb_append_n(sb, "t", 1);
  sb_append(sb, int_to_string(++count));
  char *ans = sb_get(sb);
  free(sb);
  return ans;
}
void print_token(void) {
  //fprintf(result, "%s %s\n", identity[token->kind], token->data);
}
void test() {
  printf("test: inttostring %s\n",int_to_string(-1));
  printf("test: inttostring %s\n",int_to_string(-256));
  printf("test: inttostring %s\n",int_to_string(0));
  printf("test: inttostring %s\n",int_to_string(2));
  printf("test: inttostring %s\n",int_to_string(25));
  printf("test: inttostring %s\n",int_to_string(1123));
  for (int i = 0; i < 20; i++)
    printf("test: new_label %s\n", new_label());
  for (int i = 0; i < 20; i++)
    printf("test: new_tmp %s\n", new_tmp());
}