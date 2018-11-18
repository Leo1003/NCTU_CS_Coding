#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define getcx getchar
#else
#define getcx getchar_unlocked
#endif

#define is_between(v, a, b) (v >= a && v <= b)

enum base_mode {
    NONE = 0,
    BIN = 2,
    OCT = 8,
    DEC = 10,
    HEX = 16
};

int is_digit(int ch, enum base_mode base) {
    if (is_between(ch, '0', '1')) {
        return 1;
    } else if (is_between(ch, '2', '7') && base >= OCT) {
        return 1;
    } else if (is_between(ch, '8', '9') && base >= DEC) {
        return 1;
    } else if ((is_between(ch, 'a', 'f') || is_between(ch, 'A', 'F')) && base >= HEX) {
        return 1;
    } 
    return 0;
}

long long add_digit(long long n, enum base_mode base) {
    switch (base) {
        case BIN:
            n <<= 1;
            break;
        case OCT:
            n <<= 3;
            break;
        case DEC:
            n = (n << 3) + (n << 1);
            break;
        case HEX:
            n <<= 4;
            break;
        default:
            break;
    }
    return n;
}

int to_digit(int c) {
    if (is_between(c, '0', '9'))
        return (c - '0');
    if (is_between(c, 'a', 'f'))
        return (c - 'a') + 10;
    if (is_between(c, 'A', 'F'))
        return (c - 'A') + 10;
    return 0;
}

long long inp_base()
{
    enum base_mode b = NONE;
    long long n = 0;
    int c = getcx();
    int sign = 1;

    while (!b) {
        if (c == '0') {
            c = getcx();
            switch (c) {
                case 'b':
                    b = BIN;
                    c = getcx();
                    break;
                case 'o':
                    b = OCT;
                    c = getcx();
                    break;
                case 'x':
                    b = HEX;
                    c = getcx();
                    break;
                default:
                    b = DEC;
                    break;
            }
        } else if (c == '-') {
            sign = -1;
            b = DEC;
        } else if (is_digit(c, DEC)) {
            b = DEC;
        } else {
            c = getcx();
        }
    }

    while (is_digit(c, b)) {
        n = add_digit(n, b);
        n += to_digit(c);
        c = getcx();
    }
    if (b == DEC)
        return sign * n;
    else
        return n;
}

void print_by_base(long long int num, enum base_mode mode)
{
    char binstr[64 + 1];
    int i = 64;
    switch (mode) {
        case BIN:
            binstr[i--] = '\0';
            do {
                binstr[i--] = (num & 1) ? '1' : '0';
                num >>= 1;
            } while (num && i > 0);
            i++;
            printf("0b%s\n", binstr + i);
            break;
        case OCT:
            printf("0o%llo\n", num);
            break;
        case DEC:
            printf("%lld\n", num);
            break;
        case HEX:
            printf("0x%llx\n", num);
            break;
    }
}

int main(int argc, char *argv[])
{
    enum base_mode m = DEC;
    long long int val;
    if (argc > 1) {
        if (!strcmp(argv[1], "bin")) {
            m = BIN;
        } else if (!strcmp(argv[1], "oct")) {
            m = OCT;
        } else if (!strcmp(argv[1], "dec")) {
            m = DEC;
        } else if (!strcmp(argv[1], "hex")) {
            m = HEX;
        } else {
            exit(1);
        }
    }
    while (val = inp_base()) {
        print_by_base(val, m);
    }
    return 0;
}
