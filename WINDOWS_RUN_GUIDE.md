# How to Run CST-405 Compiler on Windows

## Your Project Location
```
F:\PROJECTS\CST 405\PROJECT\CST-405
```

---

## OPTION 1: Using WSL (RECOMMENDED - Full Functionality)

### Step 1: Install WSL

1. Open PowerShell as Administrator
2. Run:
   ```powershell
   wsl --install
   ```
3. Restart your computer
4. Open "Ubuntu" from Start Menu (it will install automatically)
5. Create a username and password when prompted

### Step 2: Install Required Tools in WSL

```bash
# Update package list
sudo apt-get update

# Install all required tools
sudo apt-get install gcc flex bison nasm make git -y
```

### Step 3: Access Your Project from WSL

```bash
# Navigate to your Windows drive
cd /mnt/f/PROJECTS/CST\ 405/PROJECT/CST-405

# OR create a symbolic link for easier access
ln -s /mnt/f/PROJECTS/CST\ 405/PROJECT/CST-405 ~/compiler-project
cd ~/compiler-project
```

### Step 4: Build and Run

```bash
# Build the compiler
make -f Makefile_new

# Compile a test program
./compiler test_while.src

# Assemble and link
nasm -f elf64 output.asm -o output.o
gcc output.o -o program -no-pie

# Run the program
./program
```

**This gives you FULL functionality including running the generated programs!**

---

## OPTION 2: Using MinGW/MSYS2 (Compiler Only - No Execution)

### Step 1: Install MSYS2

1. Download from: https://www.msys2.org/
2. Install to `C:\msys64`
3. Open "MSYS2 MSYS" from Start Menu

### Step 2: Install Tools

```bash
# Update package database
pacman -Syu

# Install required packages
pacman -S mingw-w64-x86_64-gcc flex bison make
```

### Step 3: Navigate to Your Project

```bash
# In MSYS2 terminal
cd /f/PROJECTS/CST\ 405/PROJECT/CST-405
```

### Step 4: Build Compiler

```bash
# Build
make -f Makefile_new

# Compile source programs
./compiler.exe test_basic.src
```

**NOTE:** You can build the compiler and generate assembly, but CANNOT run the generated programs (they target Linux).

---

## OPTION 3: Using Git Bash (Minimal Setup)

### Step 1: Install Git for Windows

Download from: https://git-scm.com/download/win

During installation, select:
- ✅ "Use Git from Git Bash only"
- ✅ "Checkout as-is, commit Unix-style line endings"

### Step 2: Install Build Tools Manually

This is complex and NOT recommended. Use WSL or MSYS2 instead.

---

## RECOMMENDED WORKFLOW FOR YOUR SETUP

### Initial Setup (One Time)

**Open PowerShell as Administrator:**

```powershell
# Install WSL
wsl --install

# Restart computer
# Then open Ubuntu from Start Menu
```

**In Ubuntu terminal:**

```bash
# Install tools
sudo apt-get update
sudo apt-get install gcc flex bison nasm make -y

# Create easy shortcut to your project
echo "alias compiler='cd /mnt/f/PROJECTS/CST\ 405/PROJECT/CST-405'" >> ~/.bashrc
source ~/.bashrc
```

### Daily Usage

**Open Ubuntu from Start Menu:**

```bash
# Go to project
compiler

# Build
make -f Makefile_new

# Test
./compiler test_while.src

# Assemble and run
nasm -f elf64 output.asm -o output.o
gcc output.o -o program -no-pie
./program
```

---

## QUICK REFERENCE FOR YOUR LOCATION

### From WSL (Ubuntu):
```bash
cd /mnt/f/PROJECTS/CST\ 405/PROJECT/CST-405
```

### From PowerShell:
```powershell
cd "F:\PROJECTS\CST 405\PROJECT\CST-405"
```

### From Command Prompt:
```cmd
F:
cd \PROJECTS\CST 405\PROJECT\CST-405
```

---

## VIDEO RECORDING SETUP

### For Best Results in Video:

**Use WSL Ubuntu** - it will look professional and everything works.

**Terminal Setup:**

1. Open Ubuntu
2. Make text larger: Right-click title bar → Properties → Font → Size 20
3. Make window larger: Drag corners
4. Clear screen: `clear`

**Recording Commands:**

```bash
# Navigate to project
cd /mnt/f/PROJECTS/CST\ 405/PROJECT/CST-405

# Show you're in the right place
pwd
ls -la

# Continue with video script...
```

---

## TROUBLESHOOTING

### Problem: "Cannot find the path specified" in WSL

**Solution:** Check the mount point
```bash
# List Windows drives
ls /mnt/

# You should see 'c', 'f', etc.
# Navigate to F drive:
cd /mnt/f
```

### Problem: Line ending issues (^M characters)

**Solution:** Convert Windows line endings to Unix
```bash
# Install dos2unix
sudo apt-get install dos2unix

# Convert all source files
dos2unix *.l *.y *.c *.h *.src
```

### Problem: "Permission denied" when running ./compiler

**Solution:** Make it executable
```bash
chmod +x compiler
```

### Problem: WSL is slow accessing Windows files

**Solution:** Copy project to Linux filesystem
```bash
# Copy to your Linux home directory
cp -r /mnt/f/PROJECTS/CST\ 405/PROJECT/CST-405 ~/compiler-project
cd ~/compiler-project

# Work here (much faster)
# Copy results back to Windows when done
```

---

## FILE EDITING

### Option 1: Edit in Windows, Build in WSL

**Use VS Code:**
1. Install VS Code
2. Install "Remote - WSL" extension
3. In WSL terminal: `code .`
4. This opens VS Code connected to WSL - best of both worlds!

### Option 2: Edit in WSL

```bash
# Use nano (simple)
nano test_basic.src

# Or install vim
sudo apt-get install vim
vim test_basic.src
```

### Option 3: Edit in Windows

- Use Notepad++, VS Code, or any editor
- Save with Unix line endings (LF, not CRLF)
- In Notepad++: Edit → EOL Conversion → Unix (LF)

---

## COMPLETE WALKTHROUGH FOR YOUR SETUP

### One-Time Setup

```powershell
# In PowerShell (Admin)
wsl --install
# Restart computer
```

```bash
# In Ubuntu (after restart)
sudo apt-get update
sudo apt-get install gcc flex bison nasm make -y
```

### Every Time You Want to Work

```bash
# Open Ubuntu from Start Menu
cd /mnt/f/PROJECTS/CST\ 405/PROJECT/CST-405

# Build compiler (first time only)
make -f Makefile_new

# Compile your program
./compiler test_while.src

# Assemble and run
nasm -f elf64 output.asm -o output.o
gcc output.o -o program -no-pie
./program

# Clean up
make -f Makefile_new clean
```

---

## CHECKLIST FOR VIDEO RECORDING

**Before Recording:**

- [ ] WSL Ubuntu installed and working
- [ ] All tools installed (gcc, flex, bison, nasm, make)
- [ ] Can navigate to project directory
- [ ] Terminal font size increased for visibility
- [ ] Have video script open in another window

**During Recording:**

1. [ ] Start in project directory
2. [ ] Show files: `ls -la`
3. [ ] Show test program: `cat test_while.src`
4. [ ] Build: `make -f Makefile_new`
5. [ ] Compile: `./compiler test_while.src`
6. [ ] Explain output (AST, TAC, etc.)
7. [ ] Assemble: `nasm -f elf64 output.asm -o output.o`
8. [ ] Link: `gcc output.o -o program -no-pie`
9. [ ] Run: `./program`
10. [ ] Show output matches expected

**After Recording:**

- [ ] Verify video shows all required elements
- [ ] Audio is clear
- [ ] Screen is readable

---

## GETTING HELP

### Check WSL is installed:
```powershell
wsl --list --verbose
```

### Check tools in WSL:
```bash
which gcc flex bison nasm make
```

All should show paths like `/usr/bin/gcc`, etc.

### Test basic compilation:
```bash
echo 'int main() { return 0; }' > test.c
gcc test.c -o test
./test
echo $?  # Should print 0
```

---

## SUMMARY - YOUR EXACT STEPS

**In PowerShell (Admin) - ONE TIME:**
```powershell
wsl --install
# Restart computer
```

**In Ubuntu - ONE TIME:**
```bash
sudo apt-get update && sudo apt-get install gcc flex bison nasm make -y
```

**In Ubuntu - EVERY TIME:**
```bash
cd /mnt/f/PROJECTS/CST\ 405/PROJECT/CST-405
make -f Makefile_new
./compiler test_while.src
nasm -f elf64 output.asm -o output.o
gcc output.o -o program -no-pie
./program
```

**That's it! You're ready to record your video! 🎬**
