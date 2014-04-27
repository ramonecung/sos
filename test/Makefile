GTEST_DIR = third-party/gtest-1.7.0

TESTS = test-shell-io test-io test-io-fs test-io-button test-io-led \
	test-button-flash \
	test-strings test-date test-shell test-memory
TEST_IO_DEPS = test/io.cpp libgtest.a io_constants.o io.o util.o strings.o memory.o
TEST_IO_FS_DEPS = test/io_fs.cpp libgtest.a io_constants.o io_fs.o util.o strings.o memory.o
TEST_IO_BUTTON_DEPS = test/io_button.cpp libgtest.a io_constants.o io_button.o util.o strings.o memory.o
TEST_IO_LED_DEPS = test/io_led.cpp libgtest.a io_constants.o io_led.o util.o strings.o memory.o
TEST_BUTTON_FLASH_DEPS = test/button_flash.cpp libgtest.a button_flash.o memory.o util.o strings.o
TEST_MEMORY_DEPS = test/memory.cpp libgtest.a memory.o util.o strings.o
TEST_SHELL_DEPS = test/shell.cpp libgtest.a test-shell.o io_constants.o test-shell-io.o memory.o util.o date.o strings.o convert.o
TEST_SHELL_IO_DEPS = test/shell-io.cpp libgtest.a io_constants.o test-shell-io.o test-shell.o util.o strings.o memory.o date.o
TEST_DATE_DEPS = test/date.cpp libgtest.a date.o util.o strings.o memory.o
TEST_STRINGS_DEPS = test/strings.cpp libgtest.a strings.o memory.o util.o

.PHONY : clean-tests
clean-tests :
	rm -f $(TESTS) libgtest.a

test : $(TESTS)
	./test-strings
	./test-date
	./test-shell
	./test-shell-io
	./test-memory
	./test-button-flash
	./test-io-fs
	./test-io-led
	./test-io-button
	./test-io

test-io-run : test-io
	./test-io

test-io-fs-run : test-io-fs
	./test-io-fs

test-io-button-run : test-io-button
	./test-io-button

test-io-led-run : test-io-led
	./test-io-led

test-button-flash-run : test-button-flash
	./test-button-flash

test-shell-run : test-shell
	./test-shell

test-shell-io-run : test-shell-io
	./test-shell-io

test-memory-run : test-memory
	./test-memory

test-date-run : test-date
	./test-date

test-strings-run : test-strings
	./test-strings


test-io : $(TEST_IO_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_IO_DEPS) -o $@

test-io-fs : $(TEST_IO_FS_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_IO_FS_DEPS) -o $@

test-io-led : $(TEST_IO_LED_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_IO_LED_DEPS) -o $@

test-io-button : $(TEST_IO_BUTTON_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_IO_BUTTON_DEPS) -o $@

test-button-flash : $(TEST_BUTTON_FLASH_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_BUTTON_FLASH_DEPS) -o $@

test-memory : $(TEST_MEMORY_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_MEMORY_DEPS) -o $@

test-shell-io : $(TEST_SHELL_IO_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread -D TEST_SHELL \
$(TEST_SHELL_IO_DEPS) -o $@

test-shell-io.o : shell/shell-io.c
	$(CC) $(CFLAGS) -D TEST_SHELL -c shell/shell-io.c -o $@

test-shell : $(TEST_SHELL_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread -D TEST_SHELL \
$(TEST_SHELL_DEPS) -o $@

test-shell.o : shell/shell-lib.c
	$(CC) $(CFLAGS) -D TEST_SHELL -c shell/shell-lib.c -o $@

test-date : $(TEST_DATE_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_DATE_DEPS) -o $@


test-strings : $(TEST_STRINGS_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_STRINGS_DEPS) -o $@


# Google Test
libgtest.a : gtest-all.o
	ar -rv libgtest.a gtest-all.o

gtest-all.o :
	$(CPP) $(CXXFLAGS) -Wno-missing-field-initializers \
-isystem ${GTEST_DIR}/include -I${GTEST_DIR} \
-pthread -c ${GTEST_DIR}/src/gtest-all.cc

