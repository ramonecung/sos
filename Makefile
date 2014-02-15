CC = gcc
CPP = g++
CFLAGS += -g -pedantic -Wall -Wextra
CXXFLAGS += -g -Wall -Wextra
GTEST_DIR = third-party/gtest-1.7.0


TESTS = test-shell
EXECS = shell-main
SRCS = shell/shell.c

all : $(EXECS)

test : $(TESTS)
	./test-shell

clean :
	rm -f $(TESTS) $(EXECS) *.o libgtest.a

# autmatically pull in dependencies on included header files
# copied from http://stackoverflow.com/a/2394668/1424966
depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^ >>./.depend
include .depend

shell-main : shell-main.o shell.o util.o
	$(CC) $(CFLAGS) shell-main.o shell.o util.o -o $@

shell-main.o : shell/shell-main.c
	$(CC) $(CFLAGS) -c shell/shell-main.c -o $@

shell.o : shell/shell.c
	$(CC) $(CFLAGS) -c shell/shell.c -o $@

util.o : util/util.c
	$(CC) $(CFLAGS) -c util/util.c -o $@


test-shell : test/shell.cpp libgtest.a test-shell.o util.o
	g++ $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread -D TEST_SHELL test/shell.cpp libgtest.a \
test-shell.o util.o -o $@

test-shell.o : shell/shell.c
	$(CC) $(CFLAGS) -D TEST_SHELL -c shell/shell.c -o $@

libgtest.a : gtest-all.o
	ar -rv libgtest.a gtest-all.o

gtest-all.o :
	g++ $(CXXFLAGS) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
-pthread -c ${GTEST_DIR}/src/gtest-all.cc


