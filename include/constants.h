/* constants */
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_INPUT_LEN 256
#define TRUE 1
#define FALSE 0

#ifndef NULL
#define NULL 0
#endif

enum error_code {
    SUCCESS, /* implicitly counting on this to be 0 */
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
    CANNOT_APPEND_BLOCK,
    CANNOT_KILL_INIT_PROCESS,
    CANNOT_ALLOCATE_STACK
};

#endif
