#ifndef THERM2SER_H
#define THERM2SER_H

#include <stdio.h>

#ifndef TEST_SHELL
#define ostrm STDOUT
#define istrm STDIN
#define estrm STDERR
#endif

#ifdef TEST_SHELL
int cmd_therm2ser(int argc, char *argv[], FILE *ostrm);
#else
int cmd_therm2ser(int argc, char *argv[]);
#endif

#endif
