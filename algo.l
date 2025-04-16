%{
#include "algo.tab.h"
#include <stdio.h>
#include <stdlib.h>
%}

%%

"\\SET"         { return TSET; }
"\\RETURN"      { return TRETURN; }
"\\IF"          { return TIF; }
"\\ELSE"        { return TELSE; }
"\\DOWHILE"     { return TDOWHILE; }
"\\DOFORI"      { return TDOFORI; }
"\\CALL{"             { return TCALL; }
"\\begin{algo}{"      { return TBEGINALGO; }
"\\end{algo}"         { return TENDALGO; }

[0-9]+          { yylval.ival = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.str = strdup(yytext); return IDENTIFIER; }

"{"             { return LBRACE; }
"}"             { return RBRACE; }
","             { return COMMA; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"=="            { return EQ; }
"!="            { return NEQ; }
"<="            { return LE; }
">="            { return GE; }
"<"             { return LT; }
">"             { return GT; }
"&&"            { return AND; }
"||"            { return OR; }
"!"             { return NOT; }
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return TIMES; }
"/"             { return DIVIDE; }
"="             { return ASSIGN; }

[ \t\r\n]+      ; // ignorer les espaces

.               { printf("Caractère inconnu : %s\n", yytext); }

%%