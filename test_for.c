// Test program for FOR loop
// This program demonstrates the for loop functionality

int main() {
    int sum;
    sum = 0;
    int i;

    // For loop: sum = 1 + 2 + 3 + 4 + 5
    for (i = 1; i <= 5; i = i + 1;) {
        sum = sum + i;
    }

    print(sum);  // Should print 15
    return 0;
}
