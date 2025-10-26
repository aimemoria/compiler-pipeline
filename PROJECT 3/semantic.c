/*
 * SEMANTIC.C - Semantic Analyzer Implementation
 * CST-405 Compiler Project
 *
 * This file implements semantic analysis including type checking,
 * variable declaration checking, and use-before-initialization detection.
 */

#include "semantic.h"

/* Global error counter */
int semantic_errors = 0;

/* Report a semantic error with location information */
void semantic_error(const char* message, int line) {
    fprintf(stderr, "\n╔═══════════════════════════════════════════════════════════╗\n");
    fprintf(stderr, "║ SEMANTIC ERROR                                            ║\n");
    fprintf(stderr, "╠═══════════════════════════════════════════════════════════╣\n");
    fprintf(stderr, "║ Line %d: %-48s ║\n", line, message);
    fprintf(stderr, "╚═══════════════════════════════════════════════════════════╝\n\n");
    semantic_errors++;
}

/* Check if a variable has been declared */
int check_declared(const char* var_name, SymbolTable* symtab, int line) {
    Symbol* symbol = lookup_symbol(symtab, var_name);
    if (!symbol) {
        char error_msg[100];
        snprintf(error_msg, sizeof(error_msg),
                 "Variable '%s' used before declaration", var_name);
        semantic_error(error_msg, line);
        return 0;
    }
    return 1;
}

/* Check if a variable has been initialized before use */
int check_initialized(const char* var_name, SymbolTable* symtab, int line) {
    Symbol* symbol = lookup_symbol(symtab, var_name);
    if (symbol && !symbol->is_initialized) {
        char error_msg[100];
        snprintf(error_msg, sizeof(error_msg),
                 "Variable '%s' used before initialization", var_name);
        semantic_error(error_msg, line);
        return 0;
    }
    return 1;
}

