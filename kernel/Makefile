CC = gcc
LD = ld

CFLAGS = -ffreestanding -fshort-wchar
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib

BINDIR := bin
SRCDIR := src
OBJDIR := lib

SRC := $(wildcard $(SRCDIR)/*.c)
OBJS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
LDS = kernel.ld
TARGET = kernel.elf

kernel: $(OBJS) $(TARGET)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo "[MAKE] Compiling $^..."
	$(CC) $(CFLAGS) -c $^ -o $@

$(TARGET):
	@echo "[MAKE] Linking into ELF file..."
	$(LD) $(LDFLAGS) $(OBJS) -o $(BINDIR)/$(TARGET)

clean:
	rm -f $(OBJDIR)/*
	rm -f $(BINDIR)/*

