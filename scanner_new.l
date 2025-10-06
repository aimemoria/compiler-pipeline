%{
/* LEXICAL ANALYZER (SCANNER) - CST-405 Complete Compiler Project
 * Enhanced version with parser integration and while loop support
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"

int line_num = 1;
int col_num = 1;
int char_count = 0;

void update_location();
%}

%option nounput
%option noinput

DIGIT       [0-9]
LETTER      [a-zA-Z]
ID          {LETTER}({LETTER}|{DIGIT}|_)*
NUM         {DIGIT}+
WS          [ \t]

%%

"int"           { update_location(); yylval.str = strdup(yytext); return INT; }
"print"         { update_location(); yylval.str = strdup(yytext); return PRINT; }
"while"         { update_location(); yylval.str = strdup(yytext); return WHILE; }

{ID}            { update_location(); yylval.str = strdup(yytext); return ID; }
{NUM}           { update_location(); yylval.num = atoi(yytext); return NUM; }

"+"             { update_location(); yylval.str = strdup(yytext); return PLUS; }
"="             { update_location(); yylval.str = strdup(yytext); return ASSIGN; }

"<"             { update_location(); yylval.str = strdup(yytext); return RELOP; }
">"             { update_location(); yylval.str = strdup(yytext); return RELOP; }
"<="            { update_location(); yylval.str = strdup(yytext); return RELOP; }
">="            { update_location(); yylval.str = strdup(yytext); return RELOP; }
"=="            { update_location(); yylval.str = strdup(yytext); return RELOP; }
"!="            { update_location(); yylval.str = strdup(yytext); return RELOP; }

";"             { update_location(); yylval.str = strdup(yytext); return SEMICOLON; }
"("             { update_location(); yylval.str = strdup(yytext); return LPAREN; }
")"             { update_location(); yylval.str = strdup(yytext); return RPAREN; }
"{"             { update_location(); yylval.str = strdup(yytext); return LBRACE; }
"}"             { update_location(); yylval.str = strdup(yytext); return RBRACE; }

{WS}+           { col_num += yyleng; char_count += yyleng; }
\n              { line_num++; col_num = 1; char_count++; }

.               { fprintf(stderr, "LEXICAL ERROR at Line %d, Col %d: Unrecognized character '%c'\n", line_num, col_num, *yytext); col_num++; char_count++; }

%%

void update_location() {
    col_num += yyleng;
    char_count += yyleng;
}

int yywrap() {
    return 1;
}
