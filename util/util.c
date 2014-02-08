 #include <stdio.h>
 #include <stdlib.h>

void *emalloc(int size, const char *requestor, FILE *ostrm) {
    void *p = malloc(size);
    if (p == NULL) {
        fputs(requestor, ostrm);
        fputs(": could not allocate memory\n", ostrm);
    }
    return p;
}
