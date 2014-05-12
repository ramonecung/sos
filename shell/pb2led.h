#ifndef PB2LED_H
#define PB2LED_H

#include <stdio.h>

#ifndef TEST_SHELL
#define ostrm STDOUT
#define istrm STDIN
#define estrm STDERR
#endif

#ifdef TEST_SHELL
int cmd_pb2led(int argc, char *argv[], FILE *ostrm);
#else
int cmd_pb2led(int argc, char *argv[]);
#endif

#endif
