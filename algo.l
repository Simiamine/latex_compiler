%{
#include "algo.tab.h"
#include <stdio.h>
#include <stdlib.h>
%}

%%

"\\SET"                 { return TSET; }
"\\RETURN"              { return TRETURN; }
"\\IF{"                 { return TIF; }
"\\ELSE"                { return TELSE; }        /* pas encore utilisé */
"\\DOWHILE{"            { return TDOWHILE; }     /* pas encore utilisé */
"\\DOFORI{"             { return TDOFORI; }
"\\OD"                  { return TOD; }          /* fin de DOFORI      */
"\\CALL{"               { return TCALL; }
"\\begin{algo}{"        { return TBEGINALGO; }
"\\end{algo}"           { return TENDALGO; }

"{"                     { return LBRACE; }
"}"                     { return RBRACE; }
","                     { return COMMA; }

"=="                    { return EQ; }
"!="                    { return NEQ; }
"<="                    { return LE; }
">="                    { return GE; }
"<"                     { return LT; }
">"                     { return GT; }
"&&"                    { return AND; }
"||"                    { return OR; }
"!"                     { return NOT; }
"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return TIMES; }
"/"                     { return DIVIDE; }
"="                     { return ASSIGN; }

[0-9]+                  { yylval.ival = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str  = strdup(yytext); return IDENTIFIER; }

[ \t\r\n]+              ;               /* espaces = ignorés         */
.                       { printf("Caractère inconnu : %s\n", yytext); }

%%