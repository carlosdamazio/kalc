#include "file.h"

// IsValidElfHeader validates an ELF header
int
IsValidElfHeader(Elf64_Ehdr *header)
{
	return mem_cmp(&header->e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
            header->e_ident[EI_CLASS] != ELFCLASS64 || 
            header->e_ident[EI_DATA] != ELFDATA2LSB ||
            header->e_type != ET_EXEC ||
            header->e_machine != EM_X86_64 ||
            header->e_version != EV_CURRENT;
}

// GetElfHeader returns an ELF header from file
Elf64_Ehdr*
GetElfHeader(EFI_FILE *file)
{
    static Elf64_Ehdr header;
    UINTN size = sizeof(header);
    uefi_call_wrapper(file->Read, 3, file, &size, &header);
	return &header; 
}

// GetProgramHeaders returns a sequence of ELF program headers from file
Elf64_Phdr*
GetProgramHeaders(EFI_FILE *file, Elf64_Ehdr *header)
{
	Elf64_Phdr *phdrs;
	uefi_call_wrapper(file->SetPosition, 2, file, header->e_phoff);
	UINTN size = header->e_phnum * header->e_phentsize;
	uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, size, 
			(void **)&phdrs);
	uefi_call_wrapper(file->Read, 3, file, &size, phdrs);
	return phdrs;
}

// AllocateSegments allocates loadable program headers from file to memory
void
AllocateSegments(EFI_FILE *file, Elf64_Phdr *phdrs, Elf64_Ehdr *header)
{
    for (Elf64_Phdr *phdr = phdrs;
            (char *)phdr < (char *)phdrs + header->e_phnum * header->e_phentsize;
            phdr = (Elf64_Phdr *)((char *)phdr + header->e_phentsize)) {
        switch (phdr->p_type) {
            case PT_LOAD: {
				// Allocate loadable program segments
                int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
                Elf64_Addr segment = phdr->p_paddr;
                uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress, 
                        EfiLoaderData, pages, &segment);

                uefi_call_wrapper(file->SetPosition, 2, file,
                        phdr->p_offset);
                UINTN size = phdr->p_filesz;
                uefi_call_wrapper(file->Read, 3, file, &size,
                        (void *)segment);
                break;
            }
        }
    }
}

// LoadFile retrieves a file from the FS, even if SFS is not mounted yet
EFI_FILE*
LoadFile(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE Image)
{
    EFI_FILE *LoadedFile;

    if (loaded_image == NULL || fs_protocol == NULL) {
		EFI_GUID EfiImageProtocolGuid    = EFI_LOADED_IMAGE_PROTOCOL_GUID;
		EFI_GUID EfiSimpleFsProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
		uefi_call_wrapper(BS->HandleProtocol, 3, Image, &EfiImageProtocolGuid,
				(void **)&loaded_image);
		uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle,
			&EfiSimpleFsProtocolGuid, (void **)&fs_protocol);
	} 

    // Open root directory from volume
    if (Directory == NULL)
        uefi_call_wrapper(fs_protocol->OpenVolume, 2, fs_protocol, &Directory);

    EFI_STATUS ret = uefi_call_wrapper(Directory->Open, 5, Directory,
            &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    
    if (ret != EFI_SUCCESS)
        return NULL;
  
    return LoadedFile;
}
