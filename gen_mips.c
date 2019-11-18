#include "mcc.h"
static FILE *mips;

void gen_mips_init() {
    mips = fopen("mips.txt","w");
}

void gen_mips_end() {
    fprintf(mips,"$end:");
    fclose(mips);
}
// generate this env's declaration
void gen_global_decl() {
    Vector *v = env->vars->vals;
    fprintf(mips, ".data\n");
    for(int i = 0; i < v->len; i++) {    
        Var *var = v->data[i];
        var->global = true;
        printf("kind:%d type:%d name:%s val:%d\n", var->ty->kind, var->ty->ty, var->name, var->val);
        if(var->ty->kind == CONST) {
            continue;
        } else if(var->ty->kind == VAR) {               
            fprintf(mips, "%s: .space 4\n", var->name);
        } else if(var->ty->kind == ARY) {
            fprintf(mips,"%s: .space %d\n", var->name, (var->ty->len*4));
        } else {
            error("gen_global_decl wrong");
        }
    }
    fprintf(mips, ".text\n");
    fprintf(mips, "jal main\n");
    fprintf(mips, "j $end\n");
}
void gen_decl() {    
    bool global = env->prev == NULL ? true : false;
    Vector *v = env->vars->vals;
    int arg_offset = 2 + func_now->args->len;
    int local_offset = 0;
    for(int i = 0; i < v->len; i++) {    
        Var *var = v->data[i];
        var->global = global;
        printf("kind:%d type:%d name:%s val:%d %d\n", var->ty->kind, var->ty->ty, var->name, var->val,global);
        if(var->ty->kind == ARG) {
                var->val = (--arg_offset) * 4; {
                    if (arg_offset < 2) {
                        error("wrong arg_offset");
                    }
                }
            } else if (var->ty->kind == VAR) {
                local_offset++;
                var->val = -(local_offset * 4);
            } else if (var->ty->kind == ARY) {
                local_offset += var->ty->len;
                var->val = -(local_offset * 4);
            } 
            if (local_offset) {
                fprintf(mips, "addiu $sp $sp -%d\n",local_offset*4);
            }
        }
    }

static struct {
  char *name;
  int tmp;
} mips_reg[] = {
  {"$zero", 0}, {"$at",0}, {"$v0",0},{"$v1",0},{"$a0",0},
  {"$a1",0}, {"$a2",0},{"$a3",0},{"$t0",0}, {"$t1",0}, {"$t2",0}, {"$t3",0}, {"$t4",0}, {"$t5",0},
   {"$t6",0}, {"$t7",0}, {"$s0",0}, {"$s1",0}, {"$s2",0}, {"$s3",0}, {"$s4",0}, {"$s5",0}, {"$s6",0},
    {"$s7",0},{"$t8",0}, {"$t9", 0}, {"$gp",0},{"$sp",0},{"$fp",0},{"$ra",0}
};

static void printf_reg() {
    for(int i = 0; i < 18; i++)
    printf("%s %d\n",mips_reg[i].name, mips_reg[i].tmp);
}

static int get_tmp_num(char *t) {
  int ans = 0;
  for(int i = 2; t[i] != '\0'; i++) {
    ans = ans * 10 + (t[i] - '0');
  }
  return ans;
}

static char *tmp_alloc(char *t) {
    int i;
  for(i = 8; i<=15;i++) {
    if(mips_reg[i].tmp == 0) {
      mips_reg[i].tmp = get_tmp_num(t);
      break;
    }
    if (i == 15) {
      error("no free reg");
    }
  }
  return mips_reg[i].name;
}

static char *tmp_get(char *t) {
  int num = get_tmp_num(t);
  for(int i = 8; i<=15;i++) {
    if(mips_reg[i].tmp == num) {
      mips_reg[i].tmp = 0;
      return mips_reg[i].name;
    }
  }
  printf("error: %d\n",num);
  error("not find tmp");
  return NULL;
}

void gen_label(char *label) {
  fprintf(mips,"%s:\n",label);
}



static void compute(char *op, char *t1, char* t2, char *ans) {
    if (!strcmp(op,"+")) {
        fprintf(mips, "addu %s %s %s\n",tmp_alloc(ans),tmp_get(t1),tmp_get(t2));
    } else if (!strcmp(op,"-")) {
        if (!strcmp("$zero",t1)) {
            t1 = tmp_get(t1);
        }
        fprintf(mips, "subu %s %s %s\n",tmp_alloc(ans),t1,tmp_get(t2));
    } else if (!strcmp(op,"*")) {
        fprintf(mips, "mult %s %s\n",tmp_get(t1),tmp_get(t2));
        fprintf(mips, "mflo %s\n",tmp_alloc(ans));
    } else if (!strcmp(op,"/")) {
        fprintf(mips, "div %s %s\n",tmp_get(t1),tmp_get(t2));
        fprintf(mips, "mflo %s \n",tmp_alloc(ans));
    } else if (!strcmp(op,"sll")) {
        fprintf(mips, "sll %s %s %s\n",tmp_alloc(ans),tmp_get(t1),t2);
    }
}

static void branch(char *op, char *t1, char *t2) {
    if (!strcmp(op,"j")) {
        fprintf(mips, "j %s\n",t1);
    } else if (!strcmp(op,"beqz")) {
        fprintf(mips, "beqz %s %s\n",tmp_get(t1),t2);
    } else if (!strcmp(op,"bnez")) {
        fprintf(mips, "beqz %s %s\n",tmp_get(t1),t2);
    }
}

