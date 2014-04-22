#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/constants.h"

void *emalloc(int size, const char *requestor, FILE *ostrm) {
    void *p = malloc(size);
    if (p == NULL) {
        fputs(requestor, ostrm);
        fputs(": could not allocate memory\n", ostrm);
    }
    return p;
}

int efputc(int c, FILE *stream) {
    int rv = fputc(c, stream);
    if (rv == EOF) {
        return WRITE_ERROR;
    } else {
        return SUCCESS;
    }
}

int efputs(const char *s, FILE *stream) {
    int rv = fputs(s, stream);
    if (rv == EOF) {
        return WRITE_ERROR;
    } else {
        return SUCCESS;
    }
}


char **new_array_of_strings(int num_strings, const char **strings) {
    char **arr = (char **) malloc(num_strings * sizeof(char *));
    int i;
    for (i = 0; i < num_strings; i++) {
        arr[i] = (char *) malloc((1 + strlen(strings[i])) * sizeof(char));
        strcpy(arr[i], strings[i]);
    }
    return arr;
}

void delete_array_of_strings(int num_strings, char **arr) {
    int i;
    for (i = 0; i < num_strings; i++) {
        free(arr[i]);
    }
    free(arr);
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
    if (*str == '\0')
       return 0;
 
    int res = 0;  // Initialize result
    int sign = 1;  // Initialize sign as positive
    int i = 0;  // Initialize index of first digit
 
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
