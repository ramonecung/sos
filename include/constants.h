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
    WRONG_NUMBER_ARGS,
    MALLOC_ERROR,
    INVALID_INPUT,
    CANNOT_CREATE_FILE,
    CANNOT_DELETE_FILE,
    CANNOT_OPEN_FILE,
    CANNOT_CLOSE_FILE,
    CANNOT_GET_CHAR,
    CANNOT_PUT_CHAR
};

#endif
