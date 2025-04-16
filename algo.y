%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALGOS 100
#define MAX_PARAMS 10

typedef struct Instruction {
    char* code; // Placeholder pour l'instant
    struct Instruction* next;
} Instruction;

typedef struct {
    char* name;
    char* params[MAX_PARAMS];
    int param_count;
    Instruction* instructions;
} Algo;

Algo algo_table[MAX_ALGOS];
int algo_count = 0;

char* param_list[MAX_PARAMS];
int param_index = 0;

int yylex();
void yyerror(const char *s);

typedef struct {
    char* name;
    int value;
} Variable;

typedef struct ArgNode {
    int value;
    struct ArgNode* next;
} ArgNode;

ArgNode* new_arg_list(int val) {
    ArgNode* node = malloc(sizeof(ArgNode));
    node->value = val;
    node->next = NULL;
    return node;
}

ArgNode* append_arg(ArgNode* list, int val) {
    ArgNode* node = new_arg_list(val);
    ArgNode* temp = list;
    while (temp->next) temp = temp->next;
    temp->next = node;
    return list;
}

#define MAX_VARS 100
Variable vars[MAX_VARS];
int var_count = 0;

int get_var(const char* name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0)
            return vars[i].value;
    }
    printf("Variable inconnue : %s\n", name);
    exit(1);
}

void set_var(const char* name, int val) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0) {
            vars[i].value = val;
            return;
        }
    }
    vars[var_count].name = strdup(name);
    vars[var_count].value = val;
    var_count++;
}

int call_function(char* name, ArgNode* args) {
    printf("CALL à %s avec args : ", name);
    while (args) {
        printf("%d ", args->value);
        args = args->next;
    }
    printf("\n");
    return 0;
}
%}

%union {
    int ival;
    char* str;
    struct ArgNode* arglist;
}

%token <str> IDENTIFIER
%token <ival> NUMBER
%token TSET TRETURN TIF TELSE TDOWHILE TDOFORI TCALL TBEGINALGO TENDALGO
%token PLUS MINUS TIMES DIVIDE ASSIGN
%token EQ NEQ LE GE LT GT AND OR NOT
%token LBRACE RBRACE COMMA LPAREN RPAREN

%type <ival> expression
%type <arglist> args

%start programme

%%

programme:
    algos TCALL LBRACE IDENTIFIER RBRACE LBRACE args RBRACE {
        call_function($4, $7);
    }
    ;

algos:
      algo
    | algos algo
    ;

algo:
    TBEGINALGO IDENTIFIER LPAREN params RPAREN bloc TENDALGO
    {
        algo_table[algo_count].name = strdup($2);
        algo_table[algo_count].param_count = param_index;
        for (int i = 0; i < param_index; i++) {
            algo_table[algo_count].params[i] = strdup(param_list[i]);
        }
        algo_table[algo_count].instructions = NULL; // à implémenter à l'étape suivante
        printf("Algo stocké : %s avec %d paramètre(s)\n", $2, param_index);
        algo_count++;
        param_index = 0;
    }
    ;

params:
      /* vide */                     { param_index = 0; }
    | IDENTIFIER                    { param_list[param_index++] = strdup($1); }
    | params COMMA IDENTIFIER       { param_list[param_index++] = strdup($3); }
    ;

bloc:
      instruction
    | bloc instruction
    ;

instruction:
      TSET IDENTIFIER ASSIGN expression           { set_var($2, $4); }
    | TRETURN expression                          { printf("%d\n", $2); exit(0); }
    | TCALL IDENTIFIER LPAREN args RPAREN         { call_function($2, $4); }
    ;

args:
      /* vide */                 { $$ = NULL; }
    | expression                { $$ = new_arg_list($1); }
    | args COMMA expression     { $$ = append_arg($1, $3); }
    ;

expression:
      expression PLUS expression                  { $$ = $1 + $3; }
    | expression MINUS expression                 { $$ = $1 - $3; }
    | expression TIMES expression                 { $$ = $1 * $3; }
    | expression DIVIDE expression                { $$ = $1 / $3; }
    | NUMBER                                      { $$ = $1; }
    | IDENTIFIER                                  { $$ = get_var($1); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erreur de parsing : %s\n", s);
}

int main() {
    return yyparse();
}