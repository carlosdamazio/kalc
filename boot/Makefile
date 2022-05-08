LD=ld

ARCH    = x86_64
TARGET  = BOOTx64.efi
SRCS     = $(wildcard src/*.c)
OBJS     = $(patsubst %.c, %.o, $(SRCS))
STATIC  = src/boot.so

EFILIB  = gnu-efi
EFIINC := $(EFILIB)/inc
CFLAGS := -I$(EFIINC) \
		  -fno-stack-protector \
	  	  -fpic \
	  	  -fshort-wchar \
	  	  -mno-red-zone \
	  	  -Wall

LDFLAGS := -nostdlib \
		   -znocombreloc \
	       -shared \
		   -Bsymbolic \
		   -L$(EFILIB)/$(ARCH)/lib \
		   -L$(EFILIB)/$(ARCH)/gnuefi \
		   -T$(EFILIB)/gnuefi/elf_$(ARCH)_efi.lds \
		   $(EFILIB)/$(ARCH)/gnuefi/crt0-efi-$(ARCH).o \

OBJFLAGS = -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc \
		   --target efi-app-$(ARCH) --subsystem=10


all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(STATIC): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi

$(TARGET): $(STATIC)
	objcopy $(OBJFLAGS) $< $(TARGET)

clean:
	rm $(OBJS) $(STATIC)
