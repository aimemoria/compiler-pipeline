%{
/* LEXICAL ANALYZER (SCANNER) - CST-405 Complete Compiler Project
 * Enhanced version with parser integration and while loop support
 * This scanner recognizes tokens for a simple programming language with:
 *   - Variable declarations (int)
 *   - Assignments
 *   - Print statements
 *   - Arithmetic expressions (+)
 *   - While loops with relational operators (NEW FEATURE)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"  /* Include parser header for token definitions */

/* Global variables for tracking location */
int line_num = 1;
int col_num = 1;
int char_count = 0;

/* Function prototypes */
void update_location();
%}

/* Flex options */
%option nounput
%option noinput

/* Pattern definitions */
DIGIT       [0-9]
LETTER      [a-zA-Z]
ID          {LETTER}({LETTER}|{DIGIT}|_)*
NUM         {DIGIT}+
WS          [ \t]

%%

/* KEYWORDS - Must be matched before identifiers */
"int"           {
                  update_location();
                  yylval.str = strdup(yytext);
                  return INT;
                }

"print"         {
                  update_location();
                  yylval.str = strdup(yytext);
                  return PRINT;
                }

"while"         {
                  /* NEW FEATURE: While loop keyword */
                  update_location();
                  yylval.str = strdup(yytext);
                  return WHILE;
                }

/* IDENTIFIERS - Variable names */
{ID}            {
                  update_location();
                  yylval.str = strdup(yytext);
                  return ID;
                }

/* INTEGER LITERALS */
{NUM}           {
                  update_location();
                  yylval.num = atoi(yytext);
                  return NUM;
                }

/* OPERATORS */
"+"             {
                  update_location();
                  yylval.str = strdup(yytext);
                  return PLUS;
                }

"="             {
                  update_location();
                  yylval.str = strdup(yytext);
                  return ASSIGN;
                }

/* RELATIONAL OPERATORS - NEW FEATURE for while loops */
"<"             {
                  update_location();
                  yylval.str = strdup(yytext);
                  return RELOP;
                }

">"             {
                  update_location();
                  yylval.str = strdup(yytext);
                  return RELOP;
                }

"<="            {
                  update_location();
                  yylval.str = strdup(yytext);
                  return RELOP;
                }

">="            {
                  update_location();
                  yylval.str = strdup(yytext);
                  return RELOP;
                }

"=="            {
                  update_location();
                  yylval.str = strdup(yytext);
                  return RELOP;
                }

"!="            {
                  update_location();
                  yylval.str = strdup(yytext);
                  return RELOP;
                }

/* PUNCTUATION */
";"             {
                  update_location();
                  yylval.str = strdup(yytext);
                  return SEMICOLON;
                }

"("             {
                  update_location();
                  yylval.str = strdup(yytext);
                  return LPAREN;
                }

")"             {
                  update_location();
                  yylval.str = strdup(yytext);
                  return RPAREN;
                }

"{"             {
                  /* NEW FEATURE: Left brace for while loop body */
                  update_location();
                  yylval.str = strdup(yytext);
                  return LBRACE;
                }

"}"             {
                  /* NEW FEATURE: Right brace for while loop body */
                  update_location();
                  yylval.str = strdup(yytext);
                  return RBRACE;
                }

/* WHITESPACE - Ignored but tracked for location */
{WS}+           {
                  col_num += yyleng;
                  char_count += yyleng;
                }

/* NEWLINES - Track for line numbering */
\n              {
                  line_num++;
                  col_num = 1;
                  char_count++;
                }

/* ERROR HANDLING - Unrecognized characters */
.               {
                  fprintf(stderr, "LEXICAL ERROR at Line %d, Col %d: Unrecognized character '%c' (ASCII %d)\n",
                          line_num, col_num, *yytext, *yytext);
                  col_num++;
                  char_count++;
                  /* Don't exit - let parser handle the error */
                }

%%

/* Update column and character counters */
void update_location() {
    col_num += yyleng;
    char_count += yyleng;
}

/* Required by flex - called at end of file */
int yywrap() {
    return 1;
}
