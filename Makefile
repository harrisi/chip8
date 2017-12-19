CC=gcc
CFLAGS=-g
LDFLAGS=-lncurses
SOURCES=src/*.c
EXECUTABLE=chip8

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):
	@mkdir -p bin
	$(CC) $(SOURCES) -o bin/$(EXECUTABLE) $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf bin/*
