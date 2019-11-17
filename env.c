#include "mcc.h"

Env *env;
Vector *funcs;

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

// generate this env's declaration
void gen_decl() {
    Vector *v = env->vars->vals;
    Var* var;
    for(int i = 0; i < v->len; i++) {
        var = v->data[i];
        printf("kind:%d type:%d name:%s val:%d\n", var->ty->kind, var->ty->ty, var->name, var->val);
    }
}

void gen_ir2(char *op, char *t1, char *t2, char *ans) {
    printf("%s %s %s %s\n",op,t1,t2,ans);
}