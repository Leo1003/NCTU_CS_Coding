#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STR_LEN 1000
#define MIN(_a, _b) (_a > _b ? _b : _a)


static void bucket(char *str, int bk[26])
{
    memset(bk, 0, sizeof(int) * 26);
    size_t i;
    for (i = 0; i < strlen(str); i++) {
        if (islower(str[i])) {
            bk[str[i] - 'a']++;
        }
    }
}

int main() {
    char *s1 = NULL, *s2 = NULL;
    size_t l1 = 0, l2 = 0;
    char s[STR_LEN + 1];
    while (getline(&s1, &l1, stdin) != EOF) {
        getline(&s2, &l2, stdin);
        int bk1[26], bk2[26];
        bucket(s1, bk1);
        bucket(s2, bk2);

        char *sp = s;
        int i, j;
        for (i = 0; i < 26; i++) {
            for (j = 0; j < MIN(bk1[i], bk2[i]); j++) {
                *sp = 'a' + i;
                sp++;
            }
        }
        *sp = '\0';
        
        printf("%s\n", s);
    }
    free(s1);
    free(s2);
    return 0;
}
