/*
 * IRCODE.C - Intermediate Representation Code Implementation
 * CST-405 Compiler Project
 *
 * This file implements the Three-Address Code (TAC) generator.
 * TAC is an intermediate representation between the high-level source
 * and low-level assembly code.
 */

#include "ircode.h"

/* Global counters for generating unique names */
int temp_count = 0;
int label_count = 0;

/* Create a new empty TAC code list */
TACCode* create_tac_code() {
    TACCode* code = (TACCode*)malloc(sizeof(TACCode));
    if (!code) {
        fprintf(stderr, "Fatal Error: Failed to allocate TAC code\n");
        exit(1);
    }
    code->head = NULL;
    code->tail = NULL;
    code->instruction_count = 0;
    return code;
}

/* Generate a new temporary variable name: t0, t1, t2, ... */
char* new_temp() {
    char* temp = (char*)malloc(20);
    snprintf(temp, 20, "t%d", temp_count++);
    return temp;
}

/* Generate a new label name: L0, L1, L2, ... */
char* new_label() {
    char* label = (char*)malloc(20);
    snprintf(label, 20, "L%d", label_count++);
    return label;
}

/* Create a new TAC instruction */
TACInstruction* create_tac_instruction(TACOpcode opcode,
                                       const char* result,
                                       const char* op1,
                                       const char* op2,
                                       const char* label) {
    TACInstruction* inst = (TACInstruction*)malloc(sizeof(TACInstruction));
    if (!inst) {
        fprintf(stderr, "Fatal Error: Failed to allocate TAC instruction\n");
        exit(1);
    }

    inst->opcode = opcode;
    inst->result = result ? strdup(result) : NULL;
    inst->op1 = op1 ? strdup(op1) : NULL;
    inst->op2 = op2 ? strdup(op2) : NULL;
    inst->label = label ? strdup(label) : NULL;
    inst->next = NULL;

    return inst;
}

/* Append an instruction to the TAC code list */
void append_tac(TACCode* code, TACInstruction* inst) {
    if (!code->head) {
        /* First instruction */
        code->head = inst;
        code->tail = inst;
    } else {
        /* Append to end */
        code->tail->next = inst;
        code->tail = inst;
    }
    code->instruction_count++;
}

