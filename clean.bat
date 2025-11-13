@echo off
REM ========================================
REM CST-405 Compiler Clean Script (Windows)
REM ========================================

echo.
echo ========================================
echo Cleaning CST-405 Compiler Build Files
echo ========================================
echo.

echo Removing object files...
if exist *.o del *.o

echo Removing generated parser files...
if exist parser.tab.c del parser.tab.c
if exist parser.tab.h del parser.tab.h
if exist parser.output del parser.output

echo Removing generated lexer files...
if exist lex.yy.c del lex.yy.c

echo Removing compiler executable...
if exist compiler.exe del compiler.exe

echo Removing output files...
if exist output.asm del output.asm
if exist output.o del output.o
if exist program.exe del program.exe

echo Removing backup files...
if exist *~ del *~
if exist *.bak del *.bak

echo.
echo ========================================
echo CLEAN COMPLETE
echo ========================================
echo.
