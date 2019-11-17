#include "mcc.h"

// previous read and Token stream list
static List* list;
static List* pre;

// function using in analysis program
static void get_token(void);
static void pre_read(void);
static void pre_read_end(void);
static void match(int);
static int check_declaration(void);

// function for analysis program
static int unsigned_integer(void);
static int integer(void);
static void const_declaration_all(void);
static void const_declaration(void);
static void var_declaration_all(void);
static void var_declaration(void);
static void func_declaration_all(void);
static void func_declaration(void);
static void main_func(void);
static void func_declaration_without_return(void);
static Type* expression(void);
static void statement(void);
static void func_declaration_without_return(void);
static void func_declaration_with_return(void);
static void statement_list(void);
static void compound_statement(void);
static void func_reference_without_return(Function*);
static void func_reference_with_return(Function*);
static void string_ascii(void);
static Type* func_reference(void);

static void get_token(void) {
  if (token != NULL) {
    free(token);
  }
  token = list_empty(list) ? next() : list_poll(list);
}

// pre_read and pre_read_end must be used together
static void pre_read(void) {
  list_offer(pre, token);
  token = NULL;
  get_token();
}

static void pre_read_end(void) {
  list_offer(pre, token);
  token = NULL;
  if (list_empty(pre)) {
    //printf("list empty!");
    return;
  } else {
    list_append(pre, list);
    list = pre;
    pre = new_list();
    get_token();
  }
}

// match is to identify now and get next
static void match(int tk) {
  if (token->kind == tk) {
    get_token();
  } else {
    error("other error");
    //printf("%d\n",tk);
    exit(-1);
  }
}

static bool consume(int tk) {
  if (token->kind == tk) {
    get_token();
    return true;
  } else {
    return false;
  }
}

enum {
  OTHER_DECL,
  VAR_DECL,
  FUNC_DECL,
  MAIN_DECL,
  CONST_DECL,
};

static int check_declaration() {
  int type;
  switch(token->kind) {
    case INTTK:
    case CHARTK:
      pre_read();
      pre_read();
      type = token->kind == LPARENT ? FUNC_DECL : VAR_DECL;
      pre_read_end();
      break;
    case VOIDTK:
      pre_read();
      type = token->kind == IDENFR ? FUNC_DECL : MAIN_DECL;
      pre_read_end();
      break;
    case CONSTTK:
      type = CONST_DECL;
      break;
    default:
      type = OTHER_DECL;
  }
  ////printf("my: %d %d %d %d %d %s\n",type,line,pre->len,list->len,token->kind,token->data);
  //printf("this: %d",type);
  return type;
}
// label tmp generator
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
  char *ans = malloc((len+1)*sizeof(char));
  sprintf(ans,"%d",val);
  return ans;
}

enum {NOW,NEW};

static char *get_label(int add) {
  static int count = 0;
  count += add;
  StringBuilder *sb = new_sb();
  sb_append_n(sb, "$L", 2);
  sb_append(sb, int_to_string(count));
  char *ans = sb_get(sb);
  free(sb);
  return ans;
}

static char *get_tmp(int add) {
  static int count = 0;
  count += add;
  StringBuilder *sb = new_sb();
  sb_append_n(sb, "t", 1);
  sb_append(sb, int_to_string(count));
  char *ans = sb_get(sb);
  free(sb);
  return ans;
}



void program(void) {
  // identify now declaration's type
  int del_type;
  env = new_env(NULL);
  list = new_list();
  pre = new_list();
  funcs = new_vec();

  get_token();

  const_declaration_all();

  var_declaration_all();
  gen_decl();
  func_declaration_all();
  main_func();

  //printf("<程序>");
}

static int unsigned_integer(void) {
  int ans = 0;
  if (token->kind == INTCON) {
    for(int i = 0; token->data[i] != '\0'; i++) {
      ans = ans * 10 + (token->data[i] - '0');
    }
    match(INTCON);
  }
  //printf("<无符号整数>\n");
  return ans;
}

static int integer(void) {
  bool flag = true;
  if (token->kind == PLUS) {
    match(PLUS);
  } else if (token->kind == MINU) {
    flag = false;
    match(MINU);
  }
  int ans = unsigned_integer();
  //printf( "<整数> %d %d\n",ans,flag);
  return flag ? ans : -ans;
}

