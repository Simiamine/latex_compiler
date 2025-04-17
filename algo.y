%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALGOS 100
#define MAX_PARAMS 10
#define MAX_VARS 100

typedef struct ExprNode ExprNode;

typedef struct ArgNode {
    ExprNode *expr;
    struct ArgNode *next;
} ArgNode;

typedef enum { ISET, IRETURN, ICALL, IFOR, IIF } InstrType;
typedef enum { E_CONST, E_VAR, E_ADD, E_SUB, E_MUL, E_DIV, E_EQ, E_CALL } ExprType;

struct ExprNode {
    ExprType type;
    int value;
    char *var_name;
    ExprNode *left;
    ExprNode *right;
    char *call_name;
    ArgNode *args;
};

typedef struct Instruction {
    InstrType type;
    char *var_name;
    ExprNode *expr;
    char *call_name;
    ArgNode *args;
    struct Instruction *next;
    char *loop_var;
    ExprNode *from_expr;
    ExprNode *to_expr;
    struct Instruction *for_body;
    ExprNode *cond;
    struct Instruction *if_body;
    struct Instruction *else_body;
} Instruction;

typedef struct {
    char *name;
    char *params[MAX_PARAMS];
    int param_count;
    Instruction *instructions;
} Algo;

typedef struct {
    char *name;
    int value;
} Variable;

Algo algo_table[MAX_ALGOS];
int algo_count = 0;
char *param_list[MAX_PARAMS];
int param_index = 0;
Variable vars[MAX_VARS];
int var_count = 0;

int yylex(void);
void yyerror(const char *s);
int call_function(char* name, ArgNode* args);
int exec_instructions(Instruction *instr);

int get_var(const char *name) {
    for (int i = 0; i < var_count; i++)
        if (!strcmp(vars[i].name, name))
            return vars[i].value;
    fprintf(stderr, "Variable inconnue : %s\n", name);
    exit(1);
}

void set_var(const char *name, int val) {
    for (int i = 0; i < var_count; i++) {
        if (!strcmp(vars[i].name, name)) {
            vars[i].value = val;
            return;
        }
    }
    vars[var_count].name = strdup(name);
    vars[var_count].value = val;
    var_count++;
}

ArgNode* new_arg(ExprNode* e) {
    ArgNode* n = malloc(sizeof(*n));
    n->expr = e;
    n->next = NULL;
    return n;
}

ArgNode* add_arg(ArgNode* l, ExprNode* e) {
    ArgNode* n = new_arg(e);
    if (!l) return n;
    ArgNode* t = l;
    while (t->next) t = t->next;
    t->next = n;
    return l;
}

int eval_expr(ExprNode *e) {
    switch (e->type) {
        case E_CONST: return e->value;
        case E_VAR: return get_var(e->var_name);
        case E_ADD: return eval_expr(e->left) + eval_expr(e->right);
        case E_SUB: return eval_expr(e->left) - eval_expr(e->right);
        case E_MUL: return eval_expr(e->left) * eval_expr(e->right);
        case E_DIV: return eval_expr(e->left) / eval_expr(e->right);
        case E_EQ: return eval_expr(e->left) == eval_expr(e->right);
        case E_CALL: {
            printf("[EVAL] eval CALL %s(...)\n", e->call_name);
            return call_function(e->call_name, e->args);
        }
    }
    return 0;
}

int call_function(char* name, ArgNode* args) {
    Algo* target = NULL;
    for (int i = 0; i < algo_count; i++) {
        if (strcmp(algo_table[i].name, name) == 0) {
            target = &algo_table[i];
            break;
        }
    }

    if (!target) {
        printf("[ERROR] fonction inconnue : %s\n", name);
        exit(1);
    }

    ArgNode* current = args;
    for (int i = 0; i < target->param_count; i++) {
        if (!current) {
            printf("[ERROR] pas assez d'arguments pour %s\n", name);
            exit(1);
        }
        int val = eval_expr(current->expr);
        set_var(target->params[i], val);
        current = current->next;
    }

    if (current) {
        printf("[ERROR] trop d'arguments pour %s\n", name);
        exit(1);
    }

    printf("[EXEC] CALL %s(", name);
    for (int i = 0; i < target->param_count; i++) {
        printf("%d", get_var(target->params[i]));
        if (i < target->param_count - 1) printf(", ");
    }
    printf(")\n");

    int result = exec_instructions(target->instructions);
    return result;
}

int exec_instructions(Instruction *instr) {
    while (instr) {
        switch (instr->type) {
            case ISET:
                printf("[EXEC] SET %s = ...\n", instr->var_name);
                set_var(instr->var_name, eval_expr(instr->expr));
                break;
            case IRETURN: {
                int ret = eval_expr(instr->expr);
                printf("[EXEC] RETURN → %d\n", ret);
                return ret;
            }
            case ICALL:
                printf("[EXEC] CALL %s(...)\n", instr->call_name);
                call_function(instr->call_name, instr->args);
                break;
            case IFOR: {
                int start = eval_expr(instr->from_expr);
                int end = eval_expr(instr->to_expr);
                printf("[EXEC] FOR %s FROM %d TO %d\n", instr->loop_var, start, end);
                for (int i = start; i <= end; i++) {
                    set_var(instr->loop_var, i);
                    exec_instructions(instr->for_body);
                }
                break;
            }
            case IIF: {
                int cond = eval_expr(instr->cond);
                printf("[EXEC] IF condition = %d\n", cond);
                if (cond) {
                    return exec_instructions(instr->if_body);
                } else if (instr->else_body) {
                    return exec_instructions(instr->else_body);
                }
                break;
            }
        }
        instr = instr->next;
    }
    return 0;
}
%}

