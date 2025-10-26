/*
 * SEMANTIC.H - Semantic Analyzer Header
 * CST-405 Compiler Project
 *
 * This file defines the semantic analysis phase which performs:
 *   - Type checking
 *   - Variable declaration checking
 *   - Use-before-initialization checking
 *   - Scope validation
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "symtable.h"

/* Semantic error tracking */
extern int semantic_errors;

/* SEMANTIC ANALYSIS FUNCTIONS */

/* Main semantic analysis entry point
 * Traverses the AST and performs all semantic checks
 * Returns number of errors found (0 = success) */
int analyze_semantics(ASTNode* root, SymbolTable* symtab);

/* Analyze a single statement */
void analyze_statement(ASTNode* node, SymbolTable* symtab);

/* Analyze an expression and return its type */
DataType analyze_expression(ASTNode* node, SymbolTable* symtab);

/* Check if a variable has been declared */
int check_declared(const char* var_name, SymbolTable* symtab, int line);

/* Check if a variable has been initialized before use */
int check_initialized(const char* var_name, SymbolTable* symtab, int line);

/* Report a semantic error */
void semantic_error(const char* message, int line);

/* Print semantic analysis summary */
void print_semantic_summary();

#endif /* SEMANTIC_H */
