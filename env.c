#include "mcc.h"

Env *env;
Vector *funcs;

static FILE *ans;

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
    ans = fopen("mips.txt","w");
    bool global = env->prev == NULL ? true : false;
    Vector *v = env->vars->vals;
    Var* var;
    if (global) {
        fprintf(ans, ".data\n");
    }
    for(int i = 0; i < v->len; i++) {    
        var = v->data[i];
        var->global = global;
        printf("kind:%d type:%d name:%s val:%d\n", var->ty->kind, var->ty->ty, var->name, var->val);
        if (global) {
            if(var->ty->kind == CONST) {
                continue;
            } else if(var->ty->kind == VAR) {
                fprintf(ans, "%s: .space 4\n", var->name);
            } else if(var->ty->kind == ARY) {
                fprintf(ans,"%s: .space %d\n", var->name, (var->ty->len*4));
            }
        } else {
            var->val = (i+1) * 4;
        }
    }
    if (global) {
        fprintf(ans, ".text\n");
    }
}

static struct {
  char *name;
  int tmp;
} mips_reg[] = {
  {"$zero", 0}, {"$at",0}, {"v0",0},{"v1",0},{"a0",0},
  {"a1",0}, {"a2",0},{"a3",0},{"t0",0}, {"t1",0}, {"t2",0}, {"t3",0}, {"t4",0}, {"t5",0}, {"t6",0}, {"t7",0}, {"s0",0}, {"s1",0}, {"s2",0}, {"s3",0}, {"s4",0}, {"s5",0}, {"s6",0}, {"s7",0},
  {"t8",0}, {"t9", 0}, {"gp",0},{"sp",0},{"fp",0},{"ra",0}
};

static int get_tmp_num(char *t) {
  int ans = 0;
  for(int i = 2; t[i] != '\0'; i++) {
    ans = ans * 10 + (t[i] - '0');
  }
  return ans;
}

static void tmp_alloc(char *t) {
  for(int i = 8; i<=15;i++) {
    if(mips_reg[i].tmp == 0) {
      mips_reg[i].tmp = get_tmp_num(t);
      break;
    }
    if (i == 15) {
      error("no free reg");
    }
  }
}

static char *tmp_get(char *t) {
  int num = get_tmp_num(t);
  for(int i = 8; i<=15;i++) {
    if(mips_reg[i].tmp == num) {
      return mips_reg[i].name;
    }
  }
  error("not find tmp");
  return NULL;
}

void gen_ir2(char *op, char *t1, char *t2, char *ans) {
    printf("%s %s %s %s\n",op,t1,t2,ans);
}
