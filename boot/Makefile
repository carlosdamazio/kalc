LD=ld

ARCH=x86_64
SRC=main.c
OBJ=main.o
STATIC=main.so 
TARGET=main.efi

EFILIB=gnu-efi
EFIINC=$(EFILIB)/inc
CFLAGS= -I$(EFIINC) \
		-fno-stack-protector \
	  	-fpic \
	  	-fshort-wchar \
	  	-mno-red-zone \
	  	-Wall

LDFLAGS=-nostdlib \
		-znocombreloc \
	    -shared \
		-Bsymbolic \
		-L$(EFILIB)/$(ARCH)/lib \
		-L$(EFILIB)/$(ARCH)/gnuefi \
		-T$(EFILIB)/gnuefi/elf_$(ARCH)_efi.lds \
		$(EFILIB)/x86_64/gnuefi/crt0-efi-$(ARCH).o \

OBJFLAGS=-j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc \
		 --target efi-app-$(ARCH) --subsystem=10


all: $(TARGET)

$(OBJ):
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

$(STATIC): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(STATIC) -lefi -lgnuefi

$(TARGET): $(STATIC)
	objcopy $(OBJFLAGS) $(STATIC) $(TARGET)

clean:
	rm $(OBJ) $(STATIC)
