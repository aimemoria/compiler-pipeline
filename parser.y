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
 *   declaration -> INT ID SEMICOLON | INT ID LBRACKET NUM RBRACKET SEMICOLON
 *   assignment -> ID ASSIGN expression SEMICOLON
 *   print_stmt -> PRINT LPAREN expression RPAREN SEMICOLON
 *   while_stmt -> WHILE LPAREN condition RPAREN LBRACE statement_list RBRACE
 *   condition -> expression RELOP expression
 *   expression -> expression PLUS term | expression MINUS term | term
 *   term -> term MULT factor | term DIV factor | term MOD factor | factor
 *   factor -> ID | NUM | ID LBRACKET expression RBRACKET | LPAREN expression RPAREN
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

%code requires {
    /* Forward declaration for ASTNode (defined in ast.h) */
    typedef struct ASTNode ASTNode;
}

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
%token <str> IF          /* Keyword: if (NEW FEATURE) */
%token <str> ELSE        /* Keyword: else (NEW FEATURE) */
%token <str> ASSIGN      /* Operator: = */
%token <str> PLUS        /* Operator: + */
%token <str> MINUS       /* Operator: - */
%token <str> MULT        /* Operator: * */
%token <str> DIV         /* Operator: / */
%token <str> MOD         /* Operator: % */
%token <str> RELOP       /* Relational operators: <, >, <=, >=, ==, != (NEW FEATURE) */
%token <str> SEMICOLON   /* Punctuation: ; */
%token <str> LPAREN      /* Punctuation: ( */
%token <str> RPAREN      /* Punctuation: ) */
%token <str> LBRACE      /* Punctuation: { (NEW FEATURE) */
%token <str> RBRACE      /* Punctuation: } (NEW FEATURE) */
%token <str> LBRACKET    /* Punctuation: [ */
%token <str> RBRACKET    /* Punctuation: ] */
%token <str> COMMA       /* Punctuation: , */
%token <str> VOID        /* Keyword: void */
%token <str> RETURN      /* Keyword: return */

/* Non-terminal types - each produces an AST node */
%type <node> program
%type <node> declaration_list
%type <node> declaration_item
%type <node> statement_list
%type <node> statement
%type <node> var_declaration
%type <node> function_declaration
%type <node> function_definition
%type <node> param_list
%type <node> param_list_non_empty
%type <node> param
%type <node> assignment
%type <node> print_stmt
%type <node> while_stmt
%type <node> if_stmt
%type <node> return_stmt
%type <node> condition
%type <node> expression
%type <node> term
%type <node> factor
%type <node> function_call
%type <node> arg_list
%type <node> arg_list_non_empty

/* Operator precedence and associativity (lowest to highest) */
%left RELOP              /* Relational operators: <, >, <=, >=, ==, != */
%left PLUS MINUS         /* Addition and subtraction */
%left MULT DIV MOD       /* Multiplication, division, modulo (higher precedence) */

/* Start symbol */
%start program

%%

/* GRAMMAR RULES WITH SEMANTIC ACTIONS */

/* Program: A list of declarations (variables and functions) */
program:
    declaration_list
    {
        $$ = create_program_node($1);
        ast_root = $$;  /* Store root for later processing */
        printf("[PARSER] Program parsed successfully\n");
    }
    ;

/* Declaration list: One or more declarations */
declaration_list:
    declaration_item
    {
        $$ = $1;
    }
    | declaration_item declaration_list
    {
        $$ = create_statement_list_node($1, $2);
    }
    ;

