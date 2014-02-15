#include <stdio.h>

void *emalloc(int size, const char *requestor, FILE *ostrm);

char **new_array_of_strings(int num_strings, const char **strings);
void delete_array_of_strings(int num_strings, char **arr);
