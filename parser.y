%{
/* PARSER (SYNTAX ANALYZER) - CST-405 Compiler Project
 * This parser implements syntax analysis for a simple programming language
 * with variable declarations, assignments, print statements, expressions,
 * and while loops (new feature).
 *
 * Grammar:
 *   program -> statement_list
 *   statement_list -> statement statement_list | statement
 *   statement -> declaration | assignment | print_stmt | while_stmt
 *   declaration -> INT ID SEMICOLON
 *   assignment -> ID ASSIGN expression SEMICOLON
 *   print_stmt -> PRINT LPAREN expression RPAREN SEMICOLON
 *   while_stmt -> WHILE LPAREN condition RPAREN LBRACE statement_list RBRACE
 *   condition -> expression RELOP expression
 *   expression -> expression PLUS term | term
 *   term -> ID | NUM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symtable.h"

/* External references from lexer */
extern int yylex();
extern int yyparse();
extern FILE* yyin;
extern int line_num;
extern int col_num;

/* Error handling */
void yyerror(const char* s);

/* Global AST root */
ASTNode* ast_root = NULL;

/* Global symbol table */
SymbolTable* global_symtab = NULL;

/* Statistics */
int syntax_errors = 0;

%}

/* Union for semantic values - stores different types of data for tokens/non-terminals */
%union {
    int num;           /* For integer literals */
    char* str;         /* For identifiers and operators */
    ASTNode* node;     /* For AST nodes */
}

/* Token declarations with semantic types */
%token <str> ID          /* Identifier */
%token <num> NUM         /* Integer literal */
%token <str> INT         /* Keyword: int */
%token <str> PRINT       /* Keyword: print */
%token <str> WHILE       /* Keyword: while (NEW FEATURE) */
%token <str> ASSIGN      /* Operator: = */
%token <str> PLUS        /* Operator: + */
%token <str> RELOP       /* Relational operators: <, >, <=, >=, ==, != (NEW FEATURE) */
%token <str> SEMICOLON   /* Punctuation: ; */
%token <str> LPAREN      /* Punctuation: ( */
%token <str> RPAREN      /* Punctuation: ) */
%token <str> LBRACE      /* Punctuation: { (NEW FEATURE) */
%token <str> RBRACE      /* Punctuation: } (NEW FEATURE) */

/* Non-terminal types - each produces an AST node */
%type <node> program
%type <node> statement_list
%type <node> statement
%type <node> declaration
%type <node> assignment
%type <node> print_stmt
%type <node> while_stmt
%type <node> condition
%type <node> expression
%type <node> term

/* Operator precedence and associativity */
%left PLUS               /* Left-associative addition */
%left RELOP              /* Left-associative relational operators */

/* Start symbol */
%start program

%%

/* GRAMMAR RULES WITH SEMANTIC ACTIONS */

/* Program: A list of statements (the root of the AST) */
program:
    statement_list
    {
        $$ = create_program_node($1);
        ast_root = $$;  /* Store root for later processing */
        printf("[PARSER] Program parsed successfully\n");
    }
    ;

/* Statement list: One or more statements */
statement_list:
    statement
    {
        $$ = $1;
    }
    | statement statement_list
    {
        $$ = create_statement_list_node($1, $2);
    }
    ;

/* Statement: Can be a declaration, assignment, print, or while loop */
statement:
    declaration
    {
        $$ = $1;
    }
    | assignment
    {
        $$ = $1;
    }
    | print_stmt
    {
        $$ = $1;
    }
    | while_stmt
    {
        $$ = $1;
    }
    ;

/* Variable declaration: int identifier; */
declaration:
    INT ID SEMICOLON
    {
        $$ = create_declaration_node($2);
        printf("[PARSER] Declaration: int %s;\n", $2);

        /* Add to symbol table during parsing for early error detection */
        if (global_symtab && !add_symbol(global_symtab, $2, TYPE_INT, line_num)) {
            fprintf(stderr, "Semantic Error at line %d: Variable '%s' already declared\n",
                    line_num, $2);
        }
    }
    ;

/* Assignment statement: identifier = expression; */
assignment:
    ID ASSIGN expression SEMICOLON
    {
        $$ = create_assignment_node($1, $3);
        printf("[PARSER] Assignment: %s = <expression>;\n", $1);
    }
    ;

