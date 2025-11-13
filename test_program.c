int x;
int y;
int sum;

int main() {
    x = 5;
    y = 10;
    sum = x + y;
    print(sum);
    while (x < y) {
        x = x + 1;
        print(x);
    }
    return 0;
}
