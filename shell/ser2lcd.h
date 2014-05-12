#ifndef SER2LCD_H
#define SER2LCD_H

#include <stdio.h>

#define CHAR_EOF 4

#ifndef TEST_SHELL
#define ostrm STDOUT
#define istrm STDIN
#define estrm STDERR
#endif

#ifdef TEST_SHELL
int cmd_ser2lcd(int argc, char *argv[], FILE *ostrm);
#else
int cmd_ser2lcd(int argc, char *argv[]);
#endif

#endif