/* Analyze an expression and return its type */
DataType analyze_expression(ASTNode* node, SymbolTable* symtab) {
    if (!node) return TYPE_UNKNOWN;

    switch (node->type) {
        case NODE_NUMBER:
            /* Integer literal - always type int */
            return TYPE_INT;

        case NODE_IDENTIFIER: {
            /* Variable reference - check if declared and initialized */
            const char* var_name = node->data.str_value;

            if (!check_declared(var_name, symtab, node->line_number)) {
                return TYPE_UNKNOWN;
            }

            if (!check_initialized(var_name, symtab, node->line_number)) {
                return TYPE_UNKNOWN;
            }

            /* Return the variable's type from symbol table */
            Symbol* symbol = lookup_symbol(symtab, var_name);
            return symbol ? symbol->type : TYPE_UNKNOWN;
        }

        case NODE_BINARY_OP: {
            /* Binary operation - both operands must be type int */
            DataType left_type = analyze_expression(node->data.binary_op.left, symtab);
            DataType right_type = analyze_expression(node->data.binary_op.right, symtab);

            /* Type checking for addition */
            if (left_type == TYPE_INT && right_type == TYPE_INT) {
                return TYPE_INT;
            } else if (left_type != TYPE_UNKNOWN && right_type != TYPE_UNKNOWN) {
                semantic_error("Type mismatch in binary operation", node->line_number);
            }
            return TYPE_UNKNOWN;
        }

        case NODE_CONDITION: {
            /* Condition in while loop - both sides must be type int */
            DataType left_type = analyze_expression(node->data.binary_op.left, symtab);
            DataType right_type = analyze_expression(node->data.binary_op.right, symtab);

            if (left_type == TYPE_INT && right_type == TYPE_INT) {
                return TYPE_INT;  /* Condition evaluates to int (0 or 1) */
            } else if (left_type != TYPE_UNKNOWN && right_type != TYPE_UNKNOWN) {
                semantic_error("Type mismatch in condition", node->line_number);
            }
            return TYPE_UNKNOWN;
        }

        case NODE_ARRAY_ACCESS: {
            /* Array access: arr[index] */
            const char* array_name = node->data.array_access.array_name;

            /* Check if array is declared */
            Symbol* symbol = lookup_symbol(symtab, array_name);
            if (!symbol) {
                char error_msg[100];
                snprintf(error_msg, sizeof(error_msg),
                         "Array '%s' used before declaration", array_name);
                semantic_error(error_msg, node->line_number);
                return TYPE_UNKNOWN;
            }

            /* Check if it's actually an array */
            if (!symbol->is_array) {
                char error_msg[100];
                snprintf(error_msg, sizeof(error_msg),
                         "'%s' is not an array", array_name);
                semantic_error(error_msg, node->line_number);
                return TYPE_UNKNOWN;
            }

            /* Check that index is an integer expression */
            DataType index_type = analyze_expression(node->data.array_access.index, symtab);
            if (index_type != TYPE_INT && index_type != TYPE_UNKNOWN) {
                semantic_error("Array index must be an integer", node->line_number);
                return TYPE_UNKNOWN;
            }

            /* Array access returns the element type */
            return symbol->type;
        }

        case NODE_FUNCTION_CALL: {
            /* Function call: func(arg1, arg2, ...) */
            const char* func_name = node->data.func_call.func_name;

            /* Check if function is declared */
            Symbol* symbol = lookup_symbol(symtab, func_name);
            if (!symbol) {
                char error_msg[100];
                snprintf(error_msg, sizeof(error_msg),
                         "Function '%s' called before declaration", func_name);
                semantic_error(error_msg, node->line_number);
                return TYPE_UNKNOWN;
            }

            /* Check if it's actually a function */
            if (symbol->kind != SYMBOL_FUNCTION) {
                char error_msg[100];
                snprintf(error_msg, sizeof(error_msg),
                         "'%s' is not a function", func_name);
                semantic_error(error_msg, node->line_number);
                return TYPE_UNKNOWN;
            }

            /* Count arguments and validate types */
            int arg_count = 0;
            ASTNode* current_arg = node->data.func_call.args;

            while (current_arg) {
                DataType arg_type;

                if (current_arg->type == NODE_ARG_LIST) {
                    arg_type = analyze_expression(current_arg->data.arg_list.arg, symtab);
                    arg_count++;

                    /* Check parameter type if available */
                    if (arg_count <= symbol->param_count) {
                        DataType expected_type = symbol->param_types[arg_count - 1];
                        if (arg_type != TYPE_UNKNOWN && expected_type != TYPE_UNKNOWN &&
                            arg_type != expected_type) {
                            char error_msg[100];
                            snprintf(error_msg, sizeof(error_msg),
                                     "Argument %d type mismatch in call to '%s'",
                                     arg_count, func_name);
                            semantic_error(error_msg, node->line_number);
                        }
                    }

                    current_arg = current_arg->data.arg_list.next;
                } else {
                    arg_type = analyze_expression(current_arg, symtab);
                    arg_count++;

                    /* Check parameter type if available */
                    if (arg_count <= symbol->param_count) {
                        DataType expected_type = symbol->param_types[arg_count - 1];
                        if (arg_type != TYPE_UNKNOWN && expected_type != TYPE_UNKNOWN &&
                            arg_type != expected_type) {
                            char error_msg[100];
                            snprintf(error_msg, sizeof(error_msg),
                                     "Argument %d type mismatch in call to '%s'",
                                     arg_count, func_name);
                            semantic_error(error_msg, node->line_number);
                        }
                    }

                    break;
                }
            }

            /* Check argument count */
            if (arg_count != symbol->param_count) {
                char error_msg[100];
                snprintf(error_msg, sizeof(error_msg),
                         "Function '%s' expects %d arguments, got %d",
                         func_name, symbol->param_count, arg_count);
                semantic_error(error_msg, node->line_number);
            }

            /* Return the function's return type */
            return symbol->return_type;
        }

        default:
            return TYPE_UNKNOWN;
    }
}

