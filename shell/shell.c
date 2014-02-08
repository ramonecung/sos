/*
 * shell
 */
 #include "shell.h"
 #include <stdio.h>

int sh_argc(void) {
    return 0;
}

void print_prompt(FILE *ostrm) {
    fputc('$', ostrm);
}
