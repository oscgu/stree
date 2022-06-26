CC = cc
CFLAGS = -pedantic -Wall -Os -s -Wno-deprecated-declarations -std=c99 -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
DEPS = config.h
OBJ = stree.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

stree: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -rf *.o
