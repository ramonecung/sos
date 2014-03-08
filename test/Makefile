GTEST_DIR = third-party/gtest-1.7.0

TESTS = test-util test-date test-shell test-memory
TEST_MEMORY_DEPS = test/memory.cpp libgtest.a memory.o util.o
TEST_SHELL_DEPS = test/shell.cpp libgtest.a test-shell.o date.o util.o
TEST_DATE_DEPS = test/date.cpp libgtest.a date.o util.o
TEST_UTIL_DEPS = test/util.cpp libgtest.a util.o

.PHONY : clean-tests
clean-tests :
	rm -f $(TESTS) libgtest.a

test : $(TESTS)
	./test-util
	./test-date
	./test-shell
	./test-memory


test-shell-run : test-shell
	./test-shell

test-memory-run : test-memory
	./test-memory

test-memory : $(TEST_MEMORY_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_MEMORY_DEPS) -o $@

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