/* Generate TAC for an expression - returns name of result variable/temp */
char* gen_expression(ASTNode* node, TACCode* code) {
    if (!node) return NULL;

    switch (node->type) {
        case NODE_NUMBER: {
            /* Integer literal: create temp and load constant */
            char* temp = new_temp();
            char num_str[20];
            snprintf(num_str, 20, "%d", node->data.num_value);

            TACInstruction* inst = create_tac_instruction(TAC_LOAD_CONST,
                                                          temp, num_str,
                                                          NULL, NULL);
            append_tac(code, inst);
            return temp;
        }

        case NODE_IDENTIFIER: {
            /* Variable reference: just return the variable name */
            return strdup(node->data.str_value);
        }

        case NODE_BINARY_OP: {
            /* Binary operation: op1 operator op2 */
            char* left = gen_expression(node->data.binary_op.left, code);
            char* right = gen_expression(node->data.binary_op.right, code);

            char* result = new_temp();

            /* Determine the opcode based on operator */
            TACOpcode opcode;
            if (strcmp(node->data.binary_op.operator, "+") == 0) {
                opcode = TAC_ADD;
            } else if (strcmp(node->data.binary_op.operator, "-") == 0) {
                opcode = TAC_SUB;
            } else if (strcmp(node->data.binary_op.operator, "*") == 0) {
                opcode = TAC_MUL;
            } else if (strcmp(node->data.binary_op.operator, "/") == 0) {
                opcode = TAC_DIV;
            } else if (strcmp(node->data.binary_op.operator, "%") == 0) {
                opcode = TAC_MOD;
            } else {
                opcode = TAC_ADD;  /* Default fallback */
            }

            TACInstruction* inst = create_tac_instruction(opcode,
                                                          result, left, right,
                                                          NULL);
            append_tac(code, inst);

            return result;
        }

        case NODE_CONDITION: {
            /* Relational operation: op1 relop op2 */
            char* left = gen_expression(node->data.binary_op.left, code);
            char* right = gen_expression(node->data.binary_op.right, code);

            char* result = new_temp();

            /* Create relational operation instruction */
            TACInstruction* inst = create_tac_instruction(TAC_RELOP,
                                                          result, left, right,
                                                          node->data.binary_op.operator);
            append_tac(code, inst);

            return result;
        }

        case NODE_ARRAY_ACCESS: {
            /* Array access: arr[index] */
            char* array_name = node->data.array_access.array_name;
            char* index = gen_expression(node->data.array_access.index, code);

            char* result = new_temp();

            /* TAC_ARRAY_LOAD: result = array[index] */
            TACInstruction* inst = create_tac_instruction(TAC_ARRAY_LOAD,
                                                          result, array_name, index,
                                                          NULL);
            append_tac(code, inst);

            return result;
        }

        case NODE_FUNCTION_CALL: {
            /* Function call as expression: result = func(args) */
            char* func_name = node->data.func_call.func_name;
            ASTNode* args = node->data.func_call.args;

            /* Generate param instructions for each argument */
            int arg_count = 0;
            ASTNode* current_arg = args;
            while (current_arg) {
                if (current_arg->type == NODE_ARG_LIST) {
                    char* arg_result = gen_expression(current_arg->data.list.item, code);

                    TACInstruction* param = create_tac_instruction(TAC_PARAM,
                                                                   NULL, arg_result,
                                                                   NULL, NULL);
                    append_tac(code, param);

                    arg_count++;
                    current_arg = current_arg->data.list.next;
                } else {
                    char* arg_result = gen_expression(current_arg, code);

                    TACInstruction* param = create_tac_instruction(TAC_PARAM,
                                                                   NULL, arg_result,
                                                                   NULL, NULL);
                    append_tac(code, param);

                    arg_count++;
                    break;
                }
            }

            /* Generate call instruction */
            char* result = new_temp();
            char arg_count_str[20];
            snprintf(arg_count_str, 20, "%d", arg_count);

            TACInstruction* call = create_tac_instruction(TAC_CALL,
                                                         result, arg_count_str,
                                                         NULL, func_name);
            append_tac(code, call);

            return result;
        }

        default:
            return NULL;
    }
}