/* Analyze a single statement */
void analyze_statement(ASTNode* node, SymbolTable* symtab) {
    if (!node) return;

    switch (node->type) {
        case NODE_DECLARATION: {
            /* Declaration is already handled in the parser
             * (added to symbol table during parsing) */
            printf("[SEMANTIC] Declaration verified: int %s\n",
                   node->data.str_value);
            break;
        }

        case NODE_ASSIGNMENT: {
            /* Assignment: variable = expression */
            const char* var_name = node->data.assignment.var_name;

            /* Check if variable was declared */
            if (!check_declared(var_name, symtab, node->line_number)) {
                break;
            }

            /* Analyze the expression on the right side */
            DataType expr_type = analyze_expression(node->data.assignment.expr, symtab);

            /* Check type compatibility */
            Symbol* symbol = lookup_symbol(symtab, var_name);
            if (symbol && expr_type != TYPE_UNKNOWN && symbol->type != expr_type) {
                semantic_error("Type mismatch in assignment", node->line_number);
            }

            /* Mark variable as initialized */
            mark_initialized(symtab, var_name);

            printf("[SEMANTIC] Assignment verified: %s = <expr>\n", var_name);
            break;
        }

        case NODE_PRINT: {
            /* Print statement: print(expression) */
            DataType expr_type = analyze_expression(node->data.print.expr, symtab);

            if (expr_type == TYPE_UNKNOWN) {
                /* Error already reported by analyze_expression */
            }

            printf("[SEMANTIC] Print statement verified\n");
            break;
        }

        case NODE_WHILE: {
            /* While loop: while (condition) { body } */
            printf("[SEMANTIC] Analyzing while loop...\n");

            /* Analyze the condition */
            DataType cond_type = analyze_expression(node->data.while_loop.condition, symtab);

            if (cond_type == TYPE_UNKNOWN) {
                /* Error already reported */
            }

            /* Analyze the body (which may be a statement list) */
            analyze_statement(node->data.while_loop.body, symtab);

            printf("[SEMANTIC] While loop verified\n");
            break;
        }

        case NODE_STATEMENT_LIST: {
            /* Statement list: recursively analyze each statement */
            analyze_statement(node->data.stmt_list.statement, symtab);
            analyze_statement(node->data.stmt_list.next, symtab);
            break;
        }

        case NODE_FUNCTION_DEF: {
            /* Function definition: type name(params) { body } */
            const char* func_name = node->data.function.func_name;

            printf("[SEMANTIC] Analyzing function '%s'...\n", func_name);

            /* Function should already be in symbol table (added during parsing) */
            Symbol* symbol = lookup_symbol(symtab, func_name);
            if (!symbol) {
                char error_msg[100];
                snprintf(error_msg, sizeof(error_msg),
                         "Function '%s' not found in symbol table", func_name);
                semantic_error(error_msg, node->line_number);
                break;
            }

            /* Analyze function body */
            analyze_statement(node->data.function.body, symtab);

            printf("[SEMANTIC] Function '%s' verified\n", func_name);
            break;
        }

        case NODE_RETURN: {
            /* Return statement: return expr; or return; */
            printf("[SEMANTIC] Return statement verified\n");

            /* Analyze return expression if present */
            if (node->data.return_stmt.expr) {
                DataType return_type = analyze_expression(node->data.return_stmt.expr, symtab);

                if (return_type == TYPE_UNKNOWN) {
                    /* Error already reported */
                }
            }

            break;
        }

        case NODE_FUNCTION_CALL: {
            /* Function call as a statement (not used in expression) */
            analyze_expression(node, symtab);
            printf("[SEMANTIC] Function call statement verified\n");
            break;
        }

        default:
            break;
    }
}

/* Main semantic analysis function */
int analyze_semantics(ASTNode* root, SymbolTable* symtab) {
    printf("\n═══════════════ SEMANTIC ANALYSIS STARTED ═══════════════\n\n");

    semantic_errors = 0;

    if (!root) {
        semantic_error("Empty program", 0);
        return semantic_errors;
    }

    /* Start analyzing from the program root */
    if (root->type == NODE_PROGRAM) {
        analyze_statement(root->data.program.statements, symtab);
    }

    printf("\n═══════════════ SEMANTIC ANALYSIS COMPLETE ══════════════\n\n");

    return semantic_errors;
}

/* Print semantic analysis summary */
void print_semantic_summary() {
    if (semantic_errors == 0) {
        printf("✓ SUCCESS: No semantic errors detected\n");
        printf("✓ All variables properly declared and initialized\n");
        printf("✓ All type checks passed\n\n");
    } else {
        printf("✗ FAILURE: %d semantic error(s) found\n", semantic_errors);
        printf("✗ Please fix the errors before proceeding\n\n");
    }
}
