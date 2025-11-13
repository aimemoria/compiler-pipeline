@echo off
REM ========================================
REM CST-405 Compiler Test Script (Windows)
REM ========================================

if not exist compiler.exe (
    echo ERROR: Compiler not built yet!
    echo Please run: build.bat
    exit /b 1
)

echo.
echo ========================================
echo Testing CST-405 Compiler
echo ========================================
echo.

REM Test 1: Simple If Statement
echo ----------------------------------------
echo Test 1: Simple If Statement
echo ----------------------------------------
echo Source: test_if.c
echo.
type test_if.c
echo.
echo Running compiler...
compiler.exe test_if.c
if %ERRORLEVEL% EQU 0 (
    echo [PASS] Test 1 completed successfully
    echo Generated: output.asm
) else (
    echo [FAIL] Test 1 failed
)
echo.
pause

REM Test 2: If-Else Statement
echo.
echo ----------------------------------------
echo Test 2: If-Else Statement
echo ----------------------------------------
echo Source: test_if_else.c
echo.
type test_if_else.c
echo.
echo Running compiler...
compiler.exe test_if_else.c
if %ERRORLEVEL% EQU 0 (
    echo [PASS] Test 2 completed successfully
    echo Generated: output.asm
) else (
    echo [FAIL] Test 2 failed
)
echo.
pause

REM Test 3: Nested If Statements
echo.
echo ----------------------------------------
echo Test 3: Nested If Statements
echo ----------------------------------------
echo Source: test_nested_if.c
echo.
type test_nested_if.c
echo.
echo Running compiler...
compiler.exe test_nested_if.c
if %ERRORLEVEL% EQU 0 (
    echo [PASS] Test 3 completed successfully
    echo Generated: output.asm
) else (
    echo [FAIL] Test 3 failed
)
echo.
pause

REM Test 4: If with While
echo.
echo ----------------------------------------
echo Test 4: Combined If and While
echo ----------------------------------------
echo Source: test_if_while.c
echo.
type test_if_while.c
echo.
echo Running compiler...
compiler.exe test_if_while.c
if %ERRORLEVEL% EQU 0 (
    echo [PASS] Test 4 completed successfully
    echo Generated: output.asm
) else (
    echo [FAIL] Test 4 failed
)
echo.

echo.
echo ========================================
echo ALL TESTS COMPLETED
echo ========================================
echo.
echo To view assembly output: type output.asm
echo To view IR code: Check compiler output above
echo.
pause
