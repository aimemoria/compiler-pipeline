# Makefile for CST-405 Complete Compiler
# This Makefile builds all phases of the compiler

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g
LEX = flex
YACC = bison
YFLAGS = -d -v

# Target executable
TARGET = compiler

# Source files
LEX_SRC = scanner_new.l
YACC_SRC = parser.y
C_SOURCES = compiler.c ast.c symtable.c semantic.c ircode.c codegen.c
OBJECTS = compiler.o parser.tab.o lex.yy.o ast.o symtable.o semantic.o ircode.o codegen.o

# Generated files
LEX_OUTPUT = lex.yy.c
YACC_OUTPUT = parser.tab.c parser.tab.h
YACC_REPORT = parser.output

# Test files
TEST_BASIC = test_basic.src
TEST_WHILE = test_while.src
TEST_COMPLEX = test_complex.src

# ============================================================
# DEFAULT TARGET
# ============================================================

all: $(TARGET)

# ============================================================
# BUILD RULES
# ============================================================

# Build the complete compiler
$(TARGET): $(OBJECTS)
	@echo "════════════════════════════════════════════════════"
	@echo "Linking compiler..."
	@echo "════════════════════════════════════════════════════"
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)
	@echo ""
	@echo "✓ Compiler built successfully: $(TARGET)"
	@echo ""

# Generate parser from Bison grammar
parser.tab.c parser.tab.h: $(YACC_SRC)
	@echo "Generating parser with Bison..."
	$(YACC) $(YFLAGS) $(YACC_SRC)
	@echo "✓ Parser generated"

# Generate lexer from Flex specification
lex.yy.c: $(LEX_SRC) parser.tab.h
	@echo "Generating lexer with Flex..."
	$(LEX) $(LEX_SRC)
	@echo "✓ Lexer generated"

# Compile parser
parser.tab.o: parser.tab.c
	@echo "Compiling parser..."
	$(CC) $(CFLAGS) -c parser.tab.c

# Compile lexer
lex.yy.o: lex.yy.c
	@echo "Compiling lexer..."
	$(CC) $(CFLAGS) -c lex.yy.c

# Compile AST module
ast.o: ast.c ast.h
	@echo "Compiling AST module..."
	$(CC) $(CFLAGS) -c ast.c

# Compile symbol table module
symtable.o: symtable.c symtable.h
	@echo "Compiling symbol table module..."
	$(CC) $(CFLAGS) -c symtable.c

# Compile semantic analyzer
semantic.o: semantic.c semantic.h ast.h symtable.h
	@echo "Compiling semantic analyzer..."
	$(CC) $(CFLAGS) -c semantic.c

# Compile intermediate code generator
ircode.o: ircode.c ircode.h ast.h symtable.h
	@echo "Compiling IR code generator..."
	$(CC) $(CFLAGS) -c ircode.c

# Compile code generator
codegen.o: codegen.c codegen.h ircode.h symtable.h
	@echo "Compiling code generator..."
	$(CC) $(CFLAGS) -c codegen.c

# Compile main compiler driver
compiler.o: compiler.c ast.h symtable.h semantic.h ircode.h codegen.h
	@echo "Compiling main compiler driver..."
	$(CC) $(CFLAGS) -c compiler.c

# ============================================================
# TEST TARGETS
# ============================================================

# Test with basic program
test-basic: $(TARGET)
	@echo ""
	@echo "════════════════════════════════════════════════════"
	@echo "Running compiler on basic test program..."
	@echo "════════════════════════════════════════════════════"
	@echo ""
	./$(TARGET) $(TEST_BASIC)

# Test with while loop program
test-while: $(TARGET)
	@echo ""
	@echo "════════════════════════════════════════════════════"
	@echo "Running compiler on while loop test..."
	@echo "════════════════════════════════════════════════════"
	@echo ""
	./$(TARGET) $(TEST_WHILE)

# Test with complex program
test-complex: $(TARGET)
	@echo ""
	@echo "════════════════════════════════════════════════════"
	@echo "Running compiler on complex test program..."
	@echo "════════════════════════════════════════════════════"
	@echo ""
	./$(TARGET) $(TEST_COMPLEX)

# Run all tests
test-all: test-basic test-while test-complex
	@echo ""
	@echo "════════════════════════════════════════════════════"
	@echo "✓ All tests completed"
	@echo "════════════════════════════════════════════════════"

# ============================================================
# ASSEMBLY AND EXECUTION
# ============================================================

# Assemble and run the generated code (Linux only)
run: $(TARGET)
	@echo "Compiling source program..."
	./$(TARGET) $(TEST_BASIC)
	@echo ""
	@echo "Assembling generated code..."
	nasm -f elf64 output.asm -o output.o
	@echo ""
	@echo "Linking executable..."
	gcc output.o -o program -no-pie
	@echo ""
	@echo "Running program..."
	@echo "════════════════════════════════════════════════════"
	./program
	@echo "════════════════════════════════════════════════════"

# ============================================================
# UTILITY TARGETS
# ============================================================

# Clean all generated files
clean:
	@echo "Cleaning generated files..."
	rm -f $(TARGET) $(OBJECTS) $(LEX_OUTPUT) $(YACC_OUTPUT) $(YACC_REPORT)
	rm -f output.asm output.o program
	@echo "✓ Clean complete"

# Deep clean (including backup files)
distclean: clean
	@echo "Deep cleaning..."
	rm -f *~ *.bak
	@echo "✓ Deep clean complete"

# Show compiler information
info:
	@echo ""
	@echo "════════════════════════════════════════════════════"
	@echo "CST-405 Complete Compiler System"
	@echo "════════════════════════════════════════════════════"
	@echo "Target:        $(TARGET)"
	@echo "Lexer:         $(LEX_SRC)"
	@echo "Parser:        $(YACC_SRC)"
	@echo "C Sources:     $(C_SOURCES)"
	@echo ""
	@echo "Compiler Phases:"
	@echo "  1. Lexical Analysis (Flex)"
	@echo "  2. Syntax Analysis (Bison)"
	@echo "  3. Semantic Analysis"
	@echo "  4. Intermediate Code Generation (TAC)"
	@echo "  5. Assembly Code Generation (x86-64)"
	@echo ""
	@echo "New Feature: While loops with relational operators"
	@echo "════════════════════════════════════════════════════"
	@echo ""

# Show available targets
help:
	@echo ""
	@echo "Available targets:"
	@echo "  make               - Build the compiler"
	@echo "  make test-basic    - Test with basic program"
	@echo "  make test-while    - Test with while loop"
	@echo "  make test-complex  - Test with complex program"
	@echo "  make test-all      - Run all tests"
	@echo "  make run           - Build, assemble, and run (Linux)"
	@echo "  make clean         - Remove generated files"
	@echo "  make distclean     - Remove all generated files"
	@echo "  make info          - Show compiler information"
	@echo "  make help          - Show this help message"
	@echo ""

# ============================================================
# PHONY TARGETS
# ============================================================

.PHONY: all clean distclean test-basic test-while test-complex test-all run info help