static void const_declaration_all(void) {
  while (check_declaration() == CONST_DECL) {
    const_declaration();
    match(SEMICN);
  }
  //printf("<常量说明>\n");
}

static void const_declaration(void) {
  Type *tp = new_type(0, CONST);
  match(CONSTTK);
  tp->ty = token->kind;
  match(token->kind);
  do {
    Var *v = add_var(tp, token->data);
    match(IDENFR);
    match(ASSIGN);
    //printf("type is %d\n",tp->ty);
    if (tp->ty == INTTK) {
      v->val = integer();
    } else if (tp->ty == CHARTK){
      v->val = (int)(token->data[0]);
      match(CHARCON);
    }
  } while(consume(COMMA));
  //printf("<常量定义>\n");
}

static void var_declaration_all(void) {
    //printf("im in var_decl_now! %d %s\n",token->kind, token->data);
  while (check_declaration() == VAR_DECL){
    var_declaration();
    match(SEMICN);
  }
  //printf( "<变量说明>\n");
}


static void var_declaration(void) {
  int type = token->kind;
  Type* tp;
  match(type);
  do {
    tp = new_type(type, 0);
    add_var(tp, token->data);
    match(IDENFR);
    if (token->kind == LBRACK) {
      tp->kind = ARY;
      match(LBRACK);
      tp->len = unsigned_integer();
      match(RBRACK);
    } else {
      tp->kind = VAR;
    }
  } while (consume(COMMA));
  //printf( "<变量定义>\n");
}

static Type *declaration_head(Function* func) {
  Type *tp = new_type(token->kind, FUNC);
  match(token->kind);
  func->name = token->data;
  add_var(tp, token->data);
  match(IDENFR);

  //printf( "<声明头部>\n");
  return tp;
}

//
static void para_list(Function* func) {
  Var* var;
  Type* tp;
  if (token->kind == RPARENT) {
    return;
  }
  do {
    tp = new_type(token->kind, VAR);
    match(token->kind);
    var = add_var(tp, token->data);
    vec_union1(func->args, var);
    match(IDENFR);
  } while(consume(COMMA));
  //printf("<参数表>\n");
}

static void func_declaration(void) {
  Function* func = new_func();
  Type *tp = declaration_head(func);
  env = new_env(env);
  vec_union1(funcs, func);

  match(LPARENT);
  para_list(func);
  match(RPARENT);
  match(LBRACE);
  compound_statement();
  match(RBRACE);
  env = env->prev;
  //printf("<有返回值函数定义>\n");
}

static void func_declaration_all(void) {
  while (check_declaration() == FUNC_DECL) {
      func_declaration();
  }
}

static Type* factor(void) {
  Type *tp = new_type(INTTK, EXP);
  int tmp_num;
  char tmp_char;
  switch(token->kind) {
    case INTCON:
    case PLUS:
    case MINU:
      tmp_num = integer();
      gen_ir2("", int_to_string(tmp_num), "", get_tmp(NEW));
      break;
    case CHARCON:
      tp->ty = CHARTK;
      gen_ir2("", token->data, "", get_tmp(NEW));
      match(CHARCON);
      break;
    case LPARENT:
      match(LPARENT);
      expression();
      match(RPARENT);
      break;
    case IDENFR:
      Var *v = find_var(token->data);
      tp = v->ty;
      if (tp->kind == FUNC) {
        func_reference();
      } else if (tp->kind == ARY) {
        match(IDENFR);
        match(LBRACK);
        expression();
        match(RBRACK);
        char *exp = get_tmp(NOW);
        gen_ir2("", v->name, exp, get_tmp(NEW));
      } else {
        gen_ir2("", v->name, "", get_tmp(NEW));
        match(IDENFR);
      }
      break;
    default:
      break;
  }
  //printf( "<因子>\n");
  return tp;
}

// var->ty cant be change
static Type* item(void) {
  Type* tp = factor();
  Type *ans = new_type(tp->ty, EXP);
  char *reg1 = get_tmp(NOW);
  while (token->kind == MULT || token->kind == DIV) {
    char *op = token->data;
    ans->ty = INTTK;
    match(token->kind);
    factor();
    char *reg2 = get_tmp(NOW);
    char *new = get_tmp(NEW);
    gen_ir2(op, reg1, reg2, new);
    reg1 = new;
  }
  //printf( "<项>\n");
  return ans;
}

