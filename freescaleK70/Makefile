
button_flash.o : button_flash.c

io-standalone.o : io.c
	$(CC) $(CFLAGS) -c freescaleK70/io.c -U SOS -o $@

io.o : io.c
	$(CC) $(CFLAGS) -c freescaleK70/io.c -D SOS -o $@

io_led.o : io_led.c

io_button.o : io_button.c

io_fs.o : io_fs.c
