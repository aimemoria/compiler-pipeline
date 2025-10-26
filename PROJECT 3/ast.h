/*
 * AST.H - Abstract Syntax Tree Header
 * CST-405 Compiler Project
 *
 * This file defines the Abstract Syntax Tree structure used to represent
 * the parsed program in memory. Each node represents a language construct.
 */

#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* AST Node Types - Each represents a different language construct */
typedef enum {
    NODE_PROGRAM,           /* Root node of the entire program */
    NODE_STATEMENT_LIST,    /* List of statements */
    NODE_DECLARATION,       /* Variable declaration: int x; */
    NODE_ASSIGNMENT,        /* Assignment: x = expr; */
    NODE_PRINT,            /* Print statement: print(expr); */
    NODE_WHILE,            /* While loop: while (cond) { stmts } */
    NODE_CONDITION,        /* Condition for while: expr relop expr */
    NODE_BINARY_OP,        /* Binary operation: expr + expr */
    NODE_IDENTIFIER,       /* Identifier (variable name) */
    NODE_NUMBER,           /* Integer literal */
    NODE_ARRAY_DECLARATION, /* Array declaration: int arr[10]; */
    NODE_ARRAY_ACCESS,     /* Array access: arr[5] */
    NODE_FUNCTION_DECL,    /* Function declaration: int foo(int a); */
    NODE_FUNCTION_DEF,     /* Function definition: int foo(int a) { ... } */
    NODE_FUNCTION_CALL,    /* Function call: foo(x, y) */
    NODE_RETURN,           /* Return statement: return expr; */
    NODE_PARAM,            /* Function parameter: int a */
    NODE_PARAM_LIST,       /* List of parameters */
    NODE_ARG_LIST          /* List of arguments in function call */
} NodeType;

/* Forward declaration */
struct ASTNode;

/* AST Node Structure - Represents one node in the syntax tree */
typedef struct ASTNode {
    NodeType type;                /* Type of this node */

    /* Node-specific data (union to save memory) */
    union {
        /* For numbers */
        int num_value;

        /* For identifiers and operators */
        char* str_value;

        /* For binary operations and conditions */
        struct {
            char* operator;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary_op;

        /* For assignments */
        struct {
            char* var_name;
            struct ASTNode* expr;
        } assignment;

        /* For print statements */
        struct {
            struct ASTNode* expr;
        } print;

        /* For while loops (NEW FEATURE) */
        struct {
            struct ASTNode* condition;
            struct ASTNode* body;
        } while_loop;

        /* For statement lists */
        struct {
            struct ASTNode* statement;
            struct ASTNode* next;
        } stmt_list;

        /* For program node */
        struct {
            struct ASTNode* statements;
        } program;

        /* For array declarations (ARRAY FEATURE) */
        struct {
            char* var_name;
            int size;
        } array_decl;

        /* For array access */
        struct {
            char* array_name;
            struct ASTNode* index;
        } array_access;

        /* For function declaration/definition */
        struct {
            char* return_type;
            char* func_name;
            struct ASTNode* params;
            struct ASTNode* body;  /* NULL for declarations */
        } function;

        /* For function call */
        struct {
            char* func_name;
            struct ASTNode* args;
        } func_call;

        /* For return statement */
        struct {
            struct ASTNode* expr;
        } return_stmt;

        /* For parameter/argument lists */
        struct {
            struct ASTNode* item;
            struct ASTNode* next;
        } list;

        /* For parameter */
        struct {
            char* type;
            char* name;
        } param;

    } data;

    /* Source location for error reporting */
    int line_number;

} ASTNode;

/* AST CONSTRUCTION FUNCTIONS - Create nodes for different language constructs */

/* Create a program node (root of AST) */
ASTNode* create_program_node(ASTNode* statements);

/* Create a statement list node (chains statements together) */
ASTNode* create_statement_list_node(ASTNode* stmt, ASTNode* next);

/* Create a declaration node: int x; */
ASTNode* create_declaration_node(char* var_name);

/* Create an assignment node: x = expr; */
ASTNode* create_assignment_node(char* var_name, ASTNode* expr);

/* Create a print node: print(expr); */
ASTNode* create_print_node(ASTNode* expr);

/* Create a while loop node: while (cond) { body } (NEW FEATURE) */
ASTNode* create_while_node(ASTNode* condition, ASTNode* body);

/* Create a condition node: expr relop expr (NEW FEATURE) */
ASTNode* create_condition_node(ASTNode* left, char* op, ASTNode* right);

/* Create a binary operation node: left op right */
ASTNode* create_binary_op_node(char* op, ASTNode* left, ASTNode* right);

/* Create an identifier node */
ASTNode* create_id_node(char* name);

/* Create a number node */
ASTNode* create_num_node(int value);

/* Create an array declaration node: int arr[10]; (ARRAY FEATURE) */
ASTNode* create_array_declaration_node(char* var_name, int size);

/* Create an array access node: arr[5] */
ASTNode* create_array_access_node(char* array_name, ASTNode* index);

/* Create a function declaration node: int foo(params); */
ASTNode* create_function_decl_node(char* return_type, char* func_name, ASTNode* params);

/* Create a function definition node: int foo(params) { body } */
ASTNode* create_function_def_node(char* return_type, char* func_name, ASTNode* params, ASTNode* body);

/* Create a function call node: foo(args) */
ASTNode* create_function_call_node(char* func_name, ASTNode* args);

/* Create a return statement node: return expr; */
ASTNode* create_return_node(ASTNode* expr);

/* Create a parameter node: int x */
ASTNode* create_param_node(char* type, char* name);

/* Create a parameter list node */
ASTNode* create_param_list_node(ASTNode* param, ASTNode* next);

/* Create an argument list node */
ASTNode* create_arg_list_node(ASTNode* arg, ASTNode* next);

/* AST UTILITY FUNCTIONS */

/* Print the AST in a tree format (for debugging and visualization) */
void print_ast(ASTNode* node, int indent_level);

/* Free all memory used by the AST */
void free_ast(ASTNode* node);

/* Get a string representation of a node type (for debugging) */
const char* node_type_to_string(NodeType type);

#endif /* AST_H */
