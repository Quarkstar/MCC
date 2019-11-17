#ifndef GLOBAL_H
#define GLOBAL_H

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_VAL_MAX_LEN 200
#define SYM_TABLE_VAL 300

// util.c

typedef struct Node {
  void* data;
  struct Node* link;
} Node;

typedef struct {
  Node* first;
  Node* last;
  int len;
} List;

List* new_list(void);
void list_offer(List*, void*);
void* list_poll(List*);
List* list_append(List*, List*);
bool list_empty(List*);

typedef struct {
  void** data;
  int capacity;
  int len;
} Vector;

Vector* new_vec(void);
void vec_push(Vector* v, void* elem);
void vec_pushi(Vector* v, int val);
void* vec_pop(Vector* v);
void* vec_last(Vector* v);
bool vec_contains(Vector* v, void* elem);
bool vec_union1(Vector* v, void* elem);

typedef struct {
  Vector* keys;
  Vector* vals;
} Map;

Map* new_map(void);
void map_put(Map* map, char* key, void* val);
void map_puti(Map* map, char* key, int val);
void* map_get(Map* map, char* key);
int map_geti(Map* map, char* key, int default_);
bool map_exists(Map* map, char* key);

typedef struct {
  char* data;
  int capacity;
  int len;
  bool finish;
} StringBuilder;

StringBuilder* new_sb(void);
void sb_add(StringBuilder* sb, char c);
void sb_append(StringBuilder* sb, char* s);
void sb_append_n(StringBuilder* sb, char* s, int len);
char* sb_get(StringBuilder* sb);

enum {
  IDENFR,
  INTCON,
  CHARCON,
  STRCON,
  CONSTTK,
  INTTK,
  CHARTK,
  VOIDTK,
  MAINTK,
  IFTK,
  ELSETK,
  DOTK,
  WHILETK,
  FORTK,
  SCANFTK,
  PRINTFTK,
  RETURNTK,
  PLUS,
  MINU,
  MULT,
  DIV,
  LSS,
  LEQ,
  GRE,
  GEQ,
  EQL,
  NEQ,
  ASSIGN,
  SEMICN,
  COMMA,
  LPARENT,
  RPARENT,
  LBRACK,
  RBRACK,
  LBRACE,
  RBRACE
};


enum { VAR = 1, ARY, FUNC, CONST, EXP };

typedef struct Type {
  int ty;
  int kind;
  int len;
} Type;

// error.c
void error(char* s);
// test.c
void print_token(void);
void test();
// lex.c
typedef struct Token {
  int kind;
  char* data;
} Token;
void lex_init(FILE *);
Token *next(void);
// syn.c

void program(void);

extern Token* token;
extern int line;
extern const int keyword_num;

// env.c symbol tabel
typedef struct Env{
  Map *vars;
  struct Env *prev;
} Env;

typedef struct {
  char* name;
  Vector* args;
} Function;

typedef struct {
  Type* ty;
  char* name;
  int val;
} Var;

extern Env *env;
extern Vector *funcs;


Env *new_env(Env *);
Function* new_func(void);
Var *find_var(char *);
Var *find_var_this_env(char *);
Var *add_var(Type *, char *);
Type* new_type(int, int);
void gen_decl(void);
void gen_ir2(char*, char*, char*, char*);
#endif
