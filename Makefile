CC = gcc
CPP = g++
CFLAGS += -g -pedantic -Wall -Wextra
CXXFLAGS += -g -Wall -Wextra
GTEST_DIR = third-party/gtest-1.7.0


TESTS = test-util test-date test-shell
EXECS = sh
SRCS = shell/sh.c shell/shell-lib.c util/util.c util/date.c

all : $(EXECS)

test : $(TESTS)
	./test-util
	./test-date
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

sh : sh.o shell-lib.o date.o util.o
	$(CC) $(CFLAGS) sh.o shell-lib.o date.o util.o -o $@

sh.o : shell/sh.c
	$(CC) $(CFLAGS) -c shell/sh.c -o $@

shell-lib.o : shell/shell-lib.c
	$(CC) $(CFLAGS) -c shell/shell-lib.c -o $@

date.o : util/date.c
	$(CC) $(CFLAGS) -c util/date.c -o $@

util.o : util/util.c
	$(CC) $(CFLAGS) -c util/util.c -o $@


test-shell : test/shell.cpp libgtest.a test-shell.o date.o util.o
	g++ $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread -D TEST_SHELL \
test/shell.cpp libgtest.a test-shell.o date.o util.o -o $@

test-shell.o : shell/shell-lib.c
	$(CC) $(CFLAGS) -D TEST_SHELL -c shell/shell-lib.c -o $@


test-date : test/date.cpp libgtest.a date.o util.o
	g++ $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
test/date.cpp libgtest.a date.o util.o -o $@


test-util : test/util.cpp libgtest.a util.o
	g++ $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
test/util.cpp libgtest.a util.o -o $@



libgtest.a : gtest-all.o
	ar -rv libgtest.a gtest-all.o

gtest-all.o :
	g++ $(CXXFLAGS) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
-pthread -c ${GTEST_DIR}/src/gtest-all.cc


