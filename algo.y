%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALGOS 100
#define MAX_PARAMS 10
#define MAX_VARS 100

typedef enum { ISET, IRETURN, ICALL } InstrType;
typedef enum { E_CONST, E_VAR, E_ADD, E_SUB, E_MUL, E_DIV } ExprType;

typedef struct ExprNode {
    ExprType type;
    int value;               // pour E_CONST
    char *var_name;          // pour E_VAR
    struct ExprNode *left;   // pour opérateurs binaires
    struct ExprNode *right;
} ExprNode;

typedef struct ArgNode {
    int value;
    struct ArgNode *next;
} ArgNode;

typedef struct Instruction {
    InstrType type;
    char *var_name;              // pour ISET
    ExprNode *expr;              // pour ISET ou IRETURN
    char *call_name;             // pour ICALL
    ArgNode *args;
    struct Instruction *next;
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

ArgNode* new_arg(int v) {
    ArgNode* n = malloc(sizeof(*n));
    n->value = v;
    n->next = NULL;
    return n;
}

ArgNode* add_arg(ArgNode* l, int v) {
    ArgNode* n = new_arg(v);
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
    }
    return 0;
}

void exec_instructions(Instruction *instr);

int call_function(char* name, ArgNode* args) {
    Algo* target = NULL;
    for (int i = 0; i < algo_count; i++) {
        if (strcmp(algo_table[i].name, name) == 0) {
            target = &algo_table[i];
            break;
        }
    }

    if (!target) {
        printf("Erreur : fonction %s inconnue\n", name);
        exit(1);
    }

    ArgNode* current = args;
    for (int i = 0; i < target->param_count; i++) {
        if (!current) {
            printf("Erreur : trop peu d’arguments pour %s\n", name);
            exit(1);
        }
        set_var(target->params[i], current->value);
        current = current->next;
    }

    if (current) {
        printf("Erreur : trop d’arguments pour %s\n", name);
        exit(1);
    }

    printf("CALL → %s(", name);
    for (int i = 0; i < target->param_count; i++) {
        printf("%d", get_var(target->params[i]));
        if (i < target->param_count - 1) printf(",");
    }
    printf(")\n");

    exec_instructions(target->instructions);

    return 0;
}

void exec_instructions(Instruction *instr) {
    while (instr) {
        switch (instr->type) {
            case ISET:
                set_var(instr->var_name, eval_expr(instr->expr));
                break;
            case IRETURN:
                printf("%d\n", eval_expr(instr->expr));
                exit(0);
            case ICALL:
                call_function(instr->call_name, instr->args);
                break;
        }
        instr = instr->next;
    }
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
%token TSET TRETURN TIF TELSE TDOWHILE TDOFORI TOD
%token TCALL TBEGINALGO TENDALGO
%token PLUS MINUS TIMES DIVIDE ASSIGN
%token EQ NEQ LE GE LT GT AND OR NOT
%token LBRACE RBRACE COMMA

%type <ival> programme
%type <arglist> args
%type <instr> instruction bloc
%type <expr> expression

%start programme

%%

programme:
    algos call_stmt
    ;

call_stmt:
    TCALL IDENTIFIER RBRACE LBRACE args RBRACE {
        call_function($2, $5);
    }
    ;

algos:
    algo
    | algos algo
    ;

algo:
    TBEGINALGO IDENTIFIER RBRACE LBRACE params RBRACE bloc TENDALGO {
        algo_table[algo_count].name = strdup($2);
        algo_table[algo_count].param_count = param_index;
        for (int i = 0; i < param_index; i++) {
            algo_table[algo_count].params[i] = strdup(param_list[i]);
        }
        algo_table[algo_count].instructions = $7;
        printf("Algo stocké : %s (%d param)\n", $2, param_index);
        algo_count++;
        param_index = 0;
    }
    ;

params:
    /* vide */                   { param_index = 0; }
    | IDENTIFIER                { param_list[param_index++] = strdup($1); }
    | params COMMA IDENTIFIER   { param_list[param_index++] = strdup($3); }
    ;

bloc:
    instruction                 { $$ = $1; }
    | bloc instruction          {
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
    ;

args:
    /* vide */                  { $$ = NULL; }
    | expression               { $$ = new_arg(eval_expr($1)); }
    | args COMMA expression    { $$ = add_arg($1, eval_expr($3)); }
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
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erreur : %s\n", s);
}

int main(void) {
    return yyparse();
}