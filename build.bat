@echo off
REM ========================================
REM CST-405 Compiler Build Script (Windows)
REM ========================================

REM Add common tool paths to PATH
set PATH=%PATH%;C:\ProgramData\chocolatey\bin
set PATH=%PATH%;C:\Program Files (x86)\GnuWin32\bin

echo.
echo ========================================
echo Building CST-405 Compiler
echo ========================================
echo.

REM Check if bison is available (try both bison and win_bison)
where bison >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    where win_bison >nul 2>nul
    if %ERRORLEVEL% NEQ 0 (
        echo ERROR: Bison not found in PATH!
        echo.
        echo Please install Bison:
        echo   Option 1: choco install winflexbison3
        echo   Option 2: Download from https://github.com/lexxmark/winflexbison/releases
        echo.
        exit /b 1
    ) else (
        set BISON=win_bison
        echo Found: win_bison.exe
    )
) else (
    set BISON=bison
    echo Found: bison.exe
)

REM Check if flex is available (try both flex and win_flex)
where flex >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    where win_flex >nul 2>nul
    if %ERRORLEVEL% NEQ 0 (
        echo ERROR: Flex not found in PATH!
        echo.
        echo Please install Flex or add to PATH:
        echo   set PATH=%%PATH%%;C:\Program Files (x86)\GnuWin32\bin
        echo.
        exit /b 1
    ) else (
        set FLEX=win_flex
        echo Found: win_flex.exe
    )
) else (
    set FLEX=flex
    echo Found: flex.exe
)

echo [1/5] Generating Parser with Bison...
%BISON% -d -v parser.y
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Parser generation failed!
    exit /b 1
)
echo       Generated: parser.tab.c, parser.tab.h
echo.

echo [2/5] Generating Lexer with Flex...
%FLEX% scanner_new.l
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Lexer generation failed!
    exit /b 1
)
echo       Generated: lex.yy.c
echo.

echo [3/5] Compiling source files...
gcc -Wall -g -c compiler.c -o compiler.o
gcc -Wall -g -c parser.tab.c -o parser.tab.o
gcc -Wall -g -c lex.yy.c -o lex.yy.o
gcc -Wall -g -c ast.c -o ast.o
gcc -Wall -g -c symtable.c -o symtable.o
gcc -Wall -g -c semantic.c -o semantic.o
gcc -Wall -g -c ircode.c -o ircode.o
gcc -Wall -g -c codegen.c -o codegen.o

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Compilation failed!
    exit /b 1
)
echo       Compiled 8 object files
echo.

echo [4/5] Linking compiler executable...
gcc -Wall -g -o compiler.exe compiler.o parser.tab.o lex.yy.o ast.o symtable.o semantic.o ircode.o codegen.o

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Linking failed!
    exit /b 1
)
echo.

echo [5/5] Verifying build...
if exist compiler.exe (
    echo.
    echo ========================================
    echo BUILD SUCCESSFUL!
    echo ========================================
    echo.
    echo Compiler executable: compiler.exe
    echo.
    echo Next steps:
    echo   1. Test with: compiler.exe test_if.c
    echo   2. View output: type output.asm
    echo.
) else (
    echo.
    echo ========================================
    echo BUILD FAILED!
    echo ========================================
    echo.
    exit /b 1
)
