MEMORY_DEPS = memory.o util.o

CFLAGS += -Wno-zero-length-array

memory.o : memory.c
	$(CC) $(CFLAGS) -c memory/memory.c -o $@

map : map.o memory.o svc.o
