int x;
int y;
int result;

int main() {
    x = 10;
    y = 20;

    if (x > 5) {
        if (y > 15) {
            result = 100;
        } else {
            result = 50;
        }
    } else {
        result = 0;
    }

    print(result);
    return 0;
}
