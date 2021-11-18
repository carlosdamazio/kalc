.PHONY: run clean

QEMU=/usr/local/bin/qemu-system-x86_64
BUILD_DIR=build
CFLAGS=-Wall -Werror --std=c11 -pedantic -o
SRC=src/*.c
NASM_FLAGS=-f bin -o
NASM_SRC=boot/*.asm


all: kernel.o boot.bin

kernel.o:
	$(CC) $(CFLAGS) $(BUILD_DIR)/$@ $(SRC)

boot.bin:
	nasm $(NASM_FLAGS) $(BUILD_DIR)/$@ $(NASM_SRC)

run: boot.bin
	$(QEMU) $(BUILD_DIR)/$?

clean:
	rm build/*
