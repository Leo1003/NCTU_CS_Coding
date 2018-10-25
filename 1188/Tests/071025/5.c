#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int set_str(char *dest, char *src, int len)
{
    strncpy(dest, src, len);
    dest[len] = '\0';
    return len;
}

int main()
{
    int T;
    scanf("%d", &T);
    for (int s = 0; s < T; s++) {
        char str[21], ans[21];
        int anslen = 0;

        scanf("%20s", str);
        int len = strlen(str);

        //Test for every center position
        for (int i = 0; i < len; i++) {
            int j = 0;
            while (1) {
                if ((i - j < 0) || (i + j >= len)) {
                    break;
                }
                if (str[i - j] != str[i + j]) {
                    break;
                }
                j++;
            }
            int start = (i - j + 1);
            int match_len = (2 * j) - 1;
            if (match_len > anslen) {
                anslen = set_str(ans, str + start, match_len);
            } else if (match_len == anslen) {
                if (strncmp(ans, str + start, match_len) > 0) {
                    anslen = set_str(ans, str + start, match_len);
                }
            }
        }
        for (int i = 0; i < len; i++) {
            int j = 0;
            while (1) {
                if ((i - j < 0) || (i + j + 1 >= len)) {
                    break;
                }
                if (str[i - j] != str[i + j + 1]) {
                    break;
                }
                j++;
            }
            int start = (i - j + 1);
            int match_len = (2 * j);
            if (match_len > anslen) {
                anslen = set_str(ans, str + start, match_len);
            } else if (match_len == anslen) {
                if (strncmp(ans, str + start, match_len) > 0) {
                    anslen = set_str(ans, str + start, match_len);
                }
            }
        }
        printf("%s\n", ans);
    }
    return 0;
}
