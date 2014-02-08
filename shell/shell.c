/*
 * shell
 */
 #include "shell.h"
 #include "../include/constants.h"
 #include "../util/util.h"
 #include <stdio.h>


 int sh_argc(void) {
    return -1;
}

void print_prompt(FILE *ostrm) {
    fputc('$', ostrm);
    fputc(' ', ostrm);
}

char *create_input_buffer() {
    void *buf = emalloc(sizeof (char) * (MAX_INPUT_LEN + 1),
                        "create_input_buffer", stdout);
    return (char *) buf;
}

char *read_input(FILE *istrm) {
    char *buf = create_input_buffer();
    char c;
    int num_read = 0;
    int i = 0;
    buf = fgets(buf, MAX_INPUT_LEN + 1, istrm);
    fclose(istrm);
    return buf;
}
