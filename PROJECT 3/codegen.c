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

    fprintf(gen->output_file, "section .note.GNU-stack noalloc noexec nowrite progbits\n\n");

    fprintf(gen->output_file, "section .data\n");
    fprintf(gen->output_file, "    ; Data section for constants\n");
    fprintf(gen->output_file, "    fmt_int: db \"%%d\", 10, 0  ; Format string for printing integers\n\n");

    fprintf(gen->output_file, "section .bss\n");
    fprintf(gen->output_file, "    ; BSS section for uninitialized data\n");

    /* Allocate space for all variables in the symbol table (not functions) */
    if (gen->symtab) {
        for (int i = 0; i < gen->symtab->size; i++) {
            Symbol* sym = gen->symtab->table[i];
            while (sym) {
                /* Only allocate space for variables, not functions */
                if (sym->kind == SYMBOL_VARIABLE) {
                    if (sym->is_array) {
                        /* Arrays need space for multiple elements */
                        fprintf(gen->output_file, "    %s: resq %d  ; Array: %s[%d]\n",
                                sym->name, sym->array_size, sym->name, sym->array_size);
                    } else {
                        /* Regular variables need 1 qword */
                        fprintf(gen->output_file, "    %s: resq 1  ; Variable: %s\n",
                                sym->name, sym->name);
                    }
                }
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

        case TAC_SUB:
            /* Subtraction: result = op1 - op2 */
            fprintf(gen->output_file, "    ; %s = %s - %s\n",
                    inst->result, inst->op1, inst->op2);
            fprintf(gen->output_file, "    mov rax, [%s]\n", inst->op1);
            fprintf(gen->output_file, "    sub rax, [%s]\n", inst->op2);
            fprintf(gen->output_file, "    mov [%s], rax\n\n", inst->result);
            break;

        case TAC_MUL:
            /* Multiplication: result = op1 * op2 */
            fprintf(gen->output_file, "    ; %s = %s * %s\n",
                    inst->result, inst->op1, inst->op2);
            fprintf(gen->output_file, "    mov rax, [%s]\n", inst->op1);
            fprintf(gen->output_file, "    imul rax, [%s]\n", inst->op2);
            fprintf(gen->output_file, "    mov [%s], rax\n\n", inst->result);
            break;

        case TAC_DIV:
            /* Division: result = op1 / op2 */
            fprintf(gen->output_file, "    ; %s = %s / %s\n",
                    inst->result, inst->op1, inst->op2);
            fprintf(gen->output_file, "    mov rax, [%s]\n", inst->op1);
            fprintf(gen->output_file, "    cqo              ; Sign-extend rax to rdx:rax\n");
            fprintf(gen->output_file, "    mov rbx, [%s]\n", inst->op2);
            fprintf(gen->output_file, "    idiv rbx          ; Signed divide rdx:rax by rbx\n");
            fprintf(gen->output_file, "    mov [%s], rax\n\n", inst->result);
            break;

        case TAC_MOD:
            /* Modulo: result = op1 % op2 */
            fprintf(gen->output_file, "    ; %s = %s %% %s\n",
                    inst->result, inst->op1, inst->op2);
            fprintf(gen->output_file, "    mov rax, [%s]\n", inst->op1);
            fprintf(gen->output_file, "    cqo              ; Sign-extend rax to rdx:rax\n");
            fprintf(gen->output_file, "    mov rbx, [%s]\n", inst->op2);
            fprintf(gen->output_file, "    idiv rbx          ; Signed divide rdx:rax by rbx\n");
            fprintf(gen->output_file, "    mov [%s], rdx    ; Remainder is in rdx\n\n", inst->result);
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

        case TAC_ARRAY_LOAD:
            /* Array load: result = array[index] */
            fprintf(gen->output_file, "    ; %s = %s[%s]\n",
                    inst->result, inst->op1, inst->op2);
            fprintf(gen->output_file, "    mov rax, [%s]     ; Get index\n", inst->op2);
            fprintf(gen->output_file, "    imul rax, 8        ; Multiply by element size (8 bytes)\n");
            fprintf(gen->output_file, "    lea rbx, [%s]      ; Get array base address\n", inst->op1);
            fprintf(gen->output_file, "    add rbx, rax       ; Add offset\n");
            fprintf(gen->output_file, "    mov rax, [rbx]     ; Load array element\n");
            fprintf(gen->output_file, "    mov [%s], rax      ; Store in result\n\n", inst->result);
            break;

        case TAC_ARRAY_STORE:
            /* Array store: array[index] = value */
            fprintf(gen->output_file, "    ; %s[%s] = %s\n",
                    inst->result, inst->op1, inst->op2);
            fprintf(gen->output_file, "    mov rax, [%s]     ; Get index\n", inst->op1);
            fprintf(gen->output_file, "    imul rax, 8        ; Multiply by element size (8 bytes)\n");
            fprintf(gen->output_file, "    lea rbx, [%s]      ; Get array base address\n", inst->result);
            fprintf(gen->output_file, "    add rbx, rax       ; Add offset\n");
            fprintf(gen->output_file, "    mov rax, [%s]      ; Get value to store\n", inst->op2);
            fprintf(gen->output_file, "    mov [rbx], rax     ; Store in array\n\n");
            break;

        case TAC_FUNCTION_LABEL:
            /* Function label: function_name: */
            fprintf(gen->output_file, "\n; Function: %s\n", inst->label);
            fprintf(gen->output_file, "%s:\n", inst->label);
            fprintf(gen->output_file, "    ; Function prologue\n");
            fprintf(gen->output_file, "    push rbp\n");
            fprintf(gen->output_file, "    mov rbp, rsp\n");
            fprintf(gen->output_file, "    sub rsp, 64       ; Reserve space for local variables\n\n");
            break;

        case TAC_PARAM:
            /* Parameter passing: param value
             * System V AMD64 calling convention:
             * First 6 integer args in: rdi, rsi, rdx, rcx, r8, r9
             * Additional args pushed on stack in reverse order
             * For simplicity, we'll push all params on stack
             */
            fprintf(gen->output_file, "    ; param %s\n", inst->op1);
            fprintf(gen->output_file, "    mov rax, [%s]\n", inst->op1);
            fprintf(gen->output_file, "    push rax\n\n");
            break;

        case TAC_CALL:
            /* Function call: result = call function_name, num_args
             * inst->result = result temp
             * inst->label = function name
             * inst->op1 = number of arguments
             */
            fprintf(gen->output_file, "    ; %s = call %s, %s args\n",
                    inst->result, inst->label, inst->op1);

            /* Align stack to 16 bytes (required by System V AMD64) */
            fprintf(gen->output_file, "    and rsp, -16      ; Align stack to 16 bytes\n");

            /* Call the function */
            fprintf(gen->output_file, "    call %s\n", inst->label);

            /* Clean up stack (pop parameters) */
            int arg_count = atoi(inst->op1);
            if (arg_count > 0) {
                fprintf(gen->output_file, "    add rsp, %d       ; Clean up %d args from stack\n",
                        arg_count * 8, arg_count);
            }

            /* Store return value (in rax) to result */
            fprintf(gen->output_file, "    mov [%s], rax     ; Store return value\n\n",
                    inst->result);
            break;

        case TAC_RETURN:
            /* Return statement: return value */
            fprintf(gen->output_file, "    ; return %s\n", inst->op1);
            fprintf(gen->output_file, "    mov rax, [%s]     ; Load return value\n", inst->op1);
            fprintf(gen->output_file, "    mov rsp, rbp      ; Function epilogue\n");
            fprintf(gen->output_file, "    pop rbp\n");
            fprintf(gen->output_file, "    ret\n\n");
            break;

        case TAC_RETURN_VOID:
            /* Return from void function */
            fprintf(gen->output_file, "    ; return (void)\n");
            fprintf(gen->output_file, "    mov rsp, rbp      ; Function epilogue\n");
            fprintf(gen->output_file, "    pop rbp\n");
            fprintf(gen->output_file, "    ret\n\n");
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