/* Print statement: print(expression); */
print_stmt:
    PRINT LPAREN expression RPAREN SEMICOLON
    {
        $$ = create_print_node($3);
        printf("[PARSER] Print statement: print(<expression>);\n");
    }
    ;

/* While loop: while (condition) { statement_list } (NEW FEATURE) */
while_stmt:
    WHILE LPAREN condition RPAREN LBRACE statement_list RBRACE
    {
        $$ = create_while_node($3, $6);
        printf("[PARSER] While loop: while (<condition>) { <statements> }\n");
    }
    ;

/* Condition: expression relop expression (for while loops) (NEW FEATURE) */
condition:
    expression RELOP expression
    {
        $$ = create_condition_node($1, $2, $3);
        printf("[PARSER] Condition: <expr> %s <expr>\n", $2);
    }
    ;

/* Expression: Handles addition (left-recursive for left-associativity) */
expression:
    expression PLUS term
    {
        $$ = create_binary_op_node("+", $1, $3);
        printf("[PARSER] Binary operation: <expr> + <term>\n");
    }
    | term
    {
        $$ = $1;
    }
    ;

/* Term: Basic building blocks - identifiers and numbers */
term:
    ID
    {
        $$ = create_id_node($1);
        printf("[PARSER] Identifier: %s\n", $1);
    }
    | NUM
    {
        $$ = create_num_node($1);
        printf("[PARSER] Number: %d\n", $1);
    }
    ;

%%

/* ERROR HANDLING */

/* Called when a syntax error is detected */
void yyerror(const char* s) {
    fprintf(stderr, "\n╔═══════════════════════════════════════════════════════════╗\n");
    fprintf(stderr, "║ SYNTAX ERROR                                              ║\n");
    fprintf(stderr, "╠═══════════════════════════════════════════════════════════╣\n");
    fprintf(stderr, "║ Location: Line %d, Column %d                              ║\n", line_num, col_num);
    fprintf(stderr, "║ Message:  %s                                              ║\n", s);
    fprintf(stderr, "╚═══════════════════════════════════════════════════════════╝\n\n");
    syntax_errors++;
}

/* MAIN FUNCTION - Entry point for the parser */

int main(int argc, char* argv[]) {
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║          CST-405 Complete Compiler System                ║\n");
    printf("║          Phase 2: Syntax Analysis (Parser)               ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");

    /* Initialize symbol table */
    global_symtab = create_symbol_table(100);

    /* Open input file if provided */
    if (argc > 1) {
        FILE* input_file = fopen(argv[1], "r");
        if (!input_file) {
            fprintf(stderr, "Error: Cannot open file '%s'\n", argv[1]);
            return 1;
        }
        yyin = input_file;
        printf("📄 Processing file: %s\n\n", argv[1]);
    } else {
        printf("📄 Reading from standard input (Ctrl+D to end)\n\n");
    }

    printf("═══════════════════ PARSING STARTED ═══════════════════\n\n");

    /* Run the parser */
    int parse_result = yyparse();

    printf("\n═══════════════════ PARSING COMPLETE ═══════════════════\n\n");

    /* Print results */
    if (parse_result == 0 && syntax_errors == 0) {
        printf("✓ SUCCESS: No syntax errors detected\n");
        printf("✓ Abstract Syntax Tree (AST) built successfully\n");
        printf("✓ Symbol table populated\n\n");

        /* Print AST */
        printf("═══════════════ ABSTRACT SYNTAX TREE ══════════════════\n\n");
        print_ast(ast_root, 0);
        printf("\n");

        /* Print symbol table */
        printf("═══════════════════ SYMBOL TABLE ══════════════════════\n\n");
        print_symbol_table(global_symtab);
        printf("\n");

    } else {
        printf("✗ FAILURE: %d syntax error(s) found\n", syntax_errors > 0 ? syntax_errors : 1);
        printf("✗ Please fix the errors and try again\n\n");
    }

    /* Cleanup */
    if (argc > 1) {
        fclose(yyin);
    }

    /* Free AST and symbol table */
    if (ast_root) {
        free_ast(ast_root);
    }
    if (global_symtab) {
        free_symbol_table(global_symtab);
    }

    return (parse_result == 0 && syntax_errors == 0) ? 0 : 1;
}
