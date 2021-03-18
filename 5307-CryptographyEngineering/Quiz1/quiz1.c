#include <stdio.h>
#include <string.h>

int main()
{
    int ch;
    unsigned letters[26];

    memset(&letters, 0, sizeof(letters));
    while ((ch = getchar()) != EOF) {
        char c = (char)ch;

        if (c >= 'a' && c <= 'z') {
            letters[c - 'a']++;
        } else if (c >= 'A' && c <= 'Z') {
            letters[c - 'A']++;
        }
    }

    for (size_t i = 0; i < 26; i++) {
        printf("%c: %u\n", (char)('A' + i), letters[i]);
    }

    return 0;
}

