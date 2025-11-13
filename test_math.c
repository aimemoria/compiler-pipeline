// Test program for mathematical expressions
// Tests all arithmetic operators: +, -, *, /, %
// Tests operator precedence and parentheses

int a;
int b;
int c;
int result;

int main() {
    // Test basic operations
    a = 10;
    b = 3;

    // Addition
    result = a + b;
    print(result);  // Should print 13

    // Subtraction
    result = a - b;
    print(result);  // Should print 7

    // Multiplication
    result = a * b;
    print(result);  // Should print 30

    // Division
    result = a / b;
    print(result);  // Should print 3

    // Modulo
    result = a % b;
    print(result);  // Should print 1

    // Test operator precedence: 2 + 3 * 4 should be 14 (not 20)
    result = 2 + 3 * 4;
    print(result);  // Should print 14

    // Test parentheses: (2 + 3) * 4 should be 20
    result = (2 + 3) * 4;
    print(result);  // Should print 20

    // Complex expression: 10 + 5 * 2 - 8 / 4 = 10 + 10 - 2 = 18
    result = 10 + 5 * 2 - 8 / 4;
    print(result);  // Should print 18

    // Another complex expression with parentheses
    c = (10 - 2) * (3 + 2);
    print(c);  // Should print 40

    return 0;
}
