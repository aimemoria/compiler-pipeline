/*
 * SYMTABLE.C - Symbol Table Implementation
 * CST-405 Compiler Project
 *
 * This file implements a hash table-based symbol table for tracking
 * variables during compilation. Provides O(1) average-case lookup.
 */

#include "symtable.h"

/* Hash function - Converts string to table index using djb2 algorithm */
unsigned int hash(const char* str, int table_size) {
    unsigned long hash_value = 5381;
    int c;

    /* Process each character in the string */
    while ((c = *str++)) {
        hash_value = ((hash_value << 5) + hash_value) + c; /* hash * 33 + c */
    }

    return hash_value % table_size;
}

/* Create a new symbol table with specified size */
SymbolTable* create_symbol_table(int size) {
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!table) {
        fprintf(stderr, "Fatal Error: Failed to allocate symbol table\n");
        exit(1);
    }

    /* Allocate array of symbol pointers (initially all NULL) */
    table->table = (Symbol**)calloc(size, sizeof(Symbol*));
    if (!table->table) {
        fprintf(stderr, "Fatal Error: Failed to allocate symbol table array\n");
        free(table);
        exit(1);
    }

    table->size = size;
    table->num_symbols = 0;

    return table;
}

/* Add a new symbol to the table */
int add_symbol(SymbolTable* table, const char* name, DataType type, int line) {
    /* First check if symbol already exists (redeclaration error) */
    if (lookup_symbol(table, name)) {
        return 0;  /* Symbol already exists */
    }

    /* Calculate hash index */
    unsigned int index = hash(name, table->size);

    /* Create new symbol */
    Symbol* new_symbol = (Symbol*)malloc(sizeof(Symbol));
    if (!new_symbol) {
        fprintf(stderr, "Fatal Error: Failed to allocate symbol\n");
        exit(1);
    }

    new_symbol->name = strdup(name);
    new_symbol->kind = SYMBOL_VARIABLE;
    new_symbol->type = type;
    new_symbol->is_initialized = 0;  /* Not initialized until assigned */
    new_symbol->is_array = 0;         /* Not an array by default */
    new_symbol->array_size = 0;       /* No array size by default */
    new_symbol->return_type = TYPE_UNKNOWN;
    new_symbol->param_count = 0;
    new_symbol->param_types = NULL;
    new_symbol->param_names = NULL;
    new_symbol->scope = strdup("global");  /* Default to global scope */
    new_symbol->declaration_line = line;
    new_symbol->next = NULL;

    /* Insert at the beginning of the chain (for collision handling) */
    if (table->table[index] == NULL) {
        table->table[index] = new_symbol;
    } else {
        /* Collision: add to front of linked list */
        new_symbol->next = table->table[index];
        table->table[index] = new_symbol;
    }

    table->num_symbols++;
    return 1;  /* Success */
}

/* Add a new array symbol to the table */
int add_array_symbol(SymbolTable* table, const char* name, DataType type, int size, int line) {
    /* First check if symbol already exists (redeclaration error) */
    if (lookup_symbol(table, name)) {
        return 0;  /* Symbol already exists */
    }

    /* Calculate hash index */
    unsigned int index = hash(name, table->size);

    /* Create new symbol */
    Symbol* new_symbol = (Symbol*)malloc(sizeof(Symbol));
    if (!new_symbol) {
        fprintf(stderr, "Fatal Error: Failed to allocate symbol\n");
        exit(1);
    }

    new_symbol->name = strdup(name);
    new_symbol->kind = SYMBOL_VARIABLE;
    new_symbol->type = type;
    new_symbol->is_initialized = 1;  /* Arrays are considered initialized upon declaration */
    new_symbol->is_array = 1;         /* This is an array */
    new_symbol->array_size = size;    /* Store array size */
    new_symbol->return_type = TYPE_UNKNOWN;
    new_symbol->param_count = 0;
    new_symbol->param_types = NULL;
    new_symbol->param_names = NULL;
    new_symbol->scope = strdup("global");  /* Default to global scope */
    new_symbol->declaration_line = line;
    new_symbol->next = NULL;

    /* Insert at the beginning of the chain (for collision handling) */
    if (table->table[index] == NULL) {
        table->table[index] = new_symbol;
    } else {
        /* Collision: add to front of linked list */
        new_symbol->next = table->table[index];
        table->table[index] = new_symbol;
    }

    table->num_symbols++;
    return 1;  /* Success */
}

