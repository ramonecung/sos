#ifndef SHELL_IO_H
#define SHELL_IO_H

/* in production use only stdin, stdout, and stderr as I/O streams. */
/* in test use dynamic variable input and output streams */
#ifndef TEST_SHELL
#define ostrm STDOUT
#define istrm STDIN
#define estrm STDERR
#endif


#ifdef TEST_SHELL
int cmd_create(int argc, char *argv[], FILE *ostrm);
int cmd_delete(int argc, char *argv[], FILE *ostrm);
int cmd_fopen(int argc, char *argv[], FILE *ostrm);
int cmd_fclose(int argc, char *argv[], FILE *ostrm);
int cmd_fgetc(int argc, char *argv[], FILE *ostrm);
int cmd_fputc(int argc, char *argv[], FILE *ostrm);
int cmd_fputs(int argc, char *argv[], FILE *ostrm);
#else
int cmd_create(int argc, char *argv[]);
int cmd_delete(int argc, char *argv[]);
int cmd_fopen(int argc, char *argv[]);
int cmd_fclose(int argc, char *argv[]);
int cmd_fgetc(int argc, char *argv[]);
int cmd_fputc(int argc, char *argv[]);
int cmd_fputs(int argc, char *argv[]);
#endif

#endif
