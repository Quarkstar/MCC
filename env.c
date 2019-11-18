#include "mcc.h"

Env *env;
Vector *funcs;
Function *func_now;

Env *new_env(Env *prev) {
  Env *env = calloc(1, sizeof(Env));
  env->vars = new_map();
  env->prev = prev;
  return env;
}

Function *new_func() {
  Function *f = calloc(1, sizeof(Function));
  f->args = new_vec();
  return f;
}

Var *find_var(char *name) {
  for (Env *e = env; e; e = e->prev) {
    Var *var = map_get(e->vars, name);
    if (var)
      return var;
  }
  return NULL;
}

Var *find_var_this_env(char *name) {
  Var *var = map_get(env->vars, name);
    if (var) return var;
  return NULL;
}

Var *add_var(Type *ty, char *name) {
  if (find_var_this_env(name) != NULL) {
    error("var has been decl");
  }
  Var *var = calloc(1, sizeof(Var));
  var->ty = ty;
  var->name = name;
  map_put(env->vars, name, var);
  return var;
}

Type* new_type(int ty, int kind) {
  Type *tp = malloc(sizeof(Type));
  tp->ty = ty;
  tp->kind = kind;
  return tp;
}


