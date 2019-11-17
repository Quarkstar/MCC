#include "mcc.h"

int line;
static Map* keyword_map;
static FILE *fp;

void lex_init(FILE *input_file) {
  fp = input_file;
  Map* map = new_map();
  map_puti(map, "const", CONSTTK);
  map_puti(map, "int", INTTK);
  map_puti(map, "char", CHARTK);
  map_puti(map, "void", VOIDTK);
  map_puti(map, "main", MAINTK);
  map_puti(map, "if", IFTK);
  map_puti(map, "else", ELSETK);
  map_puti(map, "do", DOTK);
  map_puti(map, "while", WHILETK);
  map_puti(map, "for", FORTK);
  map_puti(map, "scanf", SCANFTK);
  map_puti(map, "printf", PRINTFTK);
  map_puti(map, "return", RETURNTK);
  map_puti(map, "+", PLUS);
  map_puti(map, "-", MINU);
  map_puti(map, "*", MULT);
  map_puti(map, "/", DIV);
  map_puti(map, "<", LSS);
  map_puti(map, "<=", LEQ);
  map_puti(map, ">", GRE);
  map_puti(map, ">=", GEQ);
  map_puti(map, "==", EQL);
  map_puti(map, "!=", NEQ);
  map_puti(map, "=", ASSIGN);
  map_puti(map, ";", SEMICN);
  map_puti(map, ",", COMMA);
  map_puti(map, "(", LPARENT);
  map_puti(map, ")", RPARENT);
  map_puti(map, "[", LBRACK);
  map_puti(map, "]", RBRACK);
  map_puti(map, "{", LBRACE);
  map_puti(map, "}", RBRACE);
  keyword_map = map;
}

Token *next() {
  Token *token;
  int nowchar, nextchar;
  nowchar = getc(fp);

  while (isspace(nowchar) || nowchar == '\n') {
    if (nowchar == '\n') {
      line++;
    }
    nowchar = getc(fp);
  }

  if (nowchar == EOF) {
    return NULL;
  }

  token = (Token*)malloc(sizeof(Token));
  StringBuilder* s = new_sb();

  // keywords
  if (isalpha(nowchar) || nowchar == '_') {
    sb_add(s, (char)nowchar);
    nextchar = getc(fp);
    while (isalnum(nextchar) || nextchar == '_') {
      sb_add(s, (char)nextchar);
      nextchar = getc(fp);
    }
    ungetc(nextchar, fp);
    token->kind = map_geti(keyword_map, sb_get(s), IDENFR);
  }

  // digit
  else if (isdigit(nowchar)) {
    sb_add(s, (char)nowchar);
    nextchar = getc(fp);
    while (isdigit(nextchar)) {
      sb_add(s, (char)nextchar);
      nextchar = getc(fp);
    }
    ungetc(nextchar, fp);
    token->kind = INTCON;
  }

  // one character
  else if (strchr("+-*/;,(){}[]", nowchar)) {
    sb_add(s, (char)nowchar);
    token->kind = map_geti(keyword_map, sb_get(s), -1);
  }

  // multi character
  else if (strchr("><=!", nowchar)) {
    sb_add(s, (char)nowchar);
    nextchar = getc(fp);
    if (nextchar == '=') {
      sb_add(s, (char)nextchar);
    } else {
      ungetc(nextchar, fp);
    }
    token->kind = map_geti(keyword_map, sb_get(s), -1);
  }

  // string
  else if (nowchar == '\"') {
    nextchar = getc(fp);
    while (nextchar != '\"') {
      sb_add(s, (char)nextchar);
      nextchar = getc(fp);
    }
    token->kind = STRCON;
  }

  // one character
  else if (nowchar == '\'') {
    sb_add(s, (char)getc(fp));
    nextchar = getc(fp);
    if (nextchar != '\'') {
      error("not a char");
    }
    token->kind = CHARCON;
  }

  // error
  else {
    error("a");
  }

  token->data = sb_get(s);
  //printf("%c  next: %d %s\n", nowchar, token->kind, token->data);
  return token;
}