%union {
    int ival;
    char* str;
    struct ArgNode* arglist;
    struct Instruction* instr;
    struct ExprNode* expr;
}

%token <str> IDENTIFIER
%token <ival> NUMBER
%token TSET TRETURN TIF TELSE TFI TDOWHILE TDOFORI TOD
%token TCALL TBEGINALGO TENDALGO
%token PLUS MINUS TIMES DIVIDE ASSIGN
%token EQ NEQ LE GE LT GT AND OR NOT
%token LBRACE RBRACE COMMA

%type <arglist> args
%type <instr> instruction
%type <expr> expression
%type <instr> for_block if_block else_block
%type <instr> bloc

%start programme

%%

programme:
    algos call_stmt {
        printf("[DEBUG] programme → algos call_stmt\n");
    }
    ;

call_stmt:
    TCALL LBRACE IDENTIFIER RBRACE LBRACE args RBRACE {
        printf("[DEBUG] call_stmt → CALL {%s} {%p}\n", $3, $6);
        int res = call_function($3, $6);
        printf("[RESULT] Résultat de l'appel à %s : %d\n", $3, res);
    }
    ;

algos:
    algo
    | algos algo
    ;

algo:
    TBEGINALGO LBRACE IDENTIFIER RBRACE LBRACE params RBRACE bloc TENDALGO {
        printf("[DEBUG] algo → begin %s\n", $3);
        algo_table[algo_count].name = strdup($3);
        algo_table[algo_count].param_count = param_index;
        for (int i = 0; i < param_index; i++)
            algo_table[algo_count].params[i] = strdup(param_list[i]);
        algo_table[algo_count].instructions = $8;
        algo_count++;
        param_index = 0;
    }
    ;

params:
    /* vide */ { param_index = 0; }
    | IDENTIFIER { param_list[param_index++] = strdup($1); }
    | params COMMA IDENTIFIER { param_list[param_index++] = strdup($3); }
    ;

bloc:
    instruction { $$ = $1; }
    | bloc instruction {
        Instruction *last = $1;
        while (last->next) last = last->next;
        last->next = $2;
        $$ = $1;
    }
    ;

instruction:
    TSET LBRACE IDENTIFIER RBRACE LBRACE expression RBRACE {
        Instruction *i = malloc(sizeof(*i));
        i->type = ISET;
        i->var_name = strdup($3);
        i->expr = $6;
        i->next = NULL;
        $$ = i;
    }
    | TRETURN LBRACE expression RBRACE {
        Instruction *i = malloc(sizeof(*i));
        i->type = IRETURN;
        i->expr = $3;
        i->next = NULL;
        $$ = i;
    }
    | TCALL IDENTIFIER RBRACE LBRACE args RBRACE {
        Instruction *i = malloc(sizeof(*i));
        i->type = ICALL;
        i->call_name = strdup($2);
        i->args = $5;
        i->next = NULL;
        $$ = i;
    }
    | for_block { $$ = $1; }
    | if_block { $$ = $1; }
    ;

for_block:
    TDOFORI LBRACE IDENTIFIER RBRACE LBRACE expression RBRACE LBRACE expression RBRACE bloc TOD {
        Instruction *i = malloc(sizeof(*i));
        i->type = IFOR;
        i->loop_var = strdup($3);
        i->from_expr = $6;
        i->to_expr = $9;
        i->for_body = $11;
        i->next = NULL;
        $$ = i;
    }
    ;

if_block:
    TIF LBRACE expression RBRACE bloc else_block TFI {
        Instruction *i = malloc(sizeof(*i));
        i->type = IIF;
        i->cond = $3;
        i->if_body = $5;
        i->else_body = $6;
        i->next = NULL;
        $$ = i;
    }
    ;

else_block:
    /* vide */ { $$ = NULL; }
    | TELSE bloc { $$ = $2; }
    ;

args:
    /* vide */ { $$ = NULL; }
    | expression { $$ = new_arg($1); }
    | args COMMA expression { $$ = add_arg($1, $3); }
    ;

expression:
    NUMBER {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_CONST;
        e->value = $1;
        $$ = e;
    }
    | IDENTIFIER {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_VAR;
        e->var_name = strdup($1);
        $$ = e;
    }
    | expression PLUS expression {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_ADD;
        e->left = $1;
        e->right = $3;
        $$ = e;
    }
    | expression MINUS expression {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_SUB;
        e->left = $1;
        e->right = $3;
        $$ = e;
    }
    | expression TIMES expression {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_MUL;
        e->left = $1;
        e->right = $3;
        $$ = e;
    }
    | expression DIVIDE expression {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_DIV;
        e->left = $1;
        e->right = $3;
        $$ = e;
    }
    | expression EQ expression {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_EQ;
        e->left = $1;
        e->right = $3;
        $$ = e;
    }
    | expression ASSIGN expression {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_EQ;
        e->left = $1;
        e->right = $3;
        $$ = e;
    }
    | TCALL LBRACE IDENTIFIER RBRACE LBRACE args RBRACE {
        ExprNode *e = malloc(sizeof(*e));
        e->type = E_CALL;
        e->call_name = strdup($3);
        e->args = $6;
        $$ = e;
    }
%%

void yyerror(const char *s) {
    fprintf(stderr, "[ERROR] yyerror: %s\n", s);
}

int main(void) {
    printf("[INFO] Début du parsing\n");
    yyparse();
    printf("[INFO] Fin d'exécution\n");
    return 0;
}