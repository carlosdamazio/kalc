#include <efi.h>
#include <efilib.h>
#include <elf.h>

int mem_cmp(const void *aptr, const void *bptr, int n)
{
    const unsigned char *a = aptr, *b = bptr;
    for (int i = 0; i < n; i++) {
        if (a[i] < b[i]) return -1;
        else if (a[i] > b[i]) return 1;
    }
    return 0;
}

EFI_FILE *LoadFile(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE Image)
{
    EFI_FILE *LoadedFile = NULL;
    EFI_LOADED_IMAGE_PROTOCOL *LoadedImage = NULL;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = NULL;
    EFI_GUID EfiImageProtocolGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_GUID EfiSimpleFsProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    // Load everything
    uefi_call_wrapper(BS->HandleProtocol, 3, Image, &EfiImageProtocolGuid,
            (void **)&LoadedImage);
    uefi_call_wrapper(BS->HandleProtocol, 3, LoadedImage->DeviceHandle,
            &EfiSimpleFsProtocolGuid, (void **)&FileSystem);
    
    if (Directory == NULL)
        uefi_call_wrapper(FileSystem->OpenVolume, 2, FileSystem, &Directory);

    EFI_STATUS ret = uefi_call_wrapper(Directory->Open, 5, Directory,
            &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    
    if (ret != EFI_SUCCESS)
        return NULL;
  
    return LoadedFile;
}

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE Image, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(Image, SystemTable);

    CHAR16   *KernelName = (CHAR16 *)L"kernel.elf";
    EFI_FILE *Kernel = NULL;
    EFI_GUID FileInfoGuid = EFI_FILE_INFO_ID;
    
    // Load kernel file
    Kernel = LoadFile(NULL, KernelName, Image);

    if (Kernel == NULL)
        Print((CHAR16 *)L"Kernel file couldn't be loaded\r\n");
    else
        Print((CHAR16 *)L"Kernel file loaded successfully\r\n");

    // Get ELF header and validate it
    Elf64_Ehdr header;
    {
        UINTN FileInfoSize;
        EFI_FILE_INFO *FileInfo;
        uefi_call_wrapper(Kernel->GetInfo, 4, Kernel, &FileInfoGuid, &FileInfoSize, 
                NULL);
        uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, FileInfoSize,
                (void **)&FileInfo);
        uefi_call_wrapper(Kernel->GetInfo, 4, Kernel, &FileInfoGuid, &FileInfoSize,
                (void **)&FileInfo);

        UINTN size = sizeof(header);
        uefi_call_wrapper(Kernel->Read, 3, Kernel, &size, &header);
    }

    if (mem_cmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
            header.e_ident[EI_CLASS] != ELFCLASS64 || 
            header.e_ident[EI_DATA] != ELFDATA2LSB ||
            header.e_type != ET_EXEC ||
            header.e_machine != EM_X86_64 ||
            header.e_version != EV_CURRENT)
        Print((CHAR16 *)L"Kernel has invalid headers\r\n");
    else
        Print((CHAR16 *)L"Kernel has valid headers\r\n");
   
    // Get program header
    Elf64_Phdr *phdrs;
    {
        uefi_call_wrapper(Kernel->SetPosition, 2, Kernel, header.e_phoff);
        UINTN size = header.e_phnum * header.e_phentsize;
        uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, size, 
                (void **)&phdrs);
        uefi_call_wrapper(Kernel->Read, 3, Kernel, &size, phdrs);
    }

    for (Elf64_Phdr *phdr = phdrs;
            (char *)phdr < (char *)phdrs + header.e_phnum * header.e_phentsize;
            phdr = (Elf64_Phdr *)((char *)phdr + header.e_phentsize)) {
        switch (phdr->p_type) {
            case PT_LOAD: {
                int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
                Elf64_Addr segment = phdr->p_paddr;
                uefi_call_wrapper(BS->AllocatePages, 4, AllocateAddress, 
                        EfiLoaderData, pages, &segment);

                uefi_call_wrapper(Kernel->SetPosition, 2, Kernel,
                        phdr->p_offset);
                UINTN size = phdr->p_filesz;
                uefi_call_wrapper(Kernel->Read, 3, Kernel, &size,
                        (void *)segment);
                break;
            }
        }
    }

    Print((CHAR16 *)L"Kernel is loaded\r\n");

    void (*KernelStart)() = ((__attribute__((sysv_abi)) void (*)() ) header.e_entry);
    KernelStart();
    return EFI_SUCCESS;
}

