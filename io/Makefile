
io-standalone.o : io.c
	$(CC) $(CFLAGS) -c io/io.c -U K70 -o $@

io.o : io.c
	$(CC) $(CFLAGS) -c io/io.c -D K70 -o $@

io_uart.o : io_uart.c
	$(CC) $(CFLAGS) -c io/io_uart.c -D K70 -o $@

io_led.o : io_led.c

io_button.o : io_button.c

io_fs.o : io_fs.c
