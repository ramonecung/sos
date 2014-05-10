#include "../include/constants.h"
#include "util.h"
#include "strings.h"
#include "../memory/memory.h"
#include "../freescaleK70/io.h"
#include "../freescaleK70/hardware/svc.h"

#ifndef SOS
#include <stdio.h>
#include <stdlib.h>
#endif

#ifdef SOS
void *emalloc(int size, const char *requestor, Stream *ostrm) {
#else
void *emalloc(int size, const char *requestor, FILE *ostrm) {
#endif
#ifdef SOS
    void *p = myMalloc(size);
#else
    void *p = malloc(size);
#endif
    /* obtaining a null pointer is a problem unless caller requested 0 bytes */
    if (size != 0 && p == NULL) {
        efputs(requestor, ostrm);
        efputs(": could not allocate memory\n", ostrm);
    }
    return p;
}

void efree(void *ptr) {
#ifdef SOS
    myFree(ptr);
#else
    free(ptr);
#endif
}

#ifdef SOS
int efputc(int c, Stream *stream) {
#else
int efputc(int c, FILE *stream) {
#endif
    int rv;
#ifdef SOS
    rv = svc_myFputc(c, stream);
#else
    rv = fputc(c, stream);
#endif
    if (rv == EOF) {
        return WRITE_ERROR;
    } else {
        return SUCCESS;
    }
}
#ifdef SOS
int efputs(const char *s, Stream *stream) {
#else
int efputs(const char *s, FILE *stream) {
#endif
    int rv;
#ifdef SOS
    rv = svc_myFputs(s, stream);
#else
    rv = fputs(s, stream);
#endif
    if (rv == EOF) {
        return WRITE_ERROR;
    } else {
        return SUCCESS;
    }
}


char **new_array_of_strings(int num_strings, const char **strings) {
    char **arr = (char **) emalloc(num_strings * sizeof(char *), "new_array_of_strings", STDERR);
    int i;
    for (i = 0; i < num_strings; i++) {
        arr[i] = (char *) emalloc((1 + string_length(strings[i])) * sizeof(char), "new_array_of_strings", STDERR);
        string_copy(strings[i], arr[i]);
    }
    return arr;
}

void delete_array_of_strings(int num_strings, char **arr) {
    int i;
    for (i = 0; i < num_strings; i++) {
        efree(arr[i]);
    }
    efree(arr);
}

int absolute_value(int val) {
    if (val < 0) {
        return -1 * val;
    } else {
        return val;
    }
}

/* isNumericChar and myAtoi taken from */
/* http://www.geeksforgeeks.org/write-your-own-atoi/ */
// A utility function to check whether x is numeric
int isNumericChar(char x)
{
    return (x >= '0' && x <= '9')? 1: 0;
}

// A simple atoi() function. If the given string contains
// any invalid character, then this function returns 0
int myAtoi(char *str)
{
    int res = 0;  // Initialize result
    int sign = 1;  // Initialize sign as positive
    int i = 0;  // Initialize index of first digit

    if (*str == '\0')
       return 0;

    // If number is negative, then update sign
    if (str[0] == '-')
    {
        sign = -1;
        i++;  // Also update index of first digit
    }

    // Iterate through all digits of input string and update result
    for (; str[i] != '\0'; ++i)
    {
        if (isNumericChar(str[i]) == 0)
            return 0; // You may add some lines to write error message
                      // to error stream
        res = res*10 + str[i] - '0';
    }

    // Return result with sign
    return sign*res;
}
/* end isNumericChar and myAtoi */