/* Generate TAC for a statement */
void gen_statement(ASTNode* node, TACCode* code) {
    if (!node) return;

    switch (node->type) {
        case NODE_DECLARATION:
            /* Declarations don't generate code - handled by symbol table */
            break;

        case NODE_ASSIGNMENT: {
            /* Assignment: var = expr */
            char* expr_result = gen_expression(node->data.assignment.expr, code);

            TACInstruction* inst = create_tac_instruction(TAC_ASSIGN,
                                                          node->data.assignment.var_name,
                                                          expr_result,
                                                          NULL, NULL);
            append_tac(code, inst);
            break;
        }

        case NODE_PRINT: {
            /* Print statement: print(expr) */
            char* expr_result = gen_expression(node->data.print.expr, code);

            TACInstruction* inst = create_tac_instruction(TAC_PRINT,
                                                          NULL, expr_result,
                                                          NULL, NULL);
            append_tac(code, inst);
            break;
        }

        case NODE_WHILE: {
            /* While loop: while (condition) { body }
             *
             * Generated code structure:
             *   L_start:                  // Loop start label
             *     temp = condition        // Evaluate condition
             *     if_false temp goto L_end  // Exit if false
             *     <body>                  // Loop body
             *     goto L_start            // Jump back to start
             *   L_end:                    // Loop end label
             */

            char* label_start = new_label();
            char* label_end = new_label();

            /* L_start: */
            TACInstruction* start_label = create_tac_instruction(TAC_LABEL,
                                                                 NULL, NULL,
                                                                 NULL, label_start);
            append_tac(code, start_label);

            /* Evaluate condition */
            char* cond_result = gen_expression(node->data.while_loop.condition, code);

            /* if_false cond_result goto L_end */
            TACInstruction* if_false = create_tac_instruction(TAC_IF_FALSE,
                                                              NULL, cond_result,
                                                              NULL, label_end);
            append_tac(code, if_false);

            /* Generate code for loop body */
            gen_statement(node->data.while_loop.body, code);

            /* goto L_start */
            TACInstruction* goto_start = create_tac_instruction(TAC_GOTO,
                                                                NULL, NULL,
                                                                NULL, label_start);
            append_tac(code, goto_start);

            /* L_end: */
            TACInstruction* end_label = create_tac_instruction(TAC_LABEL,
                                                               NULL, NULL,
                                                               NULL, label_end);
            append_tac(code, end_label);

            break;
        }

        case NODE_IF: {
            /* If statement: if (condition) { then_branch } [else { else_branch }]
             *
             * WITHOUT else:                  WITH else:
             *   cond_temp = condition          cond_temp = condition
             *   if_false cond_temp goto L_end  if_false cond_temp goto L_else
             *   <then_branch>                  <then_branch>
             * L_end:                           goto L_end
             *                                L_else:
             *                                  <else_branch>
             *                                L_end:
             */

            char* label_end = new_label();
            char* label_else = NULL;

            /* Evaluate condition */
            char* cond_result = gen_expression(node->data.if_stmt.condition, code);

            if (node->data.if_stmt.else_branch != NULL) {
                /* Has else branch */
                label_else = new_label();

                /* if_false cond_result goto L_else */
                TACInstruction* if_false = create_tac_instruction(TAC_IF_FALSE,
                                                                  NULL, cond_result,
                                                                  NULL, label_else);
                append_tac(code, if_false);

                /* Generate then branch */
                gen_statement(node->data.if_stmt.then_branch, code);

                /* goto L_end (skip else) */
                TACInstruction* goto_end = create_tac_instruction(TAC_GOTO,
                                                                  NULL, NULL,
                                                                  NULL, label_end);
                append_tac(code, goto_end);

                /* L_else: */
                TACInstruction* else_label = create_tac_instruction(TAC_LABEL,
                                                                    NULL, NULL,
                                                                    NULL, label_else);
                append_tac(code, else_label);

                /* Generate else branch */
                gen_statement(node->data.if_stmt.else_branch, code);

            } else {
                /* No else branch */

                /* if_false cond_result goto L_end */
                TACInstruction* if_false = create_tac_instruction(TAC_IF_FALSE,
                                                                  NULL, cond_result,
                                                                  NULL, label_end);
                append_tac(code, if_false);

                /* Generate then branch */
                gen_statement(node->data.if_stmt.then_branch, code);
            }

            /* L_end: */
            TACInstruction* end_label = create_tac_instruction(TAC_LABEL,
                                                               NULL, NULL,
                                                               NULL, label_end);
            append_tac(code, end_label);

            break;
        }

        case NODE_STATEMENT_LIST: {
            /* Statement list: generate code for each statement */
            gen_statement(node->data.stmt_list.statement, code);
            gen_statement(node->data.stmt_list.next, code);
            break;
        }

        case NODE_FUNCTION_DEF: {
            /* Function definition: type name(params) { body }
             *
             * Generated code structure:
             *   FUNCTION function_name:    // Function label
             *     <body statements>        // Function body
             *     return_void              // Implicit return for void functions
             */

            char* func_name = node->data.function.func_name;

            /* Generate function label */
            TACInstruction* func_label = create_tac_instruction(TAC_FUNCTION_LABEL,
                                                                NULL, NULL,
                                                                NULL, func_name);
            append_tac(code, func_label);

            /* Generate code for function body */
            gen_statement(node->data.function.body, code);

            /* Add implicit return for void functions */
            if (strcmp(node->data.function.return_type, "void") == 0) {
                TACInstruction* ret_void = create_tac_instruction(TAC_RETURN_VOID,
                                                                  NULL, NULL,
                                                                  NULL, NULL);
                append_tac(code, ret_void);
            }

            break;
        }

        case NODE_FUNCTION_CALL: {
            /* Function call: result = func(arg1, arg2, ...)
             *
             * Generated code structure:
             *   param arg1               // Push each argument
             *   param arg2
             *   result = call func, 2    // Call function with arg count
             */

            char* func_name = node->data.func_call.func_name;
            ASTNode* args = node->data.func_call.args;

            /* Count arguments and generate param instructions */
            int arg_count = 0;
            ASTNode* current_arg = args;
            while (current_arg) {
                if (current_arg->type == NODE_ARG_LIST) {
                    /* Generate expression for this argument */
                    char* arg_result = gen_expression(current_arg->data.list.item, code);

                    /* Generate param instruction */
                    TACInstruction* param = create_tac_instruction(TAC_PARAM,
                                                                   NULL, arg_result,
                                                                   NULL, NULL);
                    append_tac(code, param);

                    arg_count++;
                    current_arg = current_arg->data.list.next;
                } else {
                    /* Single argument */
                    char* arg_result = gen_expression(current_arg, code);

                    TACInstruction* param = create_tac_instruction(TAC_PARAM,
                                                                   NULL, arg_result,
                                                                   NULL, NULL);
                    append_tac(code, param);

                    arg_count++;
                    break;
                }
            }

            /* Generate call instruction */
            char* result = new_temp();
            char arg_count_str[20];
            snprintf(arg_count_str, 20, "%d", arg_count);

            TACInstruction* call = create_tac_instruction(TAC_CALL,
                                                         result, arg_count_str,
                                                         NULL, func_name);
            append_tac(code, call);

            break;
        }

        case NODE_RETURN: {
            /* Return statement: return expr;
             *
             * Generated code:
             *   return result            // Return with value
             * or
             *   return_void              // Return without value
             */

            if (node->data.return_stmt.expr) {
                /* Return with value */
                char* expr_result = gen_expression(node->data.return_stmt.expr, code);

                TACInstruction* ret = create_tac_instruction(TAC_RETURN,
                                                            NULL, expr_result,
                                                            NULL, NULL);
                append_tac(code, ret);
            } else {
                /* Return without value (void) */
                TACInstruction* ret_void = create_tac_instruction(TAC_RETURN_VOID,
                                                                  NULL, NULL,
                                                                  NULL, NULL);
                append_tac(code, ret_void);
            }

            break;
        }

        default:
            break;
    }
}

