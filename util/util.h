#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

/* memory allocation */
void *emalloc(int size, const char *requestor, FILE *ostrm);

/* I/O */
int efputc(int c, FILE *stream);
int efputs(const char * s, FILE *stream);

/* strings */
char **new_array_of_strings(int num_strings, const char **strings);
void delete_array_of_strings(int num_strings, char **arr);
int myAtoi(char *str);
int isNumericChar(char x);

/* math */
int absolute_value(int val);

#endif
