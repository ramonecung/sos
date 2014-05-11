MEMORY_DEPS = memory.o util.o

memory.o : memory.c
	$(CC) $(CFLAGS) -c memory/memory.c -o $@

map : map.o memory.o