/* Add a new function symbol to the table */
int add_function_symbol(SymbolTable* table, const char* name, DataType return_type,
                        int param_count, DataType* param_types, char** param_names, int line) {
    /* First check if symbol already exists (redeclaration error) */
    if (lookup_symbol(table, name)) {
        return 0;  /* Symbol already exists */
    }

    /* Calculate hash index */
    unsigned int index = hash(name, table->size);

    /* Create new symbol */
    Symbol* new_symbol = (Symbol*)malloc(sizeof(Symbol));
    if (!new_symbol) {
        fprintf(stderr, "Fatal Error: Failed to allocate symbol\n");
        exit(1);
    }

    new_symbol->name = strdup(name);
    new_symbol->kind = SYMBOL_FUNCTION;
    new_symbol->type = return_type;
    new_symbol->is_initialized = 1;  /* Functions are always "initialized" */
    new_symbol->is_array = 0;
    new_symbol->array_size = 0;
    new_symbol->return_type = return_type;
    new_symbol->param_count = param_count;

    /* Allocate and copy parameter types */
    if (param_count > 0) {
        new_symbol->param_types = (DataType*)malloc(param_count * sizeof(DataType));
        new_symbol->param_names = (char**)malloc(param_count * sizeof(char*));
        for (int i = 0; i < param_count; i++) {
            new_symbol->param_types[i] = param_types[i];
            new_symbol->param_names[i] = strdup(param_names[i]);
        }
    } else {
        new_symbol->param_types = NULL;
        new_symbol->param_names = NULL;
    }

    new_symbol->scope = strdup("global");  /* Functions are in global scope */
    new_symbol->declaration_line = line;
    new_symbol->next = NULL;

    /* Insert at the beginning of the chain (for collision handling) */
    if (table->table[index] == NULL) {
        table->table[index] = new_symbol;
    } else {
        /* Collision: add to front of linked list */
        new_symbol->next = table->table[index];
        table->table[index] = new_symbol;
    }

    table->num_symbols++;
    return 1;  /* Success */
}

/* Look up a symbol by name */
Symbol* lookup_symbol(SymbolTable* table, const char* name) {
    unsigned int index = hash(name, table->size);
    Symbol* current = table->table[index];

    /* Search through the chain at this index */
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;  /* Found it */
        }
        current = current->next;
    }

    return NULL;  /* Not found */
}

/* Mark a symbol as initialized (called after assignment) */
void mark_initialized(SymbolTable* table, const char* name) {
    Symbol* symbol = lookup_symbol(table, name);
    if (symbol) {
        symbol->is_initialized = 1;
    }
}

/* Mark a symbol as initialized in a specific scope */
void mark_initialized_in_scope(SymbolTable* table, const char* name, const char* scope) {
    Symbol* symbol = lookup_symbol_in_scope(table, name, scope);
    if (symbol) {
        symbol->is_initialized = 1;
    }
}

/* Check if a symbol has been initialized */
int is_initialized(SymbolTable* table, const char* name) {
    Symbol* symbol = lookup_symbol(table, name);
    if (symbol) {
        return symbol->is_initialized;
    }
    return 0;  /* Symbol not found or not initialized */
}

/* Convert data type to string representation */
const char* type_to_string(DataType type) {
    switch (type) {
        case TYPE_INT:     return "int";
        case TYPE_VOID:    return "void";
        case TYPE_UNKNOWN: return "unknown";
        default:           return "invalid";
    }
}

