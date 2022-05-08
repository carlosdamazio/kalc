#include <efi.h>
#include <efilib.h>
#include <elf.h>

static EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs_protocol;
static EFI_LOADED_IMAGE_PROTOCOL       *loaded_image;

EFI_FILE*   LoadFile(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE Image);
Elf64_Phdr* GetProgramHeaders(EFI_FILE *file, Elf64_Ehdr *header);
Elf64_Ehdr* GetElfHeader(EFI_FILE *file); 
void        AllocateSegments(EFI_FILE *file, Elf64_Phdr *phdrs, Elf64_Ehdr *header);
int         IsValidElfHeader(Elf64_Ehdr *header);

