SHELL_DEPS = sh.o shell-lib.o date.o util.o

sh : $(SHELL_DEPS)
	$(CC) $(CFLAGS) $(SHELL_DEPS) -o $@

sh.o : sh.c
shell-lib.o : shell-lib.c

date.o : util/date.c
	$(CC) $(CFLAGS) -c util/date.c -o $@

util.o : util/util.c
	$(CC) $(CFLAGS) -c util/util.c -o $@
