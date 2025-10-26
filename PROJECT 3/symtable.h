/*
 * SYMTABLE.H - Symbol Table Header
 * CST-405 Compiler Project
 *
 * This file defines the symbol table structure used for tracking
 * variables, their types, and scope information during compilation.
 * Essential for semantic analysis and type checking.
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Data types supported by our language */
typedef enum {
    TYPE_INT,          /* Integer type */
    TYPE_VOID,         /* Void type (for functions) */
    TYPE_UNKNOWN       /* Unknown/undefined type (for error handling) */
} DataType;

/* Symbol kind - variable or function */
typedef enum {
    SYMBOL_VARIABLE,   /* Regular variable or array */
    SYMBOL_FUNCTION    /* Function */
} SymbolKind;

/* Symbol table entry - Represents one variable or function */
typedef struct Symbol {
    char* name;              /* Symbol name (identifier) */
    SymbolKind kind;         /* Variable or function */
    DataType type;           /* Data type (int, void, etc.) */
    int is_initialized;      /* Flag: has this variable been assigned a value? */
    int is_array;            /* Flag: is this an array? */
    int array_size;          /* Size of array (if is_array is true) */

    /* Function-specific fields */
    DataType return_type;    /* Function return type */
    int param_count;         /* Number of parameters */
    DataType* param_types;   /* Array of parameter types */
    char** param_names;      /* Array of parameter names */

    int declaration_line;    /* Source line where symbol was declared */
    struct Symbol* next;     /* Next symbol in the hash chain (for collision handling) */
} Symbol;

/* Symbol Table - Hash table for efficient variable lookup */
typedef struct SymbolTable {
    Symbol** table;          /* Array of symbol pointers (hash table buckets) */
    int size;               /* Size of the hash table */
    int num_symbols;        /* Number of symbols currently stored */
} SymbolTable;

/* SYMBOL TABLE MANAGEMENT FUNCTIONS */

/* Create a new symbol table with given size */
SymbolTable* create_symbol_table(int size);

/* Add a symbol to the table
 * Returns 1 on success, 0 if symbol already exists (redeclaration error) */
int add_symbol(SymbolTable* table, const char* name, DataType type, int line);

/* Add an array symbol to the table
 * Returns 1 on success, 0 if symbol already exists (redeclaration error) */
int add_array_symbol(SymbolTable* table, const char* name, DataType type, int size, int line);

/* Add a function symbol to the table
 * Returns 1 on success, 0 if symbol already exists (redeclaration error) */
int add_function_symbol(SymbolTable* table, const char* name, DataType return_type,
                        int param_count, DataType* param_types, char** param_names, int line);

/* Look up a symbol by name
 * Returns pointer to symbol if found, NULL otherwise */
Symbol* lookup_symbol(SymbolTable* table, const char* name);

/* Mark a symbol as initialized (for use-before-init checking) */
void mark_initialized(SymbolTable* table, const char* name);

/* Check if a symbol is initialized */
int is_initialized(SymbolTable* table, const char* name);

/* Print the entire symbol table (for debugging and output) */
void print_symbol_table(SymbolTable* table);

/* Free all memory used by the symbol table */
void free_symbol_table(SymbolTable* table);

/* Get string representation of a data type */
const char* type_to_string(DataType type);

/* HASH FUNCTION (Internal use) */
unsigned int hash(const char* str, int table_size);

#endif /* SYMTABLE_H */