static void set(char *op, char *t1, char *t2, char *ans) {
    if (!strcmp(op,"==")) {
        op = "seq";
    } else if(!strcmp(op,"!=")) {
        op = "sne";
    } else if (!strcmp(op,"<")) {
        op = "slt";
    } else if(!strcmp(op,">")) {
        op = "sgt";
    } else if (!strcmp(op,"<=")) {
        op = "sle";
    } else if (!strcmp(op,">=")) {
        op = "sge";
    }
    fprintf(mips, "%s %s %s %s\n",op,tmp_alloc(ans),tmp_get(t1),tmp_get(t2));
}

static char *get_var(char *name) {
    Var *v = find_var(name);
    if (v->global) {
        return name;
    } else {
        StringBuilder *sb = new_sb();
        sb_append(sb, int_to_string(v->val));
        sb_append(sb, "($fp)");
        char *ans = sb_get(sb);
        free(sb);
        return ans;
    }
}

static void memory(char *op, char *t1, char *ans) {

    if (!strcmp(op,"la")) {
        fprintf(mips, "la %s %s\n",tmp_alloc(ans),get_var(t1));
    } else if (!strcmp(op,"lw")) {
        if(t1[0] == '$') {
            fprintf(mips, "lw %s (%s)\n",tmp_alloc(ans),tmp_get(t1));
        } else {
            fprintf(mips, "lw %s %s\n",tmp_alloc(ans),get_var(t1));
        }
    } else if (!strcmp(op,"sw")) {
        if(ans[0] == '$') {
            fprintf(mips, "sw %s (%s)\n",tmp_get(t1),tmp_get(ans));
        } else {
            fprintf(mips, "sw %s %s\n",tmp_get(t1),get_var(ans));
        }   
    } else if (!strcmp(op,"li")) {
        fprintf(mips, "li %s %s\n",tmp_alloc(ans),t1);
        printf_reg();
    } // TODO 'c'
}

void gen_function_begin() {
    fprintf(mips, "%s:\n",func_now->name);
    fprintf(mips, "addiu $sp $sp -8\n");
    fprintf(mips, "sw $ra 4($sp)\n");
    fprintf(mips, "sw $fp 0($sp)\n");
    fprintf(mips, "move $fp $sp\n");
}

void gen_function_end() {
    fprintf(mips, "%s_end:\n",func_now->name);
    fprintf(mips, "move $sp $fp\n");
    fprintf(mips, "lw $fp 0($sp)\n");
    fprintf(mips, "lw $ra 4($sp)\n");
    fprintf(mips, "jr $ra\n");
}

static void function(char *op, char *t1) {
    if (!strcmp(op, "jal")) {
        fprintf(mips, "jal %s\n",t1);
    } else {
        if (strlen(t1)) {
            fprintf(mips, "move $v0 %s\n",tmp_get(t1));
        }
        fprintf(mips, "j %s_end\n",func_now->name);
    }
} 

static void io(char *op, char *t1, char *t2, char *ans) {
    if(!strcmp(op,"PRINT")) {
        if(!strcmp(t2,"S")) {
            for(int i = 0; i < strlen(t1); i++) {
               fprintf(mips, "li $a0 %d\n",(int)(t1[i]));
               fprintf(mips, "li $v0 11\n");
               fprintf(mips, "syscall\n"); 
            }
        } else if (!strcmp(t2, "N")) {
            fprintf(mips, "li $a0 %d\n",(int)('\n'));
            fprintf(mips, "li $v0 11\n");
            fprintf(mips, "syscall\n");
        } else {
            fprintf(mips, "move $a0 %s\n", tmp_get(t1));
            if (!strcmp(t2, "I")) {
                fprintf(mips, "li $v0 1\n");
            } else if (!strcmp(t2, "C")) {
                fprintf(mips, "li $v0 11\n");
            }
            fprintf(mips, "syscall\n"); 
        }
    } else if(!strcmp(op, "SCANF")) {
        Var *v = find_var(ans);
        if (v->ty->ty == CHARTK) {
            fprintf(mips, "li $v0 12\n");
        } else {
            fprintf(mips, "li $v0 5\n");
        }
        fprintf(mips, "syscall\n");
        fprintf(mips, "sw $v0 %s\n",get_var(ans));
    }
}

void gen_mips(char *op, char *t1, char* t2, char *ans) {
    if (!strcmp(op,"+") || !strcmp(op,"-") ||!strcmp(op,"/") ||!strcmp(op,"*") ||!strcmp(op,"sll")) {
        compute(op,t1,t2,ans);
    } else if (!strcmp(op,"j") || !strcmp(op,"beqz") ||!strcmp(op,"bnez")) {
        branch(op,t1,t2);
    } else if (!strcmp(op,"==") || !strcmp(op,"!=") ||
                !strcmp(op,"<") ||!strcmp(op,">") ||
                !strcmp(op,"<=") || !strcmp(op,">=")) {
        set(op,t1,t2,ans);
    } else if (!strcmp(op,"la") || !strcmp(op,"lw") || !strcmp(op,"sw") || !strcmp(op,"li")) {
        memory(op,t1,ans);
    } else if (!strcmp(op, "ret") || (!strcmp(op, "jal"))) {
        function(op,t1);
    } else if(!strcmp(op,"PRINT") || (!strcmp(op, "SCANF"))) {
        io(op, t1, t2, ans);
    } else if(!strcmp(op, "move")) {
        fprintf(mips, "move %s %s\n",tmp_alloc(t1),t2);
    } else if(!strcmp(op, "push")) {
        fprintf(mips, "addiu $sp $sp -4\n");
        fprintf(mips, "sw %s ($sp)\n",tmp_get(t1));
    }
}

void gen_ir2(char *op, char *t1, char *t2, char *ans) {
    printf("%s %s %s %s\n",op,t1,t2,ans);
    gen_mips(op,t1,t2,ans);
}