static Type* expression(void) {
  Type* tp;
  bool positive = true;
  if (token->kind == PLUS || token->kind == MINU) {
    positive = token->kind == MINU ? false : true;
    match(token->kind);
  }
  tp = item();
  char *reg1 = get_tmp(NOW);
  while (token->kind == PLUS || token->kind == MINU) {
    char *op = token->data;
    tp->ty = INTTK;
    match(token->kind);
    item();
    char *reg2 = get_tmp(NOW);
    char *new = get_tmp(NEW);
    gen_ir2(op, reg1, reg2, new);
    reg1 = new;
  }
  if (!positive) {
    gen_ir2("-", "0", reg1, get_tmp(NEW));
  }
  //printf( "<表达式>\n");
  return tp;
}

static void main_func(void) {
  match(VOIDTK);
  match(MAINTK);
  match(LPARENT);
  match(RPARENT);
  env = new_env(env);
  match(LBRACE);
  compound_statement();
  match(RBRACE);
  env = env->prev;
  //printf( "<主函数>\n");
}



// ! expression must be integer
static void conditional(void) {
  Type *tp = expression();
  if (token->kind == LSS || token->kind == LEQ || token->kind == GRE ||
      token->kind == GEQ || token->kind == EQL || token->kind == NEQ)
  {
    char *tmp1 = get_tmp(NOW);
    char *op = token->data;
    match(token->kind);
    tp = expression();
    char *tmp2 = get_tmp(NOW);
    gen_ir2(op, tmp1, tmp2, get_tmp(NEW));
  }
  //printf( "<条件>\n");
}

static void conditional_statement() {
  // //printf("now token->kind is %d",token->kind);
  match(IFTK);
  match(LPARENT);
  conditional();
  char *label = get_label(NEW);
  gen_ir2("BZ", get_tmp(NOW), label, "");
  match(RPARENT);
  statement();
  if (token->kind == ELSETK) {
    char *label2 = get_label(NEW);
    gen_ir2("GOTO", label2, "", "");
    match(ELSETK);
    gen_ir2(label, "", "", "");
    statement();
    gen_ir2(label2, "", "", "");
  } else {
    gen_ir2(label, "", "", "");
  }
  //printf( "<条件语句>\n");
}

static int step_len(void) {
  return unsigned_integer();
  //printf( "<步长>\n");
}

static void for_statement() {
  char *label1 = get_label(NEW);
    char *label2 = get_label(NEW);
    match(FORTK);
    match(LPARENT);
    Var *v1 = find_var(token->data);
    match(IDENFR);
    match(ASSIGN);
    expression();
    char *tmp = get_tmp(NOW);
    gen_ir2("=", tmp, "", v1->name);
    match(SEMICN);
    gen_ir2(label1, "", "", "");
    conditional();
    match(SEMICN);
    gen_ir2("BZ", get_tmp(NOW), label2, "");
    Var *v2 = find_var(token->data);
    match(IDENFR);
    match(ASSIGN);
    Var *v3 = find_var(token->data);
    match(IDENFR);
    char *op = token->data;
    if (token->kind == PLUS) {
      match(PLUS);
    } else if (token->kind == MINU) {
      match(MINU);
    } else {
      error("for statement need an add symbol");
    }
    int step = step_len();
    match(RPARENT);
    gen_ir2("+", v1->name, int_to_string(step), v2->name);
    statement();
    gen_ir2("GOTO", label1, "", "");
    gen_ir2(label2, "", "", "");
}

static void loop_statement() {
  if (token->kind == WHILETK) {
    char *label1 = get_label(NEW);
    char *label2 = get_label(NEW);
    gen_ir2(label1, "", "", "");
    match(WHILETK);
    match(LPARENT);
    conditional();
    match(RPARENT);
    gen_ir2("BZ", get_tmp(NOW), label2, "");
    statement();
    gen_ir2("GOTO", label1, "", "");
    gen_ir2(label2, "", "", "");
  } else if (token->kind == DOTK) {
    char *label = get_label(NEW);
    gen_ir2(label, "", "", "");
    match(DOTK);
    statement();
    match(WHILETK);
    match(LPARENT);
    conditional();
    gen_ir2("BNZ", get_tmp(NOW), label, "");
    match(RPARENT);
  } else if (token->kind == FORTK) {
    for_statement();
  }
  //printf( "<循环语句>\n");
}

