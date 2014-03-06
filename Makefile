CC = gcc
CPP = g++
CFLAGS += -g -pedantic -Wall -Wextra
CXXFLAGS += -g -Wall -Wextra

EXECS = sh map
SRCS = memory/map.c memory/memory.c shell/sh.c shell/shell-lib.c util/util.c util/date.c
VPATH = shell memory


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

include memory/Makefile

include shell/Makefile

include test/Makefile
