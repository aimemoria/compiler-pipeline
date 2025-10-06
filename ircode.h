/*
 * IRCODE.H - Intermediate Representation Code Header
 * CST-405 Compiler Project
 *
 * This file defines the Three-Address Code (TAC) intermediate representation.
 * TAC is a linearized, low-level representation of the program that's easy
 * to translate into assembly code.
 *
 * Three-Address Code Format:
 *   result = operand1 op operand2
 *   Each instruction has at most 3 addresses (result, op1, op2)
 */

#ifndef IRCODE_H
#define IRCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symtable.h"

/* TAC Instruction Types */
typedef enum {
    TAC_ADD,           /* result = op1 + op2 */
    TAC_ASSIGN,        /* result = op1 (copy) */
    TAC_LOAD_CONST,    /* result = constant */
    TAC_PRINT,         /* print(op1) */
    TAC_LABEL,         /* label: (for control flow) */
    TAC_GOTO,          /* goto label (unconditional jump) */
    TAC_IF_FALSE,      /* if_false op1 goto label (conditional jump) */
    TAC_RELOP          /* result = op1 relop op2 (for conditions) */
} TACOpcode;

/* Three-Address Code Instruction */
typedef struct TACInstruction {
    TACOpcode opcode;                /* Operation type */
    char* result;                    /* Result operand (left side) */
    char* op1;                       /* First operand */
    char* op2;                       /* Second operand (if needed) */
    char* label;                     /* Label (for jumps and labels) */
    struct TACInstruction* next;     /* Next instruction in sequence */
} TACInstruction;

/* TAC Code List - Holds all generated instructions */
typedef struct {
    TACInstruction* head;            /* First instruction */
    TACInstruction* tail;            /* Last instruction (for efficient append) */
    int instruction_count;           /* Number of instructions */
} TACCode;

/* Temporary variable and label generation */
extern int temp_count;               /* Counter for temporary variables */
extern int label_count;              /* Counter for labels */

/* INTERMEDIATE CODE GENERATION FUNCTIONS */

/* Create a new TAC code list */
TACCode* create_tac_code();

/* Generate a new temporary variable name (t0, t1, t2, ...) */
char* new_temp();

/* Generate a new label name (L0, L1, L2, ...) */
char* new_label();

/* Create a new TAC instruction */
TACInstruction* create_tac_instruction(TACOpcode opcode,
                                       const char* result,
                                       const char* op1,
                                       const char* op2,
                                       const char* label);

/* Append an instruction to the TAC code list */
void append_tac(TACCode* code, TACInstruction* inst);

/* Generate TAC for the entire program (main entry point) */
TACCode* generate_tac(ASTNode* root);

/* Generate TAC for a statement */
void gen_statement(ASTNode* node, TACCode* code);

/* Generate TAC for an expression
 * Returns the name of the temporary/variable holding the result */
char* gen_expression(ASTNode* node, TACCode* code);

/* Print TAC code in readable format */
void print_tac(TACCode* code);

/* Free TAC code memory */
void free_tac(TACCode* code);

/* Get string representation of opcode (for debugging) */
const char* opcode_to_string(TACOpcode opcode);

#endif /* IRCODE_H */
