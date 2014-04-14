#include "../include/constants.h"
#include "io_fs.h"

int filename_valid(char *filename) {
    char *basename;
    if (!prefix_valid(filename)) {
        return 0;
    }
    basename = filename + 8;
    if (!basename_valid(basename)) {
        return 0;
    }
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

int basename_valid(char *basename) {
    /*
     * restrict basename to POSIX Fully portable filenames according to
     * http://en.wikipedia.org/wiki/Filename#Comparison_of_filename_limitations
     * which means:
     * max length 14
     * A–Z a–z 0–9 . _ -
     * hyphen must not be first character
     */
    char *cp;
    int count;

    cp = basename;
    if (!*cp) {
        return 0;
    }
    if (*cp == '-') {
        return 0;
    }
    count = 0;
    while (*cp) {
        if (!valid_basename_character(*cp++)) {
            return 0;
        }
        if (++count > 14) {
            return 0;
        }
    }
    return 1;
}

int valid_basename_character(char c) {
    if (c >= 'A' && c <= 'Z') {
        return 1;
    }
    if (c >= 'a' && c <= 'z') {
        return 1;
    }
    if (c >= '0' && c <= '9') {
        return 1;
    }
    if (c == '.' || c == '_' || c == '-') {
        return 1;
    }
    return 0;
}
