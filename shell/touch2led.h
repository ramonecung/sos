#ifndef TOUCH2LED_H
#define TOUCH2LED_H

#include <stdio.h>

#ifndef TEST_SHELL
#define ostrm STDOUT
#define istrm STDIN
#define estrm STDERR
#endif

#ifdef TEST_SHELL
int cmd_touch2led(int argc, char *argv[], FILE *ostrm);
#else
int cmd_touch2led(int argc, char *argv[]);
#endif

#endif
