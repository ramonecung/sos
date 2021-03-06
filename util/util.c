#include "../include/constants.h"
#include "util.h"
#include "strings.h"
#include "../memory/memory.h"
#include "../include/io.h"
#include "../include/svc.h"

#ifdef TEST_SHELL
#include <stdio.h>
#include <stdlib.h>
#endif

#ifndef TEST_SHELL
void *emalloc(int size, const char *requestor, Stream *output) {
#else
void *emalloc(int size, const char *requestor, FILE *output) {
#endif
#ifndef TEST_SHELL
    void *p = svc_myMalloc(size);
#else
    void *p = malloc(size);
#endif
    /* obtaining a null pointer is a problem unless caller requested 0 bytes */
    if (size != 0 && p == NULL) {
        efputs(requestor, output);
        efputs(": could not allocate memory\n", output);
    }
    return p;
}

void efree(void *ptr) {
#ifndef TEST_SHELL
    svc_myFree(ptr);
#else
    free(ptr);
#endif
}

#ifndef TEST_SHELL
int efgetc(Stream *stream) {
#else
int efgetc(FILE *stream) {
#endif
#ifndef TEST_SHELL
    return svc_myFgetc(stream);
#else
    return fgetc(stream);
#endif
}


#ifndef TEST_SHELL
int efputc(int c, Stream *stream) {
#else
int efputc(int c, FILE *stream) {
#endif
    int rv;
#ifndef TEST_SHELL
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

#ifndef TEST_SHELL
int efputs(const char *s, Stream *stream) {
#else
int efputs(const char *s, FILE *stream) {
#endif
    int rv;
#ifndef TEST_SHELL
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

#ifndef TEST_SHELL
int efflush(Stream *stream) {
#else
int efflush(FILE *stream) {
#endif
    int rv;
#ifndef TEST_SHELL
    rv = svc_myFflush(stream);
#else
    rv = fflush(stream);
#endif
    return rv;
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
int myAtoi(char *str) {
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

/* adapted from http://www.opensource.apple.com/source/Libc/Libc-320/stdlib/FreeBSD/strtoull.c */
unsigned long long myStrtoull(const char *nptr, char **endptr, int base) {
    const char *s;
    unsigned long long acc;
    char c;
    int neg;

    /* not actually using endptr */
    if (endptr != NULL) {
        *endptr = NULL;
    }

    s = nptr;
    do {
        c = *s++;
    } while (c == ' ' || c == '\t');
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else {
        neg = 0;
        if (c == '+')
            c = *s++;
    }
    if (neg == 1) {
        /* error for unsigned */
        ;
    }
    if ((base == 0 || base == 16) &&
        c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;
    acc = 0;

    for ( ; ; c = *s++) {
        if (c >= '0' && c <= '9')
            c -= '0';
        else if (c >= 'A' && c <= 'Z')
            c -= 'A' - 10;
        else if (c >= 'a' && c <= 'z')
            c -= 'a' - 10;
        else
            break;
        if (c >= base)
            break;

        acc *= base;
        acc += c;
    }
    return (acc);
}
/* end myStrtoull */
