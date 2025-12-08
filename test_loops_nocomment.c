int main() {
    int i;
    int sum;
    int n;

    i = 0;
    sum = 0;
    while (i < 5) {
        sum = sum + i;
        i = i + 1;
    }
    print(sum);

    sum = 0;
    for (i = 0; i < 5; i = i + 1;) {
        sum = sum + i;
    }
    print(sum);

    i = 0;
    sum = 0;
    do {
        sum = sum + i;
        i = i + 1;
    } while (i < 5);
    print(sum);

    return 0;
}
