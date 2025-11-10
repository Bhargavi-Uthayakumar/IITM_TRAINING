%{
#include "mini_sql.tab.h"
#include <stdlib.h>
%}

%%
"INSERT"             return INSERT;
"INTO"               return INTO;
"VALUES"             return VALUES;
"SELECT"             return SELECT;
"FROM"               return FROM;
"SUM"                return SUM;
"AVG"                return AVG;
"MIN"                return MIN;
"MAX"                return MAX;
"COUNT"              return COUNT;
[0-9]+               { yylval.num = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.str = strdup(yytext); return IDENTIFIER; }
","                  return COMMA;
"("                  return LPAREN;
")"                  return RPAREN;
";"                  return SEMICOLON;
[ \t\n\r]+           ;  // ignore whitespace
.                    return yytext[0];
%%
