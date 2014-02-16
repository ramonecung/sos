/* constants */
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_INPUT_LEN 256


enum error_code {
    SUCCESS,
    READ_ERROR,
    WRITE_ERROR,
    TIME_ERROR,
    EXIT_ARGS_UNSUPPORTED,
    DATE_ARGS_UNSUPPORTED
};

#define NUM_MONTHS_IN_YEAR 12
#define AFTER 1
#define BEFORE -1
enum months_in_year {
    JAN,
    FEB,
    MAR,
    APR,
    MAY,
    JUN,
    JUL,
    AUG,
    SEP,
    OCT,
    NOV,
    DEC
};

#endif
