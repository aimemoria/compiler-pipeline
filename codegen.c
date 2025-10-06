/*
 * CODEGEN.C - Code Generator Implementation
 * CST-405 Compiler Project
 *
 * This file implements code generation from Three-Address Code (TAC)
 * to x86-64 assembly language. The generated code uses a simple
 * stack-based calling convention.
 */

#include "codegen.h"

/* Create a new code generator instance */
CodeGenerator* create_code_generator(const char* output_filename, SymbolTable* symtab) {
    CodeGenerator* gen = (CodeGenerator*)malloc(sizeof(CodeGenerator));
    if (!gen) {
        fprintf(stderr, "Fatal Error: Failed to allocate code generator\n");
        exit(1);
    }

    gen->output_file = fopen(output_filename, "w");
    if (!gen->output_file) {
        fprintf(stderr, "Fatal Error: Cannot open output file '%s'\n", output_filename);
        free(gen);
        exit(1);
    }

    gen->stack_offset = 0;
    gen->symtab = symtab;

    return gen;
}

/* Generate the assembly prologue (program initialization) */
void gen_prologue(CodeGenerator* gen) {
    fprintf(gen->output_file, "; CST-405 Compiler - Generated Assembly Code\n");
    fprintf(gen->output_file, "; Target: x86-64 (64-bit)\n");
    fprintf(gen->output_file, "; Calling Convention: System V AMD64 ABI\n\n");

    fprintf(gen->output_file, "section .data\n");
    fprintf(gen->output_file, "    ; Data section for constants\n");
    fprintf(gen->output_file, "    fmt_int: db \"%%d\", 10, 0  ; Format string for printing integers\n\n");

    fprintf(gen->output_file, "section .bss\n");
    fprintf(gen->output_file, "    ; BSS section for uninitialized data\n");

    /* Allocate space for all variables in the symbol table */
    if (gen->symtab) {
        for (int i = 0; i < gen->symtab->size; i++) {
            Symbol* sym = gen->symtab->table[i];
            while (sym) {
                fprintf(gen->output_file, "    %s: resq 1  ; Variable: %s\n",
                        sym->name, sym->name);
                sym = sym->next;
            }
        }
    }

    /* Allocate space for temporaries (t0-t99) */
    fprintf(gen->output_file, "\n    ; Temporary variables\n");
    for (int i = 0; i < 100; i++) {
        fprintf(gen->output_file, "    t%d: resq 1\n", i);
    }

    fprintf(gen->output_file, "\nsection .text\n");
    fprintf(gen->output_file, "    global main\n");
    fprintf(gen->output_file, "    extern printf  ; External C library function\n\n");

    fprintf(gen->output_file, "main:\n");
    fprintf(gen->output_file, "    ; Function prologue\n");
    fprintf(gen->output_file, "    push rbp\n");
    fprintf(gen->output_file, "    mov rbp, rsp\n\n");
}

/* Generate the assembly epilogue (program termination) */
void gen_epilogue(CodeGenerator* gen) {
    fprintf(gen->output_file, "\n    ; Function epilogue\n");
    fprintf(gen->output_file, "    mov rsp, rbp\n");
    fprintf(gen->output_file, "    pop rbp\n");
    fprintf(gen->output_file, "    mov rax, 0    ; Return 0 (success)\n");
    fprintf(gen->output_file, "    ret\n");
}

