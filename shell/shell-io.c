#include "../include/constants.h"


#include "../freescaleK70/io.h"
#include "../freescaleK70/io_fs.h"
#include "../util/util.h"
#include "shell-io.h"

#ifdef TEST_SHELL
#include <stdio.h>
#endif

#ifdef TEST_SHELL
int cmd_create(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_create(int argc, char *argv[]) {
#endif
    int res;
    if (argc != 2) {
        return WRONG_NUMBER_ARGS;
    }
    res = create_fs(argv[1]);
    if (res != SUCCESS) {
        efputs("create: error creating file\n", ostrm);
    }
    return res;
}

#ifdef TEST_SHELL
int cmd_delete(int argc, char *argv[], FILE *ostrm) {
#else
int cmd_delete(int argc, char *argv[]) {
#endif
    return SUCCESS;
}
