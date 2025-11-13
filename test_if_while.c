int i;
int count;

int main() {
    i = 0;
    count = 0;

    while (i < 10) {
        if (i > 5) {
            count = count + 1;
        }
        i = i + 1;
    }

    print(count);
    return 0;
}
