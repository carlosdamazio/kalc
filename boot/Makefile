LD=ld

SRC=boot.c
OBJ=boot.o
STATIC=boot.so 
TARGET=boot.efi

EFILIB=gnu-efi
CFLAGS=-Ignu-efi/inc -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone \
	   -maccumulate-outgoing-args
LDFLAGS=-shared -Bsymbolic -L$(EFILIB)/x86_64/lib -L$(EFILIB)/x86_64/gnuefi -T$(EFILIB)/gnuefi/elf_x86_64_efi.lds \
		$(EFILIB)/x86_64/gnuefi/crt0-efi-x86_64.o -lgnuefi -lefi

OBJFLAGS=-j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc \
		 --target efi-app-x86_64 --subsystem=10


all: $(TARGET)

$(OBJ):
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

$(STATIC): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(STATIC)

$(TARGET): $(STATIC)
	objcopy $(OBJFLAGS) $(STATIC) $(TARGET)

clean:
	rm $(OBJ) $(STATIC)
