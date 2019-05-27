#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
# define debugf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
# define debugf(fmt, ...)
#endif
#define errf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

void getstdin(char **ptr, size_t *len)
{
    if (getline(ptr, len, stdin) < 0) {
        if (errno > 0) {
            abort();
        } else {
            exit(0);
        }
    }
    size_t l = strlen(*ptr);
    if ((*ptr)[l - 1] == '\n') {
        (*ptr)[l - 1] = '\0';
    }
}

void failure_function(const char *str, int *data)
{
    size_t l = strlen(str);
    size_t i = 0, j = 0;
    while (i <= l) {
        if (i == 0) {
            data[i++] = -1;
            continue;
        } else if (i == 1) {
            data[i++] = 0;
            continue;
        }
        if (str[i - 1] == str[j]) {
            data[i++] = ++j;
        } else if (j > 0) {
            j = data[j];
        } else {
            data[i++] = 0;
        }
    }
}

int main()
{
    char *input = NULL, *target = NULL;
    size_t input_l = 0, target_l = 0;
    errf("Enter input string: \n");
    getstdin(&input, &input_l);
    errf("Enter search string: \n");
    getstdin(&target, &target_l);

    size_t il = strlen(input), tl = strlen(target);

    int *failure_data = (int *)malloc(sizeof(int) * (tl + 1));
    if (failure_data == NULL) {
        abort();
    }
    failure_function(target, failure_data);

    
    bool *result = (bool *)malloc(sizeof(bool) * il);
    if (result == NULL) {
        abort();
    }
    memset(result, 0, sizeof(bool) * il);

    for (size_t d = 0; d <= tl; d++) {
        debugf("%d ", failure_data[d]);
    }
    debugf("\n");

    size_t i = 0, j = 0;
    while (i < il) {
        debugf("i = %zu, j = %zu ", i, j);
        if (input[i] == target[j]) {
            debugf("Matched: '%c'\n", input[i]);
            i++;
            j++;
            if (j >= tl) {
                result[i - tl] = true;
                errf("Found: %zu\n", i - tl);
                if (failure_data[j] > 0) {
                    j = failure_data[j];
                } else {
                    j = 0;
                }
            }
        } else {
            debugf("Not matched\n");
            if (failure_data[j] > 0) {
                j = failure_data[j];
            } else {
                if (failure_data[j] == -1) {
                    i++;
                }
                j = 0;
            }
        }
    }

    printf("%s\n", input);
    for (size_t s = 0; s < il; s++) {
        if (result[s]) {
            printf("^");
        } else {
            printf(" ");
        }
    }
    printf("\n");

    free(result);
    free(failure_data);
    free(target);
    free(input);
    return 0;
}
