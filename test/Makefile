GTEST_DIR = third-party/gtest-1.7.0

TESTS = test-shell-io test-io test-io-uart test-io-fs test-io-button test-io-led \
	test-strings test-date test-shell test-memory test-process
TEST_PROCESS_DEPS = test/process.cpp libgtest.a process.o test-memory.o test-util.o strings.o
TEST_IO_DEPS = test/io.cpp libgtest.a io.o test-util.o strings.o test-memory.o
TEST_IO_UART_DEPS = test/io_uart.cpp libgtest.a io_uart.o test-util.o strings.o test-memory.o
TEST_IO_FS_DEPS = test/io_fs.cpp libgtest.a test-io-fs.o io.o test-util.o strings.o test-memory.o
TEST_IO_BUTTON_DEPS = test/io_button.cpp libgtest.a io_button.o test-util.o strings.o test-memory.o
TEST_IO_LED_DEPS = test/io_led.cpp libgtest.a io_led.o test-util.o strings.o test-memory.o
TEST_MEMORY_DEPS = test/memory.cpp libgtest.a test-memory.o test-util.o strings.o
TEST_SHELL_DEPS = test/shell.cpp libgtest.a test-shell.o test-shell-io.o \
	test-ser2lcd.o test-touch2led.o test-pot2ser.o test-therm2ser.o \
	test-pb2led.o test-settimer.o test-procdemo.o \
	test-memory.o test-util.o test-date.o strings.o convert.o
TEST_SHELL_IO_DEPS = test/shell-io.cpp libgtest.a test-shell-io.o test-shell.o \
	test-ser2lcd.o test-touch2led.o test-pot2ser.o test-therm2ser.o \
	test-pb2led.o test-settimer.o test-procdemo.o \
	test-util.o strings.o test-memory.o test-date.o
TEST_DATE_DEPS = test/date.cpp libgtest.a test-date.o test-util.o strings.o test-memory.o
TEST_STRINGS_DEPS = test/strings.cpp libgtest.a strings.o

.PHONY : clean-tests
clean-tests :
	rm -f $(TESTS) libgtest.a

test : $(TESTS)
	./test-strings
	./test-date
	./test-shell
	./test-shell-io
	./test-memory
	./test-io-fs
	./test-io-uart
	./test-io-led
	./test-io-button
	./test-io
	./test-process

test-process-run : test-process
	./test-process

test-io-run : test-io
	./test-io

test-io-uart-run : test-io-uart
	./test-io-uart

test-io-fs-run : test-io-fs
	./test-io-fs

test-io-button-run : test-io-button
	./test-io-button

test-io-led-run : test-io-led
	./test-io-led

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


test-process : $(TEST_PROCESS_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_PROCESS_DEPS) -o $@

test-io : $(TEST_IO_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_IO_DEPS) -o $@

test-io-uart : $(TEST_IO_UART_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_IO_UART_DEPS) -o $@

test-io-fs : $(TEST_IO_FS_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_IO_FS_DEPS) -o $@

test-io-fs.o : io/io_fs.c
	$(CC) $(CFLAGS) -D TEST_SHELL -c io/io_fs.c -o $@

test-io-led : $(TEST_IO_LED_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_IO_LED_DEPS) -o $@

test-io-button : $(TEST_IO_BUTTON_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_IO_BUTTON_DEPS) -o $@

test-memory : $(TEST_MEMORY_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread \
$(TEST_MEMORY_DEPS) -o $@


test-ser2lcd.o : shell/ser2lcd.c
	$(CC) $(CFLAGS) -D TEST_SHELL -Wno-unused-parameter -c shell/ser2lcd.c -o $@

test-touch2led.o : shell/touch2led.c
	$(CC) $(CFLAGS) -D TEST_SHELL -Wno-unused-parameter -c shell/touch2led.c -o $@

test-pot2ser.o : shell/pot2ser.c
	$(CC) $(CFLAGS) -D TEST_SHELL -Wno-unused-parameter -c shell/pot2ser.c -o $@

test-therm2ser.o : shell/therm2ser.c
	$(CC) $(CFLAGS) -D TEST_SHELL -Wno-unused-parameter -c shell/therm2ser.c -o $@

test-pb2led.o : shell/pb2led.c
	$(CC) $(CFLAGS) -D TEST_SHELL -Wno-unused-parameter -c shell/pb2led.c -o $@

test-settimer.o : shell/settimer.c
	$(CC) $(CFLAGS) -D TEST_SHELL -Wno-unused-parameter -c shell/settimer.c -o $@

test-procdemo.o : shell/procdemo.c
	$(CC) $(CFLAGS) -D TEST_SHELL -Wno-unused-parameter -c shell/procdemo.c -o $@

test-shell-io : $(TEST_SHELL_IO_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread -D TEST_SHELL \
$(TEST_SHELL_IO_DEPS) -o $@

test-shell-io.o : shell/shell-io.c
	$(CC) $(CFLAGS) -D TEST_SHELL -c shell/shell-io.c -o $@

test-shell : $(TEST_SHELL_DEPS)
	$(CPP) $(CXXFLAGS) -isystem ${GTEST_DIR}/include -pthread -D TEST_SHELL \
$(TEST_SHELL_DEPS) -o $@

test-shell.o : shell/shell-lib.c
	$(CC) $(CFLAGS) -D TEST_SHELL -Wno-unused-parameter -c shell/shell-lib.c -o $@

test-util.o : util/util.c
	$(CC) $(CFLAGS) -D TEST_SHELL -c util/util.c -o $@

test-memory.o : memory/memory.c
	$(CC) $(CFLAGS) -c memory/memory.c -D TEST_SHELL -o $@

test-date.o : util/date.c
	$(CC) $(CFLAGS) -D TEST_SHELL -c util/date.c -o $@

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

