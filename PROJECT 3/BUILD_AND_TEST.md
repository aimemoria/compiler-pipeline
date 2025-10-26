# Build and Test Guide

## Prerequisites

You need these tools installed on Windows:

1. **GCC** - TDM-GCC-64 (✅ Already installed at C:\TDM-GCC-64\bin\gcc.exe)
2. **Flex** - GnuWin32 (✅ Already installed at C:\Program Files (x86)\GnuWin32\bin\flex.exe)
3. **Bison** - ❌ **NOT YET INSTALLED** - You need to install this!
4. **Make** - TDM-GCC (✅ Already installed at C:\TDM-GCC-64\bin\mingw32-make.exe)

## Installing Bison (Required!)

### Option 1: Win Flex-Bison (Recommended)
```bash
# Download from:
https://github.com/lexxmark/winflexbison/releases

# Download: win_flex_bison-latest.zip
# Extract to: C:\win_flex_bison\
# Add to PATH or copy win_bison.exe as bison.exe to C:\TDM-GCC-64\bin\
```

### Option 2: GnuWin32 Bison
```bash
# Download from:
http://gnuwin32.sourceforge.net/packages/bison.htm

# Install to: C:\Program Files (x86)\GnuWin32\
# Add C:\Program Files (x86)\GnuWin32\bin to PATH
```

### Option 3: Chocolatey (if you have it)
```bash
choco install winflexbison3
```

## Verify Installation

```bash
# Check all tools
where gcc
where flex
where bison      # or: where win_bison
where mingw32-make

# Test versions
gcc --version
flex --version
bison --version  # or: win_bison --version
```

---

## Building the Compiler

```bash
# Navigate to project directory
cd "F:\PROJECTS\CST 405\PROJECT\CST-405"

# Clean any old build
mingw32-make clean

# Build the compiler
mingw32-make

# You should see:
# - Generating lexer with Flex...
# - Generating parser with Bison...
# - Compiling [various modules]...
# - ✓ Compiler built successfully: compiler
```

### If Build Fails

**Error: "bison: command not found"**
- Solution: Install bison (see above)

**Error: "parser.y: syntax error"**
- Solution: This shouldn't happen with our grammar
- Check that you're using bison 3.x or later

**Error: "undefined reference"**
- Solution: Make sure all .c files are being compiled
- Try: `mingw32-make clean` then `mingw32-make` again

---

## Running the Compiler

### Test 1: Mathematical Expressions
```bash
./compiler test_math.c
```

**Expected Output:**
- Lexical analysis complete
- Parsing complete
- AST tree printed
- Semantic analysis complete
- TAC (Three-Address Code) printed
- Assembly code generated to `output.asm`

**What to check:**
- Look for: "Generated X TAC instructions"
- Look for: "Assembly code generated successfully"
- Check: output.asm file was created

### Test 2: Arrays
```bash
./compiler test_arrays.c
```

**Expected Output:**
- Similar to above
- TAC should show `ARRAY_LOAD` and `ARRAY_STORE` instructions
- Assembly should have array address calculations

### Test 3: While Loops
```bash
./compiler test_program.c
```

**Expected Output:**
- TAC should show `LABEL`, `GOTO`, `IF_FALSE` instructions
- Assembly should have conditional jumps

### Test 4: Functions (Parser Only)
```bash
./compiler test_functions.c
```

**Expected Output:**
- ✅ Parsing will succeed (grammar is complete)
- ✅ AST will show function nodes
- ❌ Semantic/IR/Codegen are NOT yet implemented for functions
- You'll see function declarations, definitions, and calls in the AST

---

## Examining the Output

### View the Generated Assembly
```bash
cat output.asm
# or
notepad output.asm
```

**What you'll see:**
```nasm
section .data
    fmt_int: db "%d", 10, 0

section .bss
    x: resq 1
    y: resq 1
    arr: resq 10  ; Arrays allocated with proper size
    t0: resq 1    ; Temporaries

section .text
    global main
    extern printf

main:
    push rbp
    mov rbp, rsp

    ; Your program's assembly code here
    ; - Variable assignments
    ; - Arithmetic operations
    ; - Array access
    ; - Loops
    ; - Function calls (when implemented)

    mov rsp, rbp
    pop rbp
    ret
```

### View the TAC (Intermediate Code)
The TAC is printed to console during compilation. Look for:
```
═══════════════ THREE-ADDRESS CODE (TAC) ══════════════════

Line  Opcode         Result     Op1        Op2
────────────────────────────────────────────────────────────
0     LOAD_CONST     x          5
1     LOAD_CONST     y          10
2     ADD            t0         x          y
3     PRINT          -          t0
...
```

---

## Running the Generated Program (Linux/WSL Only)

If you have Linux, WSL, or a Linux VM:

```bash
# Assemble to object file
nasm -f elf64 output.asm -o output.o

# Link with C library
gcc output.o -o program -no-pie

# Run!
./program
```

**Expected output** (for test_math.c):
```
13
7
30
3
1
14
20
18
40
```

---

## Testing Checklist

### ✅ What Should Work Now

- [ ] Build completes without errors
- [ ] `test_math.c` compiles and generates assembly
- [ ] `test_arrays.c` compiles and generates assembly
- [ ] `test_program.c` compiles and generates assembly
- [ ] `test_functions.c` parses (but doesn't generate correct runtime code yet)
- [ ] output.asm is generated
- [ ] TAC instructions are printed
- [ ] No segmentation faults

### ⚠️ Known Limitations

- Functions parse but don't generate correct runtime code yet
- No optimization passes
- Only `int` type supported
- No function return values in expressions (yet)
- No local variable scopes (yet)

---

## Troubleshooting

### "Parser conflicts"
- Bison may report shift/reduce or reduce/reduce conflicts
- As long as it generates parser.tab.c, it's usually okay
- Check parser.output file for details

### "Segmentation fault"
- Usually means parser/AST construction has an issue
- Run with gdb: `gdb ./compiler`
- Set breakpoint: `break main`
- Run: `run test_math.c`

### "Assembly file is empty"
- Check that all compiler phases completed
- Look for error messages in console output
- Verify symbol table was populated

### "Can't execute output.asm"
- .asm files are assembly source, not executables
- You need NASM and GCC to assemble and link (Linux only)
- Windows can't directly run x86-64 Linux assembly

---

## Next Steps

After successful testing:

1. **Verify all test cases pass**
2. **Review the generated assembly** - learn how your code translates!
3. **Complete function runtime** (symbol table, semantic, IR, codegen)
4. **Add more test cases**
5. **Consider optimizations**

---

## Success Criteria

You've successfully built the compiler when:

✅ `mingw32-make` completes without errors
✅ `./compiler test_math.c` generates output.asm
✅ output.asm contains valid x86-64 assembly
✅ TAC instructions are printed correctly
✅ No crashes or segmentation faults
✅ All phases (lexer→parser→semantic→IR→codegen) complete

**Congratulations! You have a working compiler!** 🎉

---

*For questions or issues, refer to COMPILER_IMPLEMENTATION_COMPLETE.md*