/* Generate code for a single TAC instruction */
void gen_tac_instruction(CodeGenerator* gen, TACInstruction* inst) {
    switch (inst->opcode) {
        case TAC_LOAD_CONST:
            /* Load constant into variable: result = constant */
            fprintf(gen->output_file, "    ; %s = %s\n", inst->result, inst->op1);
            fprintf(gen->output_file, "    mov rax, %s\n", inst->op1);
            fprintf(gen->output_file, "    mov [%s], rax\n\n", inst->result);
            break;

        case TAC_ASSIGN:
            /* Assignment: result = op1 */
            fprintf(gen->output_file, "    ; %s = %s\n", inst->result, inst->op1);
            fprintf(gen->output_file, "    mov rax, [%s]\n", inst->op1);
            fprintf(gen->output_file, "    mov [%s], rax\n\n", inst->result);
            break;

        case TAC_ADD:
            /* Addition: result = op1 + op2 */
            fprintf(gen->output_file, "    ; %s = %s + %s\n",
                    inst->result, inst->op1, inst->op2);
            fprintf(gen->output_file, "    mov rax, [%s]\n", inst->op1);
            fprintf(gen->output_file, "    add rax, [%s]\n", inst->op2);
            fprintf(gen->output_file, "    mov [%s], rax\n\n", inst->result);
            break;

        case TAC_PRINT:
            /* Print: print(op1) */
            fprintf(gen->output_file, "    ; print(%s)\n", inst->op1);
            fprintf(gen->output_file, "    mov rdi, fmt_int  ; Format string\n");
            fprintf(gen->output_file, "    mov rsi, [%s]     ; Value to print\n", inst->op1);
            fprintf(gen->output_file, "    xor rax, rax      ; No vector registers used\n");
            fprintf(gen->output_file, "    call printf\n\n");
            break;

        case TAC_LABEL:
            /* Label: label: */
            fprintf(gen->output_file, "%s:\n", inst->label);
            break;

        case TAC_GOTO:
            /* Unconditional jump: goto label */
            fprintf(gen->output_file, "    ; goto %s\n", inst->label);
            fprintf(gen->output_file, "    jmp %s\n\n", inst->label);
            break;

        case TAC_RELOP:
            /* Relational operation: result = op1 relop op2 */
            fprintf(gen->output_file, "    ; %s = %s %s %s\n",
                    inst->result, inst->op1, inst->label, inst->op2);
            fprintf(gen->output_file, "    mov rax, [%s]\n", inst->op1);
            fprintf(gen->output_file, "    cmp rax, [%s]\n", inst->op2);

            /* Set result based on comparison (using setcc instructions) */
            if (strcmp(inst->label, "<") == 0) {
                fprintf(gen->output_file, "    setl al       ; Set if less\n");
            } else if (strcmp(inst->label, ">") == 0) {
                fprintf(gen->output_file, "    setg al       ; Set if greater\n");
            } else if (strcmp(inst->label, "<=") == 0) {
                fprintf(gen->output_file, "    setle al      ; Set if less or equal\n");
            } else if (strcmp(inst->label, ">=") == 0) {
                fprintf(gen->output_file, "    setge al      ; Set if greater or equal\n");
            } else if (strcmp(inst->label, "==") == 0) {
                fprintf(gen->output_file, "    sete al       ; Set if equal\n");
            } else if (strcmp(inst->label, "!=") == 0) {
                fprintf(gen->output_file, "    setne al      ; Set if not equal\n");
            }

            fprintf(gen->output_file, "    movzx rax, al     ; Zero-extend to 64-bit\n");
            fprintf(gen->output_file, "    mov [%s], rax\n\n", inst->result);
            break;

        case TAC_IF_FALSE:
            /* Conditional jump: if_false op1 goto label */
            fprintf(gen->output_file, "    ; if_false %s goto %s\n",
                    inst->op1, inst->label);
            fprintf(gen->output_file, "    mov rax, [%s]\n", inst->op1);
            fprintf(gen->output_file, "    cmp rax, 0\n");
            fprintf(gen->output_file, "    je %s         ; Jump if zero (false)\n\n",
                    inst->label);
            break;

        default:
            fprintf(gen->output_file, "    ; Unknown TAC instruction\n\n");
            break;
    }
}

/* Generate assembly code from TAC */
void generate_assembly(CodeGenerator* gen, TACCode* tac) {
    printf("\n═══════════════ CODE GENERATION STARTED ═══════════════════\n\n");

    /* Generate prologue */
    gen_prologue(gen);

    /* Generate code for each TAC instruction */
    TACInstruction* inst = tac->head;
    while (inst) {
        gen_tac_instruction(gen, inst);
        inst = inst->next;
    }

    /* Generate epilogue */
    gen_epilogue(gen);

    printf("Assembly code generated successfully\n");
    printf("Output file: output.asm\n");

    printf("\n═══════════════ CODE GENERATION COMPLETE ══════════════════\n\n");
}

/* Close and cleanup code generator */
void close_code_generator(CodeGenerator* gen) {
    if (gen) {
        if (gen->output_file) {
            fclose(gen->output_file);
        }
        free(gen);
    }
}
