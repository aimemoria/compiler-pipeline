%{
/* LEXICAL ANALYZER (SCANNER) - CST-301/302 Project
 * Enhanced version with error tracking and location reporting
 * This scanner recognizes tokens for a simple programming language
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Global variables for tracking */
int line_num = 1;
int col_num = 1;
int token_count = 0;
int error_count = 0;
int char_count = 0;

/* Function prototypes */
void print_token(const char* token_type, const char* lexeme);
void print_error(const char* message, char bad_char);
void update_location();
void print_statistics();
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

"int"           { 
                  print_token("KEYWORD", yytext);
                  update_location();
                  /* return INT; */
                }

"print"         { 
                  print_token("KEYWORD", yytext);
                  update_location();
                  /* return PRINT; */
                }

{ID}            { 
                  print_token("IDENTIFIER", yytext);
                  update_location();
                  /* yylval.str = strdup(yytext); */
                  /* return ID; */
                }

{NUM}           { 
                  print_token("INTEGER", yytext);
                  update_location();
                  /* yylval.num = atoi(yytext); */
                  /* return NUM; */
                }

"+"             { 
                  print_token("OPERATOR", yytext);
                  update_location();
                  /* return '+'; */
                }

"="             { 
                  print_token("OPERATOR", yytext);
                  update_location();
                  /* return '='; */
                }

";"             { 
                  print_token("PUNCTUATION", yytext);
                  update_location();
                  /* return ';'; */
                }

"("             { 
                  print_token("PUNCTUATION", yytext);
                  update_location();
                  /* return '('; */
                }

")"             { 
                  print_token("PUNCTUATION", yytext);
                  update_location();
                  /* return ')'; */
                }

{WS}+           { 
                  col_num += yyleng;
                  char_count += yyleng;
                }

\n              { 
                  line_num++;
                  col_num = 1;
                  char_count++;
                }

.               { 
                  print_error("Unrecognized character", *yytext);
                  col_num++;
                  char_count++;
                  error_count++;
                }

%%

/* Support functions */
void print_token(const char* token_type, const char* lexeme) {
    printf("Line %d, Col %d: TOKEN=%-12s LEXEME='%s'\n", 
           line_num, col_num, token_type, lexeme);
    token_count++;
}

void print_error(const char* message, char bad_char) {
    fprintf(stderr, "LEXICAL ERROR at Line %d, Col %d: %s '%c' (ASCII %d)\n", 
            line_num, col_num, message, bad_char, bad_char);
}

void update_location() {
    col_num += yyleng;
    char_count += yyleng;
}

void print_statistics() {
    printf("\n==================================================\n");
    printf("LEXICAL ANALYSIS COMPLETE\n");
    printf("==================================================\n");
    printf("Statistics:\n");
    printf("  Total characters processed: %d\n", char_count);
    printf("  Total lines processed: %d\n", line_num);
    printf("  Tokens successfully recognized: %d\n", token_count);
    printf("  Lexical errors found: %d\n", error_count);
    
    if (error_count == 0) {
        printf("\n✓ CERTIFICATION: Source code is lexically CORRECT\n");
        printf("  All characters have been successfully tokenized.\n");
        printf("  No lexical errors detected.\n");
        printf("  Ready for syntax analysis phase.\n");
    } else {
        printf("\n✗ CERTIFICATION: Source code contains lexical ERRORS\n");
        printf("  %d lexical error(s) must be fixed before proceeding.\n", error_count);
        printf("  Please correct the errors and re-run the lexical analyzer.\n");
    }
    printf("==================================================\n");
}

int yywrap() {
    print_statistics();
    return 1;
}

int main(int argc, char* argv[]) {
    printf("CST-301/302 Lexical Analyzer\n");
    printf("Enhanced Version with Error Tracking\n");
    printf("==================================================\n");
    
    if (argc > 1) {
        FILE* input_file = fopen(argv[1], "r");
        if (!input_file) {
            fprintf(stderr, "Error: Cannot open file '%s'\n", argv[1]);
            return 1;
        }
        yyin = input_file;
        printf("Processing file: %s\n", argv[1]);
    } else {
        printf("Reading from standard input (Ctrl+D to end):\n");
    }
    
    printf("==================================================\n");
    
    yylex();
    
    if (argc > 1) {
        fclose(yyin);
    }
    
    return (error_count == 0) ? 0 : 1;
}
