# CST-405 Compiler Demonstration - Video Script

## Total Duration: 10-12 minutes

---

## SECTION 1: INTRODUCTION (1 minute)

**[Screen: Show desktop with project folder]**

**Script:**
"Hello! Today I'm demonstrating my complete compiler implementation for CST-405. This is a fully functional compiler that translates a simple programming language into executable x86-64 assembly code.

The compiler implements all five major compilation phases:
- Lexical Analysis
- Syntax Analysis
- Semantic Analysis
- Intermediate Code Generation
- And Assembly Code Generation

I've also added a significant new feature—while loops with relational operators—that required implementation across every phase of the compiler. Let's dive in!"

---

## SECTION 2: CODE ARCHITECTURE OVERVIEW (2 minutes)

**[Screen: Open project folder, show file structure]**

**Script:**
"First, let me show you the architecture. The compiler consists of several modular components:

**[Point to each file as you mention it]**

1. **scanner_new.l** - The Flex lexical analyzer that tokenizes the source code
2. **parser.y** - The Bison parser that performs syntax analysis and builds the AST
3. **ast.c and ast.h** - The Abstract Syntax Tree data structure
4. **symtable.c and symtable.h** - Symbol table for tracking variables
5. **semantic.c and semantic.h** - Semantic analyzer for type checking
6. **ircode.c and ircode.h** - Intermediate code generator producing Three-Address Code
7. **codegen.c and codegen.h** - Final assembly code generator
8. **compiler.c** - The main driver that orchestrates all phases

**[Open scanner_new.l]**

Looking at the lexer, you can see I've added support for the while keyword, relational operators like less-than, greater-than, equals, and the curly braces needed for loop bodies. This is part of my new feature.

**[Open parser.y, scroll to while_stmt rule]**

In the parser grammar, here's the while loop rule I added. It expects the while keyword, a condition in parentheses, and a statement block in braces. This builds an AST node specifically for while loops.

**[Open ast.h, show NODE_WHILE]**

The AST defines all our language constructs as node types. You can see NODE_WHILE here, which is my new addition for the while loop feature."

---

## SECTION 3: ASSIGNED TASKS COMPLETED (2 minutes)

**[Screen: Open README_COMPILER.md or show checklist]**

**Script:**
"Let me outline the individually assigned tasks I completed for this project:

**Phase 1 - Lexical Analysis:**
- Extended the lexer to recognize all tokens including the new while keyword
- Added support for six relational operators: <, >, <=, >=, ==, !=
- Added brace tokens for block structures
- Maintained accurate line and column tracking for error reporting

**Phase 2 - Syntax Analysis:**
- Designed and implemented the complete parser grammar using Bison
- Added grammar rules for while loops and conditions
- Built the Abstract Syntax Tree during parsing
- Integrated early symbol table population during parsing

**Phase 3 - Semantic Analysis:**
- Implemented comprehensive type checking
- Added variable declaration verification
- Implemented use-before-initialization detection
- Added condition type validation for while loops

**Phase 4 - Intermediate Code Generation:**
- Designed and implemented Three-Address Code representation
- Created control flow instructions: LABEL, GOTO, IF_FALSE
- Implemented automatic temporary variable generation
- Generated loop control flow with proper label management

**Phase 5 - Code Generation:**
- Implemented x86-64 assembly code generation
- Added support for conditional jumps and labels
- Implemented relational operations using CMP and SETcc instructions
- Generated complete executable programs with proper calling conventions

**New Feature - While Loops:**
This is the significant feature I added. It required work in EVERY phase:
- Lexer: New tokens
- Parser: New grammar rules
- AST: New node types
- Semantic: Condition validation
- IR: Control flow generation
- Codegen: Assembly for loops

Plus extensive documentation and three comprehensive test programs."

---

## SECTION 4: AST TO IR TO ASSEMBLY LOGIC (3 minutes)

**[Screen: Draw diagram or open code side-by-side]**

**Script:**
"Now let me explain the critical logic behind converting the AST to intermediate code and then to assembly. This is the heart of the compiler.

**STEP 1: AST Structure**

**[Open test_while.src in editor]**

Let's use this while loop as an example:
```
while (counter < limit) {
    print(counter);
    counter = counter + 1;
}
```

**[Draw or show AST diagram]**

The parser builds this into an AST with a WHILE node at the top. The WHILE node has two children:
- A CONDITION node (counter < limit)
- A STATEMENT_LIST node (the loop body)

The condition node contains a less-than operator with two ID nodes for 'counter' and 'limit'.

**STEP 2: AST to Intermediate Code**

**[Open ircode.c, find gen_statement WHILE case]**

Here's where the magic happens. When we traverse to a WHILE node during IR generation, we execute this code:

1. First, we generate a start label - this is where the loop begins
2. We recursively generate code for the condition expression
3. The condition generates a RELOP instruction that compares counter and limit, storing the result in a temporary variable
4. We generate an IF_FALSE instruction - if the condition is false (zero), jump to the end label
5. We recursively generate code for the loop body
6. We generate a GOTO back to the start label
7. Finally, we place the end label

**[Open compiler and show TAC output example]**

This produces Three-Address Code like:
```
L0:                    // start label
  t0 = counter < limit // condition evaluation
  if_false t0 goto L1  // exit if false
  print(counter)       // body
  t1 = counter + 1
  counter = t1
  goto L0              // loop back
L1:                    // end label
```

This TAC is a linear, simplified representation that's easy to translate to assembly.

**STEP 3: TAC to Assembly**

**[Open codegen.c, find gen_tac_instruction]**

The code generator walks through each TAC instruction and emits corresponding assembly:

- LABEL instructions become assembly labels
- RELOP uses CMP to compare, then SETcc instructions to set the result
- IF_FALSE uses CMP and JE (jump if equal to zero)
- GOTO becomes JMP
- Arithmetic and assignments use MOV and ADD instructions

**[Show assembly output example]**

The final assembly looks like:
```asm
L0:
    mov rax, [counter]
    cmp rax, [limit]
    setl al              ; set if less
    movzx rax, al
    mov [t0], rax

    mov rax, [t0]
    cmp rax, 0
    je L1                ; jump if false

    ; loop body...
    jmp L0
L1:
```

This is executable x86-64 assembly that can be assembled with NASM and linked with GCC.

The key insight is this three-level transformation:
- AST captures program structure hierarchically
- TAC linearizes it with explicit control flow
- Assembly provides machine-executable instructions

Each level is simpler to generate from the previous one."

---

## SECTION 5: BUILD AND EXECUTION DEMONSTRATION (3-4 minutes)

**[Screen: Terminal in project directory]**

**Script:**
"Now let's see it in action. I'll demonstrate the complete workflow from source code to running executable.

**Building the Compiler:**

**[Type command]**
```bash
make -f Makefile_new
```

**[Wait for build]**

You can see the Makefile building all components:
- Bison generates the parser from parser.y
- Flex generates the lexer from scanner_new.l
- GCC compiles all the C modules
- Everything links together into the compiler executable

**Compiling a Basic Program:**

**[Show test_basic.src]**

Here's a simple test program with declarations, assignments, and arithmetic. Let's compile it:

**[Type command]**
```bash
./compiler test_basic.src
```

**[Let it run, scroll through output]**

Look at the output! The compiler shows:
- Lexical and syntax analysis succeeded
- Semantic analysis with no errors
- The complete Abstract Syntax Tree structure
- The Symbol Table with all variables
- The generated Three-Address Code
- Confirmation that assembly was generated

**[Show output.asm]**
```bash
cat output.asm
```

Here's the generated assembly code - complete with data section, BSS for variables, and the executable code in the text section.

**Compiling the While Loop Feature:**

**[Show test_while.src]**

Now let's test my new feature - while loops:

**[Type command]**
```bash
./compiler test_while.src
```

**[Point to output]**

Perfect! Notice how the TAC shows the loop structure with labels L0 and L1, the condition evaluation, and the conditional jump. This demonstrates the while loop feature working through all compiler phases.

**Assembling and Running:**

**[Note: This requires Linux/WSL - adapt if needed]**

Let's turn this assembly into a running program:

**[Type commands]**
```bash
nasm -f elf64 output.asm -o output.o
gcc output.o -o program -no-pie
./program
```

**[Show output]**

Excellent! The program executes correctly. It counts from 0 to 4 and then prints the sum of 10, exactly as expected.

**Testing Complex Features:**

**[Type command]**
```bash
./compiler test_complex.src
```

This complex test combines multiple features including nested expressions and different relational operators.

**[Show TAC output with multiple labels]**

You can see multiple loop structures with different labels - L0/L1 for the first loop, L2/L3 for the second loop. The compiler correctly generates distinct labels for each control flow structure.

**Error Handling Demo:**

**[Create error file or show error example]**

Let me show error handling. I'll create a file with an undeclared variable:

**[Type in editor]**
```
int x;
y = 10;
print(y);
```

**[Save as test_error.src and compile]**
```bash
./compiler test_error.src
```

**[Point to error]**

The semantic analyzer catches the error immediately: 'Variable y used before declaration' with the exact line number. The compiler fails gracefully and reports the problem clearly."

---

## SECTION 6: CODE WALKTHROUGH (2 minutes)

**[Screen: Open key source files]**

**Script:**
"Let me quickly walk through some key implementation details:

**Symbol Table:**

**[Open symtable.c]**

The symbol table uses a hash table for O(1) lookup. Here's the hash function using the djb2 algorithm. Variables are stored with their type, declaration line, and initialization status.

**AST Construction:**

**[Open ast.c, show create functions]**

Each AST node is created with helper functions. For example, create_while_node takes a condition and body, allocates memory, and links them together. Notice the extensive comments explaining each step.

**Semantic Analysis:**

**[Open semantic.c, show analyze_statement]**

The semantic analyzer traverses the AST recursively. For a while loop, it validates the condition type and then analyzes the body. For assignments, it checks variable declaration and marks variables as initialized.

**Code Generation:**

**[Open codegen.c, show gen_tac_instruction for RELOP]**

For relational operations, the code generator uses CMP to compare values, then SETcc instructions - SETL for less-than, SETE for equals, etc. This produces a 0 or 1 result that IF_FALSE can test."

---

## SECTION 7: CONCLUSION (1 minute)

**[Screen: Show project summary or README]**

**Script:**
"In summary, I've built a complete, fully functional compiler with:

✓ All five compilation phases implemented
✓ A significant new feature (while loops) integrated across every phase
✓ Over 2400 lines of well-documented, commented code
✓ Comprehensive error handling and reporting
✓ Three detailed test programs
✓ Complete build system and documentation

The compiler successfully transforms high-level source code through lexical analysis, parsing, semantic checking, intermediate representation, and finally into executable x86-64 assembly.

This project demonstrates deep understanding of compiler architecture, language theory, code generation, and software engineering practices.

Thank you for watching!"

**[End recording]**

---

# END OF SCRIPT

**Total Words:** ~2000
**Estimated Duration:** 10-12 minutes at natural speaking pace
**Sections:** 7 clearly defined segments
