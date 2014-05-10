#ifndef UTIL_H
#define UTIL_H

#ifdef SOS
#include "../freescaleK70/io.h"
#else
#include <stdio.h>
#endif

/* memory allocation */
#ifdef SOS
void *emalloc(int size, const char *requestor, struct Stream *ostrm);
#else
void *emalloc(int size, const char *requestor, FILE *ostrm);
#endif

void efree(void *ptr);

/* I/O */
#ifdef SOS
int efputc(int c, Stream *stream);
int efputs(const char * s, Stream *stream);
#else
int efputc(int c, FILE *stream);
int efputs(const char * s, FILE *stream);
#endif



/* strings */
char **new_array_of_strings(int num_strings, const char **strings);
void delete_array_of_strings(int num_strings, char **arr);
int myAtoi(char *str);
int isNumericChar(char x);

/* math */
int absolute_value(int val);

#endif
