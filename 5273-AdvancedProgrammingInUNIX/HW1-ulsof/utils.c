#include "utils.h"

#include <ctype.h>
#include <string.h>

char *trim(char *s)
{
    // Trim left
    while (isspace(*s)) {
        s++;
    }

    if (*s == '\0') {
        return s;
    }
    
    // Trim right
    char *e = s + strlen(s) - 1;
    while (e > s && isspace(*e)) {
        e--;
    }

    *(e + 1) = '\0';

    return s;
}