/* Generate TAC for the entire program */
TACCode* generate_tac(ASTNode* root) {
    printf("\n=========== INTERMEDIATE CODE GENERATION STARTED ==========\n\n");

    TACCode* code = create_tac_code();

    /* Reset counters for each compilation */
    temp_count = 0;
    label_count = 0;

    if (root && root->type == NODE_PROGRAM) {
        /* Handle program with declaration list (may include functions) */
        ASTNode* current = root->data.program.statements;

        while (current) {
            if (current->type == NODE_STATEMENT_LIST) {
                gen_statement(current->data.stmt_list.statement, code);
                current = current->data.stmt_list.next;
            } else {
                gen_statement(current, code);
                break;
            }
        }
    }

    printf("Generated %d TAC instructions\n", code->instruction_count);
    printf("\n=========== INTERMEDIATE CODE GENERATION COMPLETE =========\n");

    return code;
}

/* Convert opcode to string for printing */
const char* opcode_to_string(TACOpcode opcode) {
    switch (opcode) {
        case TAC_ADD:        return "ADD";
        case TAC_SUB:        return "SUB";
        case TAC_MUL:        return "MUL";
        case TAC_DIV:        return "DIV";
        case TAC_MOD:        return "MOD";
        case TAC_ASSIGN:     return "ASSIGN";
        case TAC_LOAD_CONST: return "LOAD_CONST";
        case TAC_PRINT:      return "PRINT";
        case TAC_LABEL:      return "LABEL";
        case TAC_GOTO:       return "GOTO";
        case TAC_IF_FALSE:   return "IF_FALSE";
        case TAC_RELOP:      return "RELOP";
        case TAC_ARRAY_STORE: return "ARRAY_STORE";
        case TAC_ARRAY_LOAD:  return "ARRAY_LOAD";
        case TAC_FUNCTION_LABEL: return "FUNCTION";
        case TAC_PARAM:       return "PARAM";
        case TAC_CALL:        return "CALL";
        case TAC_RETURN:      return "RETURN";
        case TAC_RETURN_VOID: return "RETURN_VOID";
        default:             return "UNKNOWN";
    }
}

