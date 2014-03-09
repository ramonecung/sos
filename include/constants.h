/* constants */
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_INPUT_LEN 256
#define TRUE 1
#define FALSE 0

enum error_code {
    SUCCESS,
    READ_ERROR,
    WRITE_ERROR,
    TIME_ERROR,
    EXIT_ARGS_UNSUPPORTED,
    DATE_ARGS_UNSUPPORTED,
    WRONG_NUMBER_ARGS
};

#endif
