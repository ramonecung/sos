SHELL_DEPS = sh.o shell-lib.o memory.o date.o util.o strings.o \
shell-io.o io.o io_led.o io_button.o io_fs.o

sh : $(SHELL_DEPS)
	$(CC) $(CFLAGS) $(SHELL_DEPS) -o $@

sh.o : sh.c
shell-lib.o : shell-lib.c
shell-io.o : shell-io.c

date.o : util/date.c
	$(CC) $(CFLAGS) -c util/date.c -o $@

strings.o : util/strings.c
	$(CC) $(CFLAGS) -c util/strings.c -o $@

util.o : util/util.c
	$(CC) $(CFLAGS) -c util/util.c -o $@
