%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void execute_insert(char *table, int *values, int count);
extern void execute_select(char *agg, char *column, char *table);

void yyerror(const char *s);
int yylex(void);
%}

%union {
    int num;
    char *str;
    struct {
        int *values;
        int count;
    } num_list;
}

%token <str> IDENTIFIER
%token <num> NUMBER
%token INSERT INTO VALUES SELECT FROM
%token SUM AVG MIN MAX COUNT
%token COMMA LPAREN RPAREN SEMICOLON

%type <num_list> number_list
%type <str> agg_func

%start input

%%

input:
      /* empty */
    | input command
    ;

command:
      insert_stmt
    | select_stmt
    ;

insert_stmt:
      INSERT INTO IDENTIFIER VALUES LPAREN number_list RPAREN opt_semicolon
        {
            execute_insert($3, $6.values, $6.count);
            free($3);
        }
    ;

select_stmt:
      SELECT agg_func LPAREN IDENTIFIER RPAREN FROM IDENTIFIER opt_semicolon
        {
            execute_select($2, $4, $7);
            free($4);
            free($7);
        }
    ;

opt_semicolon:
      /* empty */
    | SEMICOLON
    ;

number_list:
      NUMBER
        {
            $$.values = malloc(sizeof(int));
            $$.values[0] = $1;
            $$.count = 1;
        }
    | number_list COMMA NUMBER
        {
            $$.values = realloc($1.values, sizeof(int) * ($1.count + 1));
            $$.values[$1.count] = $3;
            $$.count = $1.count + 1;
        }
    ;

agg_func:
    SUM     { $$ = strdup("SUM"); }
    | AVG     { $$ = strdup("AVG"); }
    | MIN     { $$ = strdup("MIN"); }
    | MAX     { $$ = strdup("MAX"); }
    | COUNT   { $$ = strdup("COUNT"); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}
