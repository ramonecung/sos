MEMORY_DEPS = memory.o util.o

memory.o : memory.c
	$(CC) $(CFLAGS) -c memory/memory.c -D SOS -o $@

map : map.o memory.o
