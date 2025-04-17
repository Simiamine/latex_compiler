%{
#include "algo.tab.h"
#include <stdio.h>
#include <stdlib.h>
%}

%%

"\\SET"                 { printf("[LEX] Token: \\SET\n"); return TSET; }
"\\RETURN"              { printf("[LEX] Token: \\RETURN\n"); return TRETURN; }
"\\IF"                  { printf("[LEX] Token: \\IF\n"); return TIF; }
"\\ELSE"                { printf("[LEX] Token: \\ELSE\n"); return TELSE; }
"\\FI"                  { printf("[LEX] Token: \\FI\n"); return TFI; }
"\\DOWHILE"             { printf("[LEX] Token: \\DOWHILE\n"); return TDOWHILE; }
"\\DOFORI"              { printf("[LEX] Token: \\DOFORI\n"); return TDOFORI; }
"\\OD"                  { printf("[LEX] Token: \\OD\n"); return TOD; }
"\\CALL"                { printf("[LEX] Token: \\CALL\n"); return TCALL; }
"\\begin{algo}"         { printf("[LEX] Token: \\begin{algo}\n"); return TBEGINALGO; }
"\\end{algo}"           { printf("[LEX] Token: \\end{algo}\n"); return TENDALGO; }

"{"                     { printf("[LEX] Token: {\n"); return LBRACE; }
"}"                     { printf("[LEX] Token: }\n"); return RBRACE; }
","                     { printf("[LEX] Token: ,\n"); return COMMA; }

"=="                    { printf("[LEX] Token: ==\n"); return EQ; }
"!="                    { printf("[LEX] Token: !=\n"); return NEQ; }
"<="                    { printf("[LEX] Token: <=\n"); return LE; }
">="                    { printf("[LEX] Token: >=\n"); return GE; }
"<"                     { printf("[LEX] Token: <\n"); return LT; }
">"                     { printf("[LEX] Token: >\n"); return GT; }
"&&"                    { printf("[LEX] Token: &&\n"); return AND; }
"||"                    { printf("[LEX] Token: ||\n"); return OR; }
"!"                     { printf("[LEX] Token: !\n"); return NOT; }
"+"                     { printf("[LEX] Token: +\n"); return PLUS; }
"-"                     { printf("[LEX] Token: -\n"); return MINUS; }
"*"                     { printf("[LEX] Token: *\n"); return TIMES; }
"/"                     { printf("[LEX] Token: /\n"); return DIVIDE; }
"="                     { printf("[LEX] Token: =\n"); return ASSIGN; }

[0-9]+ {
    yylval.ival = atoi(yytext);
    printf("[LEX] Token: NUMBER (%d)\n", yylval.ival);
    return NUMBER;
}

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.str = strdup(yytext);
    printf("[LEX] Token: IDENTIFIER (%s)\n", yylval.str);
    return IDENTIFIER;
}

[ \t\r\n]+              ; /* blancs et sauts de ligne ignorés */

. {
    printf("[LEX] Caractère inconnu : %s\n", yytext);
}

%%