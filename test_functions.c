// Test program for function support
// Tests function declarations, definitions, calls, parameters, and returns

// Function declaration (prototype)
int add(int a, int b);
int multiply(int x, int y);

// Simple function that adds two numbers
int add(int a, int b) {
    int result;
    result = a + b;
    return result;
}

// Function that multiplies two numbers
int multiply(int x, int y) {
    int result;
    result = x * y;
    return result;
}

// Function that computes factorial
int factorial(int n) {
    int result;
    int i;
    result = 1;
    i = 1;
    while (i <= n) {
        result = result * i;
        i = i + 1;
    }
    return result;
}

// Global variables
int x;
int y;
int sum;
int product;
int fact;
int result;

// Main program
int main() {
    x = 5;
    y = 10;

    // Call add function
    sum = add(x, y);
    print(sum);  // Should print 15

    // Call multiply function
    product = multiply(x, y);
    print(product);  // Should print 50

    // Call factorial function
    fact = factorial(5);
    print(fact);  // Should print 120

    // Nested function calls
    result = add(multiply(2, 3), multiply(4, 5));
    print(result);  // Should print 26 (6 + 20)

    return 0;
}
