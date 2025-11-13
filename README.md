# C Compiler with Decision Control

A complete compiler implementation for a C-like programming language that generates x86-64 assembly code.

## Features
- Variables and arrays
- Mathematical expressions (+, -, *, /, %) with precedence
- Decision control structures (if, if-else, nested if)
- Boolean/relational operations (<, >, <=, >=, ==, !=)
- Control flow (while loops)
- Functions with parameters and return values
- Type checking and error reporting

## Build & Run

### Windows
```bash
build.bat
.\compiler.exe test_if.c
type output.asm
```

### Linux/macOS
```bash
make
./compiler test_program.c
nasm -f elf64 output.asm -o output.o
gcc output.o -o program -no-pie
./program
```

## Compiler Phases
1. **Lexical Analysis** (scanner_new.l) - Tokenization
2. **Syntax Analysis** (parser.y) - Parsing & AST generation
3. **Symbol Table** (symtable.c/h) - Variable/function tracking
4. **Semantic Analysis** (semantic.c/h) - Type checking
5. **IR Generation** (ircode.c/h) - Three-address code
6. **Code Generation** (codegen.c/h) - x86-64 assembly

## Test Programs
- test_if.c - Simple if statements
- test_if_else.c - If-else statements
- test_nested_if.c - Nested if statements
- test_if_while.c - Combined if and while loops
- test_program.c - Basic features
- test_math.c - Mathematical expressions
- test_arrays.c - Array operations
- test_functions.c - Function calls

## Language Example
```c
int add(int a, int b) {
    return a + b;
}

int x;
int y;
x = 5;
y = 10;
print(add(x, y));  // Output: 15
```

## Requirements
- GCC compiler
- Flex (lexical analyzer generator)
- Bison (parser generator)
- Make (for Linux/macOS) or use build.bat (for Windows)

## Course
CST-405 Compiler Design
