#include <efi.h>
#include <efilib.h>
#include <elf.h>

EFI_FILE*   load_file(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE Image);
Elf64_Phdr* get_program_headers(EFI_FILE *file, Elf64_Ehdr *header);
Elf64_Ehdr* get_elf_header(EFI_FILE *file);
void        allocate_segments(EFI_FILE *file, Elf64_Phdr *phdrs, Elf64_Ehdr *header);
int         is_valid_elf_header(Elf64_Ehdr *header);
