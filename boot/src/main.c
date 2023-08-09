#include <efi.h>
#include <efilib.h>
#include <elf.h>

typedef struct {
    void          *FrameBufferBase;
    unsigned long FrameBufferSize;
    unsigned int  HorizontalRes;
    unsigned int  VerticalRes;
    unsigned int  PixelsPerScanLine;
} FrameBuffer;

typedef struct {
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN info_sz;
    UINTN native_mode;
    UINTN num_modes;
} EnhancedVideoModeInfo;


FrameBuffer buff;

FrameBuffer*
NewFrameBuffer(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop)
{
    buff.FrameBufferBase   = (void*) gop->Mode->FrameBufferBase;
    buff.FrameBufferSize   = (unsigned long) gop->Mode->FrameBufferSize;
    buff.HorizontalRes     = (unsigned int)  gop->Mode->Info->HorizontalResolution;
    buff.VerticalRes       = (unsigned int)  gop->Mode->Info->VerticalResolution;
    buff.PixelsPerScanLine = (unsigned int)  gop->Mode->Info->PixelsPerScanLine;
    return &buff;
}

EFI_GRAPHICS_OUTPUT_PROTOCOL*
GetGOP()
{
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = NULL;

    EFI_STATUS status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL,
                                          (void **)&gop);
    if (EFI_ERROR(status)) {
        Print((CHAR16*)L"Could not locate GOP\n");
    }
    return gop;
}

EnhancedVideoModeInfo*
GetVideoModesInfo(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop) 
{
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    EFI_STATUS status;
    UINTN SizeOfInfo;
    EnhancedVideoModeInfo *einfo = NULL;

    status = uefi_call_wrapper(gop->QueryMode, 4, gop, 
                               gop->Mode==NULL?0:gop->Mode->Mode, &SizeOfInfo,
                               &info);
    if (status == EFI_NOT_STARTED)
        status = uefi_call_wrapper(gop->SetMode, 2, gop, 0);
    if (EFI_ERROR(status)) {
        Print((CHAR16*)L"ERROR: Unable to get native mode");
    } else {
        einfo->info = info;
        einfo->info_sz = SizeOfInfo;
        einfo->native_mode = gop->Mode->Mode;
        einfo->num_modes = gop->Mode->MaxMode;
    }

    return einfo;
}

void
PrintVideoModesInfo(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop,
                    EnhancedVideoModeInfo *einfo) 
{
    
    for (UINTN i = 0; i < einfo->num_modes - 1; i++) {
        uefi_call_wrapper(gop->QueryMode, 4, gop, i, &einfo->info_sz, 
                                   &einfo->info);
        Print((CHAR16*) L"Mode %03d, Width %d, Height %d, Format %x %s\r\n",
                i,
                einfo->info->HorizontalResolution,
                einfo->info->VerticalResolution,
                einfo->info->PixelFormat,
                i == einfo->native_mode ? "x": "");
    } 
}

FrameBuffer*
GetFrameBufferFromMode(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop, UINTN mode)
{
    EFI_STATUS status = uefi_call_wrapper(gop->SetMode, 2, gop, mode);
    if (EFI_ERROR(status)) {
        Print((CHAR16*) L"ERROR: Could not set mode %03d", mode);
        return NULL;
    }
    return NewFrameBuffer(gop);
}


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

    Print((CHAR16 *)L"Kernel file is loaded\r\n");

    // Get GOP and look for video mode and set it up
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = GetGOP();
    if (gop == NULL) {
        return EFI_ERROR("Couldn't get GOP");
    }
    Print((CHAR16*) L"Got GOP.\r\n");
    FrameBuffer *buff = NewFrameBuffer(gop);
    Print((CHAR16*) L"Got framebuffer.\r\n");

    unsigned int (*KernelStart)(FrameBuffer*) = ((__attribute__((sysv_abi)) unsigned int (*)(FrameBuffer*) ) header.e_entry);
    Print((CHAR16*)L"Kernel result = %u\n", KernelStart(buff));

    return EFI_SUCCESS;
}

