# CST-301/302 Lexical Analyzer Makefile
# This version builds the standalone lexer for project requirements

CC = gcc
LEX = flex
CFLAGS = -g -Wall -Wextra
LEXER_TARGET = lexer
COMPILER_TARGET = minicompiler

# Default target - standalone lexer for project submission
all: $(LEXER_TARGET)

# Standalone lexer (with commented returns) - for project demonstration
$(LEXER_TARGET): scanner.l
	@echo "Building standalone lexical analyzer..."
	$(LEX) scanner.l
	$(CC) $(CFLAGS) -o $(LEXER_TARGET) lex.yy.c
	@echo "✓ Standalone lexer built successfully!"

# Full compiler (uncomment returns in scanner.l first)
$(COMPILER_TARGET): lex.yy.o parser.tab.o main.o ast.o symtab.o codegen.o tac.o
	@echo "Building full compiler..."
	$(CC) $(CFLAGS) -o $(COMPILER_TARGET) $^
	@echo "✓ Full compiler built successfully!"

# Generate lexer source code from Flex
lex.yy.c: scanner.l parser.tab.h
	$(LEX) scanner.l

# Generate parser from Bison
parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

# Object file rules
lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

parser.tab.o: parser.tab.c
	$(CC) $(CFLAGS) -c parser.tab.c

main.o: main.c ast.h
	$(CC) $(CFLAGS) -c main.c

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

symtab.o: symtab.c symtab.h
	$(CC) $(CFLAGS) -c symtab.c

codegen.o: codegen.c codegen.h ast.h symtab.h
	$(CC) $(CFLAGS) -c codegen.c

tac.o: tac.c tac.h ast.h
	$(CC) $(CFLAGS) -c tac.c

# Test targets for project demonstration
test: $(LEXER_TARGET)
	@echo "\n=== Testing Lexical Analyzer ==="
	@echo "Testing with valid input (test.c):"
	@echo "=================================="
	./$(LEXER_TARGET) test.c

test-stdin: $(LEXER_TARGET)
	@echo "\n=== Testing from Standard Input ==="
	@echo "Type your code below (Ctrl+D to end):"
	@echo "===================================="
	./$(LEXER_TARGET)

test-errors: $(LEXER_TARGET)
	@echo "\n=== Testing Error Handling ==="
	@echo "Testing with error file (test_errors.c):"
	@echo "========================================"
	./$(LEXER_TARGET) test_errors.c

# Demonstration commands for video recording
demo: $(LEXER_TARGET)
	@echo "\n" " DEMO: CST-301/302 Lexical Analyzer"
	@echo "=" * 50
	@echo "1. Testing valid source code:"
	@echo "-" * 30
	./$(LEXER_TARGET) test.c
	@echo "\n2. Testing error detection:"
	@echo "-" * 30
	-./$(LEXER_TARGET) test_errors.c || true
	@echo "\n3. Interactive testing:"
	@echo "-" * 30
	@echo "Enter: int x; x = 42; print(x);"
	@echo "int x; x = 42; print(x);" | ./$(LEXER_TARGET)

# Installation verification commands for documentation
verify-install:
	@echo "Verifying Flex and Bison installation:"
	@echo "======================================"
	@echo "Flex version:"
	flex --version || echo "❌ Flex not installed"
	@echo "\nBison version:"
	bison --version || echo "❌ Bison not installed"
	@echo "\nGCC version:"
	gcc --version || echo "❌ GCC not installed"
	@echo "\nSystem information:"
	uname -a

# Clean up generated files
clean:
	@echo "Cleaning up generated files..."
	rm -f $(LEXER_TARGET) $(COMPILER_TARGET)
	rm -f lex.yy.c lex.yy.o
	rm -f parser.tab.c parser.tab.h parser.tab.o
	rm -f *.o *.s
	@echo "✓ Cleanup complete"

# Help target
help:
	@echo "Availabl
