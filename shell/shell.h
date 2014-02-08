#include <stdio.h>

/* function declarations */
int sh_argc(void);

void print_prompt(FILE *ostrm);

char *create_input_buffer();

char *read_input(FILE *istrm);
