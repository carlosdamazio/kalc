#include <efi.h>
#include <efilib.h>
#include <elf.h>

#define PSF_MAGIC0 0x36
#define PSF_MAGIC1 0x04

typedef struct { 
    unsigned char magic[2];
    unsigned char mode;
    unsigned char charsize;
} PSF1_Header;

typedef struct {
    PSF1_Header *psf_header;
    void       *glyph_buffer;
} PSF1_Font;

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


EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs_protocol;
EFI_LOADED_IMAGE_PROTOCOL       *loaded_image;


FrameBuffer*
NewFrameBuffer(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop)
{
    static FrameBuffer buff;
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
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

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

EFI_FILE*
LoadFile(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE Image)
{
    EFI_FILE *LoadedFile;
    EFI_GUID EfiImageProtocolGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_GUID EfiSimpleFsProtocolGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    if (loaded_image == NULL || fs_protocol == NULL) {
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


PSF1_Font*
LoadFont(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE ImageHandle)
{
    EFI_FILE *font = LoadFile(Directory, Path, ImageHandle); 
    if (font == NULL) {
        Print((CHAR16*) L"Font not loaded.\r\n");
        return NULL;
    }
    Print((CHAR16*) L"Loaded font file.\r\n");
    PSF1_Header *header;
    uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, sizeof(PSF1_Header), (void **)&header);
    UINTN header_size = sizeof(PSF1_Header);
    uefi_call_wrapper(font->Read, 3, font, &header_size, header);
    
    if (header->magic[0] != PSF_MAGIC0 || header->magic[1] != PSF_MAGIC1) {
        Print((CHAR16*) L"Magic 0: %x\r\n", header->magic[0]);
        Print((CHAR16*) L"Magic 1: %x\r\n", header->magic[1]);
        Print((CHAR16*) L"File has invalid magic number in header.\r\n");
        return NULL;
    }
    
    UINTN glyph_buffer_size = header->charsize * 256;
    if (header->mode == 1) {
        glyph_buffer_size = header->charsize * 512;
    }
    
    void *glyph_buffer;
    {
        uefi_call_wrapper(font->SetPosition, 2, font, sizeof(PSF1_Header));
        uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, glyph_buffer_size,
                (void **) &glyph_buffer);
        uefi_call_wrapper(font->Read, 3, font, &glyph_buffer_size, glyph_buffer);
    }

    PSF1_Font *built_font;
    uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, sizeof(PSF1_Font),
                (void **) &built_font);
    built_font->psf_header = header;
    built_font->glyph_buffer = glyph_buffer;
    return built_font;
}

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE Image, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(Image, SystemTable);

    CHAR16   *KernelName = (CHAR16 *)L"kernel.elf";
    EFI_FILE *Kernel;
    EFI_GUID FileInfoGuid = EFI_FILE_INFO_ID;
    
    // Load kernel file
    Kernel = LoadFile(NULL, KernelName, Image);

    if (Kernel == NULL) {
        Print((CHAR16 *)L"Kernel file couldn't be loaded\r\n");
        return EFI_ERROR((CHAR16 *)L"Kernel file couldn't be loaded\r\n"); 
    } else
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

    // Get Framebuffer
    FrameBuffer *buff = NewFrameBuffer(gop);
    Print((CHAR16*) L"Got framebuffer.\r\n");

    // Load font file
    PSF1_Font *font = LoadFont(NULL, (CHAR16 *) L"zap-light16.psf", Image);
    if (font == NULL) {
        Print((CHAR16*) L"Couldn't get font file: either is not valid or not found.\r\n");
        return EFI_ERROR(1);
    }
    Print((CHAR16*) L"Font file loaded.\r\n");

    
    void (*KernelStart)(FrameBuffer*, PSF1_Font*) = ((__attribute__((sysv_abi)) void (*)(FrameBuffer*, PSF1_Font*) ) header.e_entry);
    KernelStart(buff, font);

    return EFI_SUCCESS;
}

