/*
 * COMPILER.C - Main Compiler Driver
 * CST-405 Complete Compiler Project
 *
 * This is the main entry point for the complete compiler.
 * It orchestrates all compilation phases:
 *   1. Lexical Analysis (Scanning)
 *   2. Syntax Analysis (Parsing)
 *   3. Semantic Analysis
 *   4. Intermediate Code Generation (TAC)
 *   5. Code Generation (Assembly)
 *
 * NEW FEATURE: Support for while loops with relational operators
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symtable.h"
#include "semantic.h"
#include "ircode.h"
#include "codegen.h"

/* External declarations from parser */
extern int yyparse();
extern FILE* yyin;
extern ASTNode* ast_root;
extern SymbolTable* global_symtab;
extern int syntax_errors;
extern int line_num;

/* Function prototypes */
void print_banner();
void print_phase_separator(const char* phase_name);
void print_summary(int success);

int main(int argc, char* argv[]) {
    /* Print compiler banner */
    print_banner();

    /* Check command line arguments */
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        fprintf(stderr, "Example: %s program.src\n", argv[0]);
        return 1;
    }

    const char* input_filename = argv[1];
    const char* output_filename = "output.asm";

    /* Open input file */
    FILE* input_file = fopen(input_filename, "r");
    if (!input_file) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", input_filename);
        return 1;
    }

    yyin = input_file;
    printf("📄 Input file: %s\n", input_filename);
    printf("📝 Output file: %s\n\n", output_filename);

    /* ===================================================================
     * PHASE 1 & 2: LEXICAL AND SYNTAX ANALYSIS
     * The lexer (scanner) and parser work together during yyparse()
     * ================================================================ */
    print_phase_separator("PHASE 1 & 2: LEXICAL AND SYNTAX ANALYSIS");

    /* Initialize symbol table before parsing */
    global_symtab = create_symbol_table(100);

    /* Run the parser (which calls the lexer) */
    int parse_result = yyparse();

    /* Check for syntax errors */
    if (parse_result != 0 || syntax_errors > 0) {
        fprintf(stderr, "\n✗ COMPILATION FAILED: Syntax errors detected\n");
        fprintf(stderr, "✗ Please fix the errors and try again\n\n");

        fclose(input_file);
        if (global_symtab) free_symbol_table(global_symtab);
        if (ast_root) free_ast(ast_root);
        return 1;
    }

    printf("✓ Lexical analysis complete\n");
    printf("✓ Syntax analysis complete\n");
    printf("✓ Abstract Syntax Tree (AST) constructed\n\n");

    /* ===================================================================
     * PHASE 3: SEMANTIC ANALYSIS
     * Type checking, variable declaration/initialization checking
     * ================================================================ */
    print_phase_separator("PHASE 3: SEMANTIC ANALYSIS");

    int semantic_result = analyze_semantics(ast_root, global_symtab);

    if (semantic_result > 0) {
        fprintf(stderr, "\n✗ COMPILATION FAILED: Semantic errors detected\n");
        fprintf(stderr, "✗ Please fix the errors and try again\n\n");

        fclose(input_file);
        free_symbol_table(global_symtab);
        free_ast(ast_root);
        return 1;
    }

    print_semantic_summary();

    /* Print AST for debugging */
    printf("═══════════════ ABSTRACT SYNTAX TREE ══════════════════\n\n");
    print_ast(ast_root, 0);
    printf("\n");

    /* Print symbol table */
    printf("═══════════════════ SYMBOL TABLE ══════════════════════\n\n");
    print_symbol_table(global_symtab);
    printf("\n");

    /* ===================================================================
     * PHASE 4: INTERMEDIATE CODE GENERATION
     * Generate Three-Address Code (TAC) from AST
     * ================================================================ */
    print_phase_separator("PHASE 4: INTERMEDIATE CODE GENERATION");

    TACCode* tac = generate_tac(ast_root);

    if (!tac) {
        fprintf(stderr, "\n✗ COMPILATION FAILED: IR generation failed\n\n");

        fclose(input_file);
        free_symbol_table(global_symtab);
        free_ast(ast_root);
        return 1;
    }

    /* Print TAC */
    print_tac(tac);

    /* ===================================================================
     * PHASE 5: CODE GENERATION
     * Generate assembly code from TAC
     * ================================================================ */
    print_phase_separator("PHASE 5: ASSEMBLY CODE GENERATION");

    CodeGenerator* codegen = create_code_generator(output_filename, global_symtab);
    generate_assembly(codegen, tac);
    close_code_generator(codegen);

    /* ===================================================================
     * COMPILATION COMPLETE
     * ================================================================ */
    print_summary(1);

    printf("✓ Compilation successful!\n");
    printf("✓ Assembly code written to: %s\n\n", output_filename);

    printf("To assemble and link (on Linux):\n");
    printf("  nasm -f elf64 %s -o output.o\n", output_filename);
    printf("  gcc output.o -o program -no-pie\n");
    printf("  ./program\n\n");

    /* Cleanup */
    fclose(input_file);
    free_ast(ast_root);
    free_symbol_table(global_symtab);
    free_tac(tac);

    return 0;
}

/* Print the compiler banner */
void print_banner() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                                                           ║\n");
    printf("║           CST-405 COMPLETE COMPILER SYSTEM                ║\n");
    printf("║                                                           ║\n");
    printf("║  A full-featured compiler with lexer, parser, semantic   ║\n");
    printf("║  analyzer, intermediate code generator, and code gen.    ║\n");
    printf("║                                                           ║\n");
    printf("║  NEW FEATURE: While loops with relational operators      ║\n");
    printf("║                                                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");
}

/* Print phase separator */
void print_phase_separator(const char* phase_name) {
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║ %-57s ║\n", phase_name);
    printf("╚═══════════════════════════════════════════════════════════╝\n\n");
}

/* Print compilation summary */
void print_summary(int success) {
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                   COMPILATION SUMMARY                     ║\n");
    printf("╠═══════════════════════════════════════════════════════════╣\n");

    if (success) {
        printf("║  Status:           ✓ SUCCESS                              ║\n");
        printf("║  Lexical errors:   0                                      ║\n");
        printf("║  Syntax errors:    0                                      ║\n");
        printf("║  Semantic errors:  0                                      ║\n");
        printf("║  Code generated:   Yes                                    ║\n");
    } else {
        printf("║  Status:           ✗ FAILED                               ║\n");
    }

    printf("╚═══════════════════════════════════════════════════════════╝\n\n");
}