static void print_statement(void) {
  match(PRINTFTK);
  match(LPARENT);
  if (token->kind == STRCON) {
    string_ascii();
    if (token->kind == COMMA) {
      match(COMMA);
      expression();
    }
  } else {
    expression();
  }
  match(RPARENT);
  //printf( "<写语句>\n");
}

static void scanf_statement(void) {
  match(SCANFTK);
  match(LPARENT);
  match(IDENFR);
  while (token->kind == COMMA) {
    match(COMMA);
    match(IDENFR);
  }
  match(RPARENT);
  //printf( "<读语句>\n");
}

static void return_statement() {
  match(RETURNTK);
  if (token->kind == LPARENT) {
    match(LPARENT);
    Type *tp = expression();
    match(RPARENT);
    gen_ir2("RET", get_tmp(NOW), "", "");
  } else {
    gen_ir2("RET", "", "", "");
  }
  //printf( "<返回语句>\n");
}

static void assign_statement(void) {
  Var *v = find_var(token->data);
  match(IDENFR);
  if (token->kind == LBRACK) {
    match(LBRACK);
    Type *tp = expression();
    match(RBRACK);
  }
  match(ASSIGN);
  expression();
  gen_ir2("", get_tmp(NOW), "", v->name);
  //printf( "<赋值语句>\n");
}

static void para_ref_list(Function* func) {
  Var* v;
  Type* tp;
  int pos = 0;
  if (token->kind == RPARENT) {
    return;
  }
  do {
    tp = expression();
    gen_ir2("PUSH",get_tmp(NOW),"","");
  } while (consume(COMMA));

  //printf( "<值参数表>\n");
}

static Type* func_reference(void) {
  int i;
  Type* tp;
  Function* func = NULL;
  // print_token->kind();
  tp = (find_var(token->data))->ty;
  for (i = 0; i < funcs->len; i++) {
    func = (Function*)funcs->data[i];
    if (strcmp(token->data, func->name) == 0) {
      break;
    }
  }
  if (tp->ty == VOIDTK || tp->ty == INTTK || tp->ty == CHARTK) {
    match(IDENFR);
    match(LPARENT);
    para_ref_list(func);
    match(RPARENT);
  } else {
    error("undefine function");
  }
  gen_ir2("CALL",func->name,"","");
  return tp;
}

static void statement() {
  //printf("statement:%d %s\n", token->kind, token->data);
  if (token->kind == IFTK) {
    conditional_statement();
  } else if (token->kind == DOTK || token->kind == WHILETK ||
             token->kind == FORTK) {
    loop_statement();
  } else if (token->kind == RETURNTK) {
    return_statement();
    match(SEMICN);
  } else if (token->kind == PRINTFTK) {
    print_statement();
    match(SEMICN);
  } else if (token->kind == SCANFTK) {
    scanf_statement();
    match(SEMICN);
  } else if (token->kind == LBRACE) {
    match(LBRACE);
    statement_list();
    match(RBRACE);
  } else if (token->kind == SEMICN) {
    match(SEMICN);
  } else if (token->kind == IDENFR) {
    pre_read();
    if (token->kind == ASSIGN || token->kind == LBRACK) {
      pre_read_end();
      assign_statement();
    } else if (token->kind == LPARENT) {
      pre_read_end();
      func_reference();
    }
    match(SEMICN);
  } else {
    // print_token->kind();
    error("statement no match");
  }
  //printf( "<语句>\n");
}

static void statement_list() {
  while (token->kind != RBRACE) {
    statement();
  }
  //printf( "<语句列>\n");
}

static void compound_statement() {
  if (token->kind == CONSTTK) {
    const_declaration_all();
  }
  if (token->kind == INTTK || token->kind == CHARTK) {
    var_declaration_all();
  }
  gen_decl();
  statement_list();
  //printf( "<复合语句>\n");
}



// string() may be conflict with lib function
static void string_ascii(void) {
  match(STRCON);
  //printf( "<字符串>\n");
}
