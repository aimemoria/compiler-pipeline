// Test program for nested loops
// Demonstrates for loops, while loops, and do-while loops working together

int main() {
    int i;
    int j;
    int sum;
    sum = 0;

    // Nested for loop
    for (i = 1; i <= 3; i = i + 1;) {
        for (j = 1; j <= 2; j = j + 1;) {
            sum = sum + 1;
        }
    }

    print(sum);  // Should print 6 (3*2)

    int k;
    k = 0;

    // While loop with do-while inside
    while (k < 2) {
        int m;
        m = 0;
        do {
            sum = sum + 1;
            m = m + 1;
        } while (m < 2);
        k = k + 1;
    }

    print(sum);  // Should print 10 (6 + 2*2)
    return 0;
}
