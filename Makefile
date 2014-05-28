CC = gcc
CPP = g++
CFLAGS += -pedantic -Wall -Wextra
CXXFLAGS += -Wall -Wextra

EXECS = sh map
SRCS = memory/map.c memory/memory.c shell/sh.c shell/shell-lib.c util/util.c util/date.c util/strings.c util/convert.c \
io/io.c io/io_fs.c io/io_button.c io/io_led.c

VPATH = init memory shell io


.PHONY : all
all : $(EXECS)

.PHONY : clean
clean : clean-tests
	rm -f $(EXECS) *.o

# autmatically pull in dependencies on included header files
# copied from http://stackoverflow.com/a/2394668/1424966
depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^ >>./.depend
include .depend

include io/Makefile

include memory/Makefile

include shell/Makefile

include init/Makefile

include test/Makefile
