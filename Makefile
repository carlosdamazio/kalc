.PHONY: build clean

CFLAGS=-Wall -Werror --std=c11 -pedantic
SRC=src/*.c
OUT=-o kalc


build:
	$(CC) $(CFLAGS) $(OUT) $(SRC)

run:
	./kalc

clean:
	rm -rf kalc

all: build run clean

