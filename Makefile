CC=gcc
CFLAGS=-g
LDFLAGS=-lncurses
SOURCES=src/*.c
EXECUTABLE=chip8

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCES) -o bin/$(EXECUTABLE)

clean:
	rm bin/*
