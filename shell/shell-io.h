#ifndef SHELL_IO_H
#define SHELL_IO_H

#ifdef TEST_SHELL
int cmd_create(int argc, char *argv[], FILE *ostrm);
int cmd_delete(int argc, char *argv[], FILE *ostrm);
#else
int cmd_create(int argc, char *argv[]);
int cmd_delete(int argc, char *argv[]);
#endif

#endif
