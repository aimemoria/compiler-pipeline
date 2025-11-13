/*
 * CODEGEN.H - Code Generator Header
 * CST-405 Compiler Project
 *
 * This file defines the code generation phase which translates
 * Three-Address Code (TAC) into target assembly code (x86-64).
 */

#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ircode.h"
#include "symtable.h"

/* Assembly code output structure */
typedef struct {
    FILE* output_file;          /* File to write assembly code to */
    int stack_offset;           /* Current stack frame offset */
    SymbolTable* symtab;        /* Symbol table for variable locations */
} CodeGenerator;

/* CODE GENERATION FUNCTIONS */

/* Create a new code generator */
CodeGenerator* create_code_generator(const char* output_filename, SymbolTable* symtab);

/* Generate assembly code from TAC */
void generate_assembly(CodeGenerator* gen, TACCode* tac);

/* Generate assembly prologue (program setup) */
void gen_prologue(CodeGenerator* gen);

/* Generate assembly epilogue (program cleanup) */
void gen_epilogue(CodeGenerator* gen);

/* Generate code for a single TAC instruction */
void gen_tac_instruction(CodeGenerator* gen, TACInstruction* inst);

/* Get memory location for a variable/temporary */
const char* get_location(CodeGenerator* gen, const char* name);

/* Close and cleanup code generator */
void close_code_generator(CodeGenerator* gen);

#endif /* CODEGEN_H */
