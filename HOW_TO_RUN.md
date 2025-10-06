# How to Run the CST-405 Compiler

## Complete Step-by-Step Guide

---

## PREREQUISITES

### Required Software

1. **GCC (GNU Compiler Collection)**
   - Linux: Usually pre-installed
   - Windows: Install MinGW-w64 or use WSL (Windows Subsystem for Linux)
   - Mac: Install Xcode Command Line Tools

2. **Flex (Lexical Analyzer Generator)**
   - Linux: `sudo apt-get install flex`
   - Mac: `brew install flex`
   - Windows (WSL): `sudo apt-get install flex`

3. **Bison (Parser Generator)**
   - Linux: `sudo apt-get install bison`
   - Mac: `brew install bison`
   - Windows (WSL): `sudo apt-get install bison`

4. **NASM (Netwide Assembler)** - For assembling output
   - Linux: `sudo apt-get install nasm`
   - Mac: `brew install nasm`
   - Windows (WSL): `sudo apt-get install nasm`

5. **Make (Build Tool)**
   - Usually comes with GCC
   - Linux: `sudo apt-get install make`

### Verify Installation

Run these commands to verify everything is installed:

```bash
gcc --version      # Should show GCC version
flex --version     # Should show Flex version
bison --version    # Should show Bison version
nasm --version     # Should show NASM version
make --version     # Should show Make version
```

---

## SETUP

### 1. Clone the Repository

```bash
cd /path/to/your/workspace
git clone https://github.com/aimemoria/CST-405.git
cd CST-405
```

### 2. Verify Files

Make sure you have all these files:

```bash
ls -la
```

You should see:
- `scanner_new.l` - Lexer
- `parser.y` - Parser
- `ast.c`, `ast.h` - AST module
- `symtable.c`, `symtable.h` - Symbol table
- `semantic.c`, `semantic.h` - Semantic analyzer
- `ircode.c`, `ircode.h` - IR generator
- `codegen.c`, `codegen.h` - Code generator
- `compiler.c` - Main driver
- `Makefile_new` - Build file
- `test_basic.src`, `test_while.src`, `test_complex.src` - Test programs

---

## BUILDING THE COMPILER

### Method 1: Using Make (Recommended)

```bash
# Build the complete compiler
make -f Makefile_new

# You should see compilation messages, then:
# ✓ Compiler built successfully: compiler
```

### Method 2: Manual Build (If Make doesn't work)

```bash
# Step 1: Generate parser
bison -d -v parser.y

# Step 2: Generate lexer
flex scanner_new.l

# Step 3: Compile all modules
gcc -Wall -g -c parser.tab.c
gcc -Wall -g -c lex.yy.c
gcc -Wall -g -c ast.c
gcc -Wall -g -c symtable.c
gcc -Wall -g -c semantic.c
gcc -Wall -g -c ircode.c
gcc -Wall -g -c codegen.c

# Step 4: Link everything
gcc -Wall -g -o compiler parser.tab.o lex.yy.o ast.o symtable.o semantic.o ircode.o codegen.o
```

### Verify Build

```bash
ls -la compiler

# Should show the compiler executable
# On Windows it will be compiler.exe
```

---

## RUNNING THE COMPILER

### Basic Usage

```bash
./compiler <input_file>
```

The compiler will:
1. Parse the input file
2. Perform all compilation phases
3. Generate `output.asm` assembly file

### Example 1: Compile Basic Test

```bash
./compiler test_basic.src
```

**Expected Output:**
- Phase reports for lexical, syntax, semantic analysis
- Abstract Syntax Tree display
- Symbol Table display
- Three-Address Code listing
- Assembly code generation confirmation

### Example 2: Compile While Loop Test (NEW FEATURE)

```bash
./compiler test_while.src
```

**Expected Output:**
- Same as above, but TAC shows loop control flow with labels

### Example 3: Compile Complex Test

```bash
./compiler test_complex.src
```

**Expected Output:**
- Multiple loops with different labels (L0/L1, L2/L3, etc.)

---

## VIEWING GENERATED ASSEMBLY

```bash
cat output.asm
```

or use a text editor:

```bash
nano output.asm
# or
code output.asm
```

You'll see:
- `.data` section with printf format string
- `.bss` section with variable declarations
- `.text` section with executable code

---

## ASSEMBLING AND RUNNING (Linux/WSL Only)

### Step 1: Assemble to Object File

```bash
nasm -f elf64 output.asm -o output.o
```

This creates `output.o` (object file)

### Step 2: Link to Executable

```bash
gcc output.o -o program -no-pie
```

This creates `program` (executable)

### Step 3: Run the Program

```bash
./program
```

**For test_basic.src:** Prints three numbers (30, 10, 20)

**For test_while.src:** Prints 0, 1, 2, 3, 4, then 10 (sum)

**For test_complex.src:** Prints 15 (3*5), then countdown from 10

---

## RUNNING ALL TESTS AUTOMATICALLY

### Using Make Targets

```bash
# Test basic features
make -f Makefile_new test-basic

# Test while loops
make -f Makefile_new test-while

# Test complex program
make -f Makefile_new test-complex

# Run all tests
make -f Makefile_new test-all
```

---

## CREATING YOUR OWN PROGRAMS

### Language Syntax

```c
// Variable declarations
int variable_name;

// Assignments
variable_name = expression;

// Print statements
print(expression);

// While loops (NEW FEATURE)
while (condition) {
    statements;
}

// Expressions
variable_name + other_variable
variable_name + 123

// Conditions (for while loops)
variable1 < variable2
variable1 > variable2
variable1 <= variable2
variable1 >= variable2
variable1 == variable2
variable1 != variable2
```

### Example Program

Create a file `myprogram.src`:

```c
int x;
int y;

x = 5;
y = 0;

while (y < x) {
    print(y);
    y = y + 1;
}

print(x);
```

### Compile It

```bash
./compiler myprogram.src
```

---

## TROUBLESHOOTING

### Problem: "command not found: flex"

**Solution:** Install Flex
```bash
sudo apt-get install flex
```

### Problem: "command not found: bison"

**Solution:** Install Bison
```bash
sudo apt-get install bison
```

### Problem: "parser.y:XX: syntax error"

**Solution:** Make sure Bison version is 3.0 or higher
```bash
bison --version
```

### Problem: "undefined reference to yylex"

**Solution:** Make sure scanner_new.l is compiled after parser.y
```bash
# Rebuild in correct order
make -f Makefile_new clean
make -f Makefile_new
```

### Problem: Compiler builds but crashes on run

**Solution:** Check input file format
- Must have proper syntax
- Must end with newline
- Use Unix line endings (LF, not CRLF)

### Problem: "No such file or directory: output.asm"

**Solution:** Compilation failed - check error messages above
- Look for "SEMANTIC ERROR" or "SYNTAX ERROR"
- Fix the source program and recompile

### Problem: NASM errors when assembling

**Solution:** Make sure you're using 64-bit format
```bash
nasm -f elf64 output.asm -o output.o
```

For 32-bit systems, use:
```bash
nasm -f elf32 output.asm -o output.o
gcc -m32 output.o -o program -no-pie
```

---

## CLEANING UP

### Remove Generated Files

```bash
make -f Makefile_new clean
```

This removes:
- Object files (*.o)
- Generated lexer and parser (lex.yy.c, parser.tab.c, parser.tab.h)
- Compiler executable
- Assembly output (output.asm)
- Compiled program

### Deep Clean

```bash
make -f Makefile_new distclean
```

This also removes backup files (*~, *.bak)

---

## QUICK REFERENCE

### Complete Workflow

```bash
# 1. Build compiler
make -f Makefile_new

# 2. Compile source program
./compiler test_while.src

# 3. View generated assembly (optional)
cat output.asm

# 4. Assemble to object file (Linux/WSL)
nasm -f elf64 output.asm -o output.o

# 5. Link to executable
gcc output.o -o program -no-pie

# 6. Run the program
./program
```

### One-Line Test

```bash
make -f Makefile_new && ./compiler test_basic.src && nasm -f elf64 output.asm -o output.o && gcc output.o -o program -no-pie && ./program
```

---

## WINDOWS-SPECIFIC NOTES

### Using WSL (Windows Subsystem for Linux) - RECOMMENDED

1. Install WSL2: https://docs.microsoft.com/en-us/windows/wsl/install
2. Install Ubuntu from Microsoft Store
3. Open Ubuntu terminal
4. Install tools:
   ```bash
   sudo apt-get update
   sudo apt-get install gcc flex bison nasm make git
   ```
5. Follow Linux instructions above

### Using MinGW (Native Windows) - LIMITED

MinGW supports building the compiler but NOT running the generated assembly (which targets Linux).

1. Install MinGW-w64
2. Build the compiler: `mingw32-make -f Makefile_new`
3. Run compiler: `compiler.exe test_basic.src`
4. Assembly generation works, but cannot execute on Windows

**Recommendation:** Use WSL for full functionality

---

## MAC-SPECIFIC NOTES

### Install Prerequisites

```bash
# Install Homebrew if you don't have it
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install tools
brew install gcc flex bison nasm make
```

### Adjust Makefile (if needed)

Mac's default Bison might be old. Use Homebrew's version:

```bash
# Add to PATH
export PATH="/usr/local/opt/bison/bin:$PATH"
```

### Assembly Execution

Mac uses Mach-O format, not ELF. You'd need to:
1. Change assembly format in codegen.c
2. Use `nasm -f macho64 output.asm`
3. Link with: `gcc output.o -o program`

For this project, use Linux/WSL for simplest results.

---

## GETTING HELP

### Compiler Info

```bash
make -f Makefile_new info
```

Shows compiler structure and features.

### Make Targets

```bash
make -f Makefile_new help
```

Lists all available make targets.

### Check Build

```bash
file compiler
```

Should show: "ELF 64-bit LSB executable" (Linux)

---

## EXPECTED OUTPUT EXAMPLES

### test_basic.src Output

```
30
10
20
```

### test_while.src Output

```
0
1
2
3
4
10
```

### test_complex.src Output

```
15
10
9
8
7
6
5
4
3
2
1
10
```

---

## VIDEO DEMONSTRATION CHECKLIST

When recording your video, run these commands in order:

```bash
# 1. Show file structure
ls -la

# 2. Show a test program
cat test_while.src

# 3. Build compiler
make -f Makefile_new

# 4. Compile test program
./compiler test_while.src

# 5. Show generated assembly
head -n 30 output.asm

# 6. Show TAC (scroll up in compiler output)

# 7. Assemble and link
nasm -f elf64 output.asm -o output.o
gcc output.o -o program -no-pie

# 8. Run program
./program

# 9. Test error handling (create error file)
echo "int x; y = 10;" > test_error.src
./compiler test_error.src

# 10. Show success
echo "✓ Complete compiler demonstration!"
```

---

## SUMMARY

**To get started quickly:**

```bash
# Install prerequisites (Ubuntu/Debian)
sudo apt-get install gcc flex bison nasm make

# Clone and build
git clone https://github.com/aimemoria/CST-405.git
cd CST-405
make -f Makefile_new

# Test
./compiler test_while.src
nasm -f elf64 output.asm -o output.o
gcc output.o -o program -no-pie
./program
```

**You're done! The compiler works!** 🎉

---

For questions or issues, refer to README_COMPILER.md for detailed documentation.