/* Declaration item: Can be a variable declaration or function declaration/definition */
declaration_item:
    var_declaration
    {
        $$ = $1;
    }
    | function_declaration
    {
        $$ = $1;
    }
    | function_definition
    {
        $$ = $1;
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

/* Statement: Can be a declaration, assignment, print, while loop, if statement, or return */
statement:
    var_declaration
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
    | if_stmt
    {
        $$ = $1;
    }
    | return_stmt
    {
        $$ = $1;
    }
    ;

/* Variable declaration: int identifier; OR int arr[size]; */
var_declaration:
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
    | INT ID LBRACKET NUM RBRACKET SEMICOLON
    {
        $$ = create_array_declaration_node($2, $4);
        printf("[PARSER] Array Declaration: int %s[%d];\n", $2, $4);

        /* Add array to symbol table during parsing for early error detection */
        if (global_symtab && !add_array_symbol(global_symtab, $2, TYPE_INT, $4, line_num)) {
            fprintf(stderr, "Semantic Error at line %d: Array '%s' already declared\n",
                    line_num, $2);
        }
    }
    ;

/* Assignment statement: identifier = expression; OR arr[index] = expression; (ARRAY FEATURE) */
assignment:
    ID ASSIGN expression SEMICOLON
    {
        $$ = create_assignment_node($1, $3);
        printf("[PARSER] Assignment: %s = <expression>;\n", $1);
    }
    | ID LBRACKET expression RBRACKET ASSIGN expression SEMICOLON
    {
        ASTNode* array_access = create_array_access_node($1, $3);
        $$ = create_assignment_node($1, $6);
        /* Replace the identifier with array access in assignment */
        free($$->data.assignment.var_name);
        $$->data.assignment.var_name = strdup($1);
        printf("[PARSER] Array Assignment: %s[<index>] = <expression>;\n", $1);
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

/* While loop: while (condition) { statement_list } */
while_stmt:
    WHILE LPAREN condition RPAREN LBRACE statement_list RBRACE
    {
        $$ = create_while_node($3, $6);
        printf("[PARSER] While loop: while (<condition>) { <statements> }\n");
    }
    ;

/* If statement: if (condition) { statement_list } [else { statement_list }] */
if_stmt:
    IF LPAREN condition RPAREN LBRACE statement_list RBRACE
    {
        $$ = create_if_node($3, $6, NULL);
        printf("[PARSER] If statement: if (<condition>) { <statements> }\n");
    }
    | IF LPAREN condition RPAREN LBRACE statement_list RBRACE ELSE LBRACE statement_list RBRACE
    {
        $$ = create_if_node($3, $6, $10);
        printf("[PARSER] If-else statement: if (<condition>) { <statements> } else { <statements> }\n");
    }
    ;

/* Return statement: return expression; */
return_stmt:
    RETURN expression SEMICOLON
    {
        $$ = create_return_node($2);
        printf("[PARSER] Return statement: return <expression>;\n");
    }
    ;

/* Function declaration: int foo(params); or void foo(params); */
function_declaration:
    INT ID LPAREN param_list RPAREN SEMICOLON
    {
        $$ = create_function_decl_node("int", $2, $4);
        printf("[PARSER] Function declaration: int %s(...);\n", $2);
    }
    | VOID ID LPAREN param_list RPAREN SEMICOLON
    {
        $$ = create_function_decl_node("void", $2, $4);
        printf("[PARSER] Function declaration: void %s(...);\n", $2);
    }
    ;

/* Function definition: int foo(params) { body } or void foo(params) { body } */
function_definition:
    INT ID LPAREN param_list RPAREN LBRACE statement_list RBRACE
    {
        $$ = create_function_def_node("int", $2, $4, $7);
        printf("[PARSER] Function definition: int %s(...) { ... }\n", $2);
    }
    | VOID ID LPAREN param_list RPAREN LBRACE statement_list RBRACE
    {
        $$ = create_function_def_node("void", $2, $4, $7);
        printf("[PARSER] Function definition: void %s(...) { ... }\n", $2);
    }
    ;

/* Parameter list: Can be empty or have parameters */
param_list:
    /* empty */
    {
        $$ = NULL;
    }
    | param_list_non_empty
    {
        $$ = $1;
    }
    ;

/* Non-empty parameter list */
param_list_non_empty:
    param
    {
        $$ = create_param_list_node($1, NULL);
    }
    | param COMMA param_list_non_empty
    {
        $$ = create_param_list_node($1, $3);
    }
    ;

/* Parameter: int x */
param:
    INT ID
    {
        $$ = create_param_node("int", $2);
        printf("[PARSER] Parameter: int %s\n", $2);
    }
    ;

/* Condition: expression relop expression (for while loops) */
condition:
    expression RELOP expression
    {
        $$ = create_condition_node($1, $2, $3);
        printf("[PARSER] Condition: <expr> %s <expr>\n", $2);
    }
    ;

/* Expression: Handles addition and subtraction (lowest precedence) */
expression:
    expression PLUS term
    {
        $$ = create_binary_op_node("+", $1, $3);
        printf("[PARSER] Binary operation: <expr> + <term>\n");
    }
    | expression MINUS term
    {
        $$ = create_binary_op_node("-", $1, $3);
        printf("[PARSER] Binary operation: <expr> - <term>\n");
    }
    | term
    {
        $$ = $1;
    }
    ;

/* Term: Handles multiplication, division, and modulo (higher precedence) */
term:
    term MULT factor
    {
        $$ = create_binary_op_node("*", $1, $3);
        printf("[PARSER] Binary operation: <term> * <factor>\n");
    }
    | term DIV factor
    {
        $$ = create_binary_op_node("/", $1, $3);
        printf("[PARSER] Binary operation: <term> / <factor>\n");
    }
    | term MOD factor
    {
        $$ = create_binary_op_node("%%", $1, $3);
        printf("[PARSER] Binary operation: <term> %% <factor>\n");
    }
    | factor
    {
        $$ = $1;
    }
    ;

/* Factor: Basic building blocks - identifiers, numbers, array access, function calls, and parenthesized expressions */
factor:
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
    | ID LBRACKET expression RBRACKET
    {
        $$ = create_array_access_node($1, $3);
        printf("[PARSER] Array Access: %s[<index>]\n", $1);
    }
    | function_call
    {
        $$ = $1;
    }
    | LPAREN expression RPAREN
    {
        $$ = $2;
        printf("[PARSER] Parenthesized expression\n");
    }
    ;

/* Function call: foo(args) */
function_call:
    ID LPAREN arg_list RPAREN
    {
        $$ = create_function_call_node($1, $3);
        printf("[PARSER] Function call: %s(...)\n", $1);
    }
    ;

/* Argument list: Can be empty or have arguments */
arg_list:
    /* empty */
    {
        $$ = NULL;
    }
    | arg_list_non_empty
    {
        $$ = $1;
    }
    ;

/* Non-empty argument list */
arg_list_non_empty:
    expression
    {
        $$ = create_arg_list_node($1, NULL);
    }
    | expression COMMA arg_list_non_empty
    {
        $$ = create_arg_list_node($1, $3);
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
