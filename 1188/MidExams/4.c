#include <ctype.h>
#include <stdio.h>
#include <string.h>
#define MAX_DIGITS 10
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

const char *digit_0[] = {
    " -- ",
    "|  |",
    "|  |",
    "|  |",
    " -- ",
};
const char *digit_1[] = {
    "    ",
    "   |",
    "   |",
    "   |",
    "    ",
};
const char *digit_2[] = {
    " -- ",
    "   |",
    " -- ",
    "|   ",
    " -- ",
};
const char *digit_3[] = {
    " -- ",
    "   |",
    " --|",
    "   |",
    " -- ",
};
const char *digit_4[] = {
    "    ",
    "|  |",
    " --|",
    "   |",
    "    ",
};
const char *digit_5[] = {
    " -- ",
    "|   ",
    " -- ",
    "   |",
    " -- ",
};
const char *digit_6[] = {
    " -- ",
    "|   ",
    "|-- ",
    "|  |",
    " -- ",
};
const char *digit_7[] = {
    "--- ",
    "   |",
    "   |",
    "   |",
    "    ",
};
const char *digit_8[] = {
    " -- ",
    "|  |",
    "|--|",
    "|  |",
    " -- ",
};
const char *digit_9[] = {
    " -- ",
    "|  |",
    " --|",
    "   |",
    " -- ",
};

const char **digit[] = {
    digit_0,
    digit_1,
    digit_2,
    digit_3,
    digit_4,
    digit_5,
    digit_6,
    digit_7,
    digit_8,
    digit_9,
};

void print_digits(const char *str)
{
    for (int l = 0; l < 5; l++) {
        for (int i = 0; i < strlen(str) && i < MAX_DIGITS; i++) {
            if (!isdigit(str[i])) {
                continue;
            }
            printf("%s", digit[str[i] - '0'][l]);
        }
        printf("\n");
    }
}

int main()
{
    char str[MAX_DIGITS + 1];
    scanf("%" STR(MAX_DIGITS) "s", str);
    print_digits(str);
    return 0;
}
