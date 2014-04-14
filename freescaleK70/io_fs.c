#include "../include/constants.h"
#include "io_fs.h"

int filename_valid(char *filename) {
    return 1;
}

int prefix_valid(char *filename) {
    char prefix[] = "/dev/fs/"; /* 8 characters */
    int i;
    for (i = 0; i < 8; i++) {
        if (!filename[i]) {
            return 0;
        }
        if (filename[i] != prefix[i]) {
            return 0;
        }
    }
    return 1;
}
