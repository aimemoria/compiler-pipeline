// ERROR TEST FILE - Demonstrates lexical error detection
// This file contains intentional lexical errors to test error handling

int x;           // Valid - should work fine
int y@;          // ERROR: Invalid character '@' in identifier  
int z;           // Valid - should work fine

x = 10;          // Valid assignment
y# = 20;         // ERROR: Invalid character '#' 
z = x + y;       // Valid expression

print(x);        // Valid print statement
print$(z);       // ERROR: Invalid character '$'
print(y);        // Valid print statement

int 123abc;      // ERROR: Identifier cannot start with digit
int _valid;      // Valid - identifiers can start with underscore

a = b & c;       // ERROR: Invalid character '&' 
d = e | f;       // ERROR: Invalid character '|'

int test@var;    // ERROR: '@' not allowed in identifier
int test_var;    // Valid - underscores are allowed

x = 42%;         // ERROR: Invalid character '%'
y = 100.5;       // ERROR: Decimal point not supported (only integers)

print("hello");  // ERROR: String literals not in our grammar
print('a');      // ERROR: Character literals not in our grammar

// Test various invalid characters that might appear
int a*b;         // ERROR: '*' not defined as operator
int c/d;         // ERROR: '/' not defined as operator  
int e?f;         // ERROR: '?' not valid character
int g[5];        // ERROR: '[' and ']' not in our grammar
int h{i};        // ERROR: '{' and '}' not in our grammar

// Valid ending to test recovery
int final;
final = 999;
print(final);
