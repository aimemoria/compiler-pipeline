// Test program for array support
// Tests array declaration, initialization, and access

int arr[5];
int i;
int sum;
int value;

int main() {
    // Initialize array elements
    arr[0] = 10;
    arr[1] = 20;
    arr[2] = 30;
    arr[3] = 40;
    arr[4] = 50;

    // Access and print array elements
    print(arr[0]);  // Should print 10
    print(arr[1]);  // Should print 20
    print(arr[2]);  // Should print 30

    // Compute sum of array elements
    sum = 0;
    i = 0;
    while (i < 5) {
        value = arr[i];
        sum = sum + value;
        i = i + 1;
    }

    print(sum);  // Should print 150 (10+20+30+40+50)

    // Test array with expressions as index
    i = 2;
    value = arr[i];
    print(value);  // Should print 30

    // Test array assignment with expression
    arr[3] = arr[0] + arr[1];
    print(arr[3]);  // Should print 30 (10+20)

    return 0;
}
