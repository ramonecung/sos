#ifndef UTIL_H
#define UTIL_H

#ifndef TEST_SHELL
#include "../include/io.h"
#else
#include <stdio.h>
#endif


/* memory allocation */
#ifndef TEST_SHELL
void *emalloc(int size, const char *requestor, struct Stream *output);
#else
void *emalloc(int size, const char *requestor, FILE *output);
#endif

void efree(void *ptr);

/* I/O */
#ifndef TEST_SHELL
int efgetc(Stream *stream);
int efputc(int c, Stream *stream);
int efputs(const char * s, Stream *stream);
int efflush(Stream *stream);
#else
int efgetc(FILE *stream);
int efputc(int c, FILE *stream);
int efputs(const char * s, FILE *stream);
int efflush(FILE *stream);
#endif


/* strings */
char **new_array_of_strings(int num_strings, const char **strings);
void delete_array_of_strings(int num_strings, char **arr);
int myAtoi(char *str);
int isNumericChar(char x);
unsigned long long myStrtoull(const char *nptr, char **endptr, int base);

/* math */
int absolute_value(int val);

#endif
