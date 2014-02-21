CC = gcc
CPP = g++
CFLAGS += -pedantic -Wall -Wextra
CXXFLAGS += -Wall -Wextra
GTEST_DIR = third-party/gtest-1.7.0

SRCS = shell/sh.c shell/shell-lib.c util/util.c util/date.c

EXECS = sh
SHELL_DEPS = sh.o shell-lib.o date.o util.o

TESTS = test-util test-date test-shell
TEST_SHELL_DEPS = test/shell.cpp libgtest.a test-shell.o date.o util.o
TEST_DATE_DEPS = test/date.cpp libgtest.a date.o util.o
TEST_UTIL_DEPS = test/util.cpp libgtest.a util.o

all : $(EXECS)
.PHONY : all

test : $(TESTS)
	./test-util
	./test-date
	./test-shell

.PHONY : clean
clean :
	rm -f $(TESTS) $(EXECS) *.o libgtest.a

# autmatically pull in dependencies on included header files
# copied from http://stackoverflow.com/a/2394668/1424966
depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^ >>./.depend
include .depend

sh : $(SHELL_DEPS)
	$(CC) $(CFLAGS) $(SHELL_DEPS) -o $@

sh.o : shell/sh.c
	$(CC) $(CFLAGS) -c shell/sh.c -o $@

shell-lib.o : shell/shell-lib.c
		$(CC) $(CFLAGS) -c shell/shell-lib.c -o $@

date.o : util/date.c
	$(CC) $(CFLAGS) -c util/date.c -o $@

util.o : util/util.c
	$(CC) $(CFLAGS) -c util/util.c -o $@


test-shell : $(TEST_SHELL_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread -D TEST_SHELL \
$(TEST_SHELL_DEPS) -o $@

test-shell.o : shell/shell-lib.c
	$(CC) $(CFLAGS) -D TEST_SHELL -c shell/shell-lib.c -o $@


test-date : $(TEST_DATE_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_DATE_DEPS) -o $@


test-util : $(TEST_UTIL_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_UTIL_DEPS) -o $@


# Google Test
libgtest.a : gtest-all.o
	ar -rv libgtest.a gtest-all.o

gtest-all.o :
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
-pthread -c ${GTEST_DIR}/src/gtest-all.cc