/* Print the TAC code in a readable format */
void print_tac(TACCode* code) {
    printf("\n=============== THREE-ADDRESS CODE (TAC) ==================\n\n");
    printf("%-5s %-15s %-10s %-10s %-10s %-10s\n",
           "Line", "Opcode", "Result", "Op1", "Op2", "Label");
    printf("------------------------------------------------------------\n");

    TACInstruction* current = code->head;
    int line_num = 0;

    while (current) {
        printf("%-5d %-15s", line_num++, opcode_to_string(current->opcode));

        switch (current->opcode) {
            case TAC_ADD:
            case TAC_SUB:
            case TAC_MUL:
            case TAC_DIV:
            case TAC_MOD:
                printf(" %-10s %-10s %-10s\n",
                       current->result, current->op1, current->op2);
                break;

            case TAC_ASSIGN:
                printf(" %-10s %-10s\n",
                       current->result, current->op1);
                break;

            case TAC_LOAD_CONST:
                printf(" %-10s %-10s\n",
                       current->result, current->op1);
                break;

            case TAC_PRINT:
                printf(" %-10s %-10s\n", "-", current->op1);
                break;

            case TAC_LABEL:
                printf(" %-10s %-10s %-10s %-10s\n",
                       "-", "-", "-", current->label);
                break;

            case TAC_GOTO:
                printf(" %-10s %-10s %-10s %-10s\n",
                       "-", "-", "-", current->label);
                break;

            case TAC_IF_FALSE:
                printf(" %-10s %-10s %-10s %-10s\n",
                       "-", current->op1, "-", current->label);
                break;

            case TAC_RELOP:
                printf(" %-10s %-10s %-10s %-10s\n",
                       current->result, current->op1, current->op2, current->label);
                break;

            case TAC_ARRAY_LOAD:
                printf(" %-10s %-10s %-10s (array load)\n",
                       current->result, current->op1, current->op2);
                break;

            case TAC_ARRAY_STORE:
                printf(" %-10s %-10s %-10s (array store)\n",
                       current->result, current->op1, current->op2);
                break;

            case TAC_FUNCTION_LABEL:
                printf(" %-10s %-10s %-10s %-10s\n",
                       "-", "-", "-", current->label);
                break;

            case TAC_PARAM:
                printf(" %-10s %-10s\n", "-", current->op1);
                break;

            case TAC_CALL:
                printf(" %-10s %-10s %-10s (call)\n",
                       current->result ? current->result : "-", current->label, current->op1);
                break;

            case TAC_RETURN:
                printf(" %-10s %-10s\n", "-", current->op1);
                break;

            case TAC_RETURN_VOID:
                printf("\n");
                break;

            default:
                printf("\n");
                break;
        }

        current = current->next;
    }

    printf("============================================================\n");
    printf("Total instructions: %d\n\n", code->instruction_count);
}

/* Free all TAC code memory */
void free_tac(TACCode* code) {
    if (!code) return;

    TACInstruction* current = code->head;
    while (current) {
        TACInstruction* next = current->next;

        free(current->result);
        free(current->op1);
        free(current->op2);
        free(current->label);
        free(current);

        current = next;
    }

    free(code);
}
