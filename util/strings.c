#include "strings.h"
#include "stdlib.h"

int string_length(const char *s) {
    const char *cp = s;
    int len = 0;
    while (*cp) {
        len++;
        cp++;
    }
    return len;
}

int strings_equal(const char *base_s, char *new_s) {
    const char *p1;
    char *p2;
    p1 = base_s, p2 = new_s;
    while(*p1 && *p2) {
        if (*p1++ != *p2++) {
            return 0;
        }
    }
    if ((*p1 && !*p2) || (!*p1 && *p2)) {
        return 0;
    }
    return 1;
}
