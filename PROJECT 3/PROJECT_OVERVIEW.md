# Project Overview

## CST-405 Compiler Implementation

This project implements a complete compiler for a C-like programming language, translating source code through all compilation phases to executable x86-64 assembly code.

### Core Components

**Lexical Analyzer (scanner_new.l)**
Tokenizes source code using Flex, recognizing keywords (int, void, while, return, print), operators (+, -, *, /, %, =, <, >, <=, >=, ==, !=), identifiers, numbers, and punctuation. Tracks line numbers for error reporting.

**Syntax Analyzer (parser.y)**
Built with Bison, the parser implements a complete grammar supporting variable declarations, array declarations, assignments, expressions with proper operator precedence (PEMDAS), while loops, function declarations/definitions/calls, and return statements. Constructs an Abstract Syntax Tree (AST) for semantic analysis.

**Symbol Table (symtable.c/h)**
Hash table implementation tracking variables, arrays, and functions with their types, sizes, parameters, and return types. Handles collision resolution via chaining and provides O(1) average lookup time.

**Semantic Analyzer (semantic.c/h)**
Validates program correctness by checking: variable declaration before use, initialization before use, type compatibility in expressions and assignments, array index validity, function signature matching (parameter count and types), and return type consistency.

**IR Generator (ircode.c/h)**
Generates Three-Address Code (TAC) as an intermediate representation. Supports 18 TAC instructions including arithmetic operations, assignments, control flow (labels, jumps, conditional jumps), array operations, and function operations (calls, parameters, returns).

**Code Generator (codegen.c/h)**
Translates TAC to x86-64 NASM assembly following System V AMD64 calling convention. Manages variable storage in BSS section, implements arithmetic and relational operations, handles control flow with labels and jumps, and generates function prologues/epilogues with proper stack management.

### Language Features
- Integer variables and arrays
- Arithmetic operators with precedence
- Relational operators for comparisons
- While loops for iteration
- Functions with multiple parameters
- Return statements
- Print statements for output

### Project Achievement
Successfully implements all required compiler phases with comprehensive error handling, producing working executables from source code. Demonstrates mastery of compiler design principles and implementation techniques.
