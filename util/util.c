#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *emalloc(int size, const char *requestor, FILE *ostrm) {
    void *p = malloc(size);
    if (p == NULL) {
        fputs(requestor, ostrm);
        fputs(": could not allocate memory\n", ostrm);
    }
    return p;
}

char **new_array_of_strings(int num_strings, const char **strings) {
    char **arr = (char **) malloc(num_strings * sizeof(char *));
    int i;
    for (i = 0; i < num_strings; i++) {
        arr[i] = (char *) malloc((1 + strlen(strings[i])) * sizeof(char));
        strcpy(arr[i], strings[i]);
    }
    return arr;
}

void delete_array_of_strings(int num_strings, char **arr) {
    int i;
    for (i = 0; i < num_strings; i++) {
        free(arr[i]);
    }
    free(arr);
}

int absolute_value(int val) {
    if (val < 0) {
        return -1 * val;
    } else {
        return val;
    }
}

