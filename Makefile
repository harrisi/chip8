CC=gcc
CFLAGS=-g
LDFLAGS=-lncurses
SOURCES=src/*.c
EXECUTABLE=chip8

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):
	@mkdir -p bin
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o bin/$(EXECUTABLE)

clean:
	rm -rf bin/*