/* Print the entire symbol table in a formatted way */
void print_symbol_table(SymbolTable* table) {
    printf("+============================================================+\n");
    printf("| %-20s %-10s %-12s %-10s |\n", "Variable", "Type", "Initialized", "Line");
    printf("+============================================================+\n");

    int count = 0;

    /* Iterate through all buckets */
    for (int i = 0; i < table->size; i++) {
        Symbol* current = table->table[i];

        /* Traverse the chain at this bucket */
        while (current != NULL) {
            printf("| %-20s %-10s %-12s %-10d |\n",
                   current->name,
                   type_to_string(current->type),
                   current->is_initialized ? "Yes" : "No",
                   current->declaration_line);
            count++;
            current = current->next;
        }
    }

    if (count == 0) {
        printf("| %-58s |\n", "(No symbols in table)");
    }

    printf("+============================================================+\n");
    printf("Total symbols: %d\n", table->num_symbols);
}

/* Free all memory used by the symbol table */
void free_symbol_table(SymbolTable* table) {
    if (!table) return;

    /* Free all symbols in all buckets */
    for (int i = 0; i < table->size; i++) {
        Symbol* current = table->table[i];

        while (current != NULL) {
            Symbol* next = current->next;
            free(current->name);  /* Free the duplicated name string */
            free(current);        /* Free the symbol structure */
            current = next;
        }
    }

    /* Free the table array and the table structure itself */
    free(table->table);
    free(table);
}

/* SCOPE MANAGEMENT FUNCTIONS */

/* Add a symbol with specific scope */
int add_symbol_with_scope(SymbolTable* table, const char* name, DataType type, int line, const char* scope) {
    /* For scoped lookup, we need to check only in the current scope */
    Symbol* existing = lookup_symbol_in_scope(table, name, scope);
    if (existing && strcmp(existing->scope, scope) == 0) {
        return 0;  /* Symbol already exists in this scope */
    }

    /* Calculate hash index */
    unsigned int index = hash(name, table->size);

    /* Create new symbol */
    Symbol* new_symbol = (Symbol*)malloc(sizeof(Symbol));
    if (!new_symbol) {
        fprintf(stderr, "Fatal Error: Failed to allocate symbol\n");
        exit(1);
    }

    new_symbol->name = strdup(name);
    new_symbol->kind = SYMBOL_VARIABLE;
    new_symbol->type = type;
    new_symbol->is_initialized = 0;
    new_symbol->is_array = 0;
    new_symbol->array_size = 0;
    new_symbol->return_type = TYPE_UNKNOWN;
    new_symbol->param_count = 0;
    new_symbol->param_types = NULL;
    new_symbol->param_names = NULL;
    new_symbol->scope = strdup(scope);  /* Set scope */
    new_symbol->declaration_line = line;
    new_symbol->next = NULL;

    /* Insert at the beginning of the chain */
    if (table->table[index] == NULL) {
        table->table[index] = new_symbol;
    } else {
        new_symbol->next = table->table[index];
        table->table[index] = new_symbol;
    }

    table->num_symbols++;
    return 1;  /* Success */
}

/* Lookup symbol in specific scope first, then global */
Symbol* lookup_symbol_in_scope(SymbolTable* table, const char* name, const char* current_scope) {
    if (!table || !name) return NULL;

    unsigned int index = hash(name, table->size);
    Symbol* current = table->table[index];

    /* First, search in the current scope */
    while (current != NULL) {
        if (strcmp(current->name, name) == 0 &&
            current->scope && strcmp(current->scope, current_scope) == 0) {
            return current;
        }
        current = current->next;
    }

    /* If not found in current scope, search in global scope */
    current = table->table[index];
    while (current != NULL) {
        if (strcmp(current->name, name) == 0 &&
            current->scope && strcmp(current->scope, "global") == 0) {
            return current;
        }
        current = current->next;
    }

    return NULL;  /* Symbol not found */
}

/* Add function parameter to symbol table */
int add_parameter(SymbolTable* table, const char* name, DataType type, int line, const char* function_scope) {
    return add_symbol_with_scope(table, name, type, line, function_scope);
}
