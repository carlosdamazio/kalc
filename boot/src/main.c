#include <efi.h>
#include <efilib.h>
#include <elf.h>

#include "file.h"
#include "font.h"
#include "output.h"

// memcmp implementation
int mem_cmp(const void *aptr, const void *bptr, int n)
{
    const unsigned char *a = aptr, *b = bptr;
    for (int i = 0; i < n; i++) {
        if (a[i] < b[i]) return -1;
        else if (a[i] > b[i]) return 1;
    }
    return 0;
}

// efi_main executes the bootloader
EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE Image, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(Image, SystemTable);
	Print((CHAR16*) L"[BOOT] Initialized boot services.\r\n");
    
	EFI_FILE *Kernel;
	CHAR16   *KernelName  = (CHAR16 *)L"kernel.elf";
    EFI_GUID FileInfoGuid = EFI_FILE_INFO_ID;
    
    Kernel = LoadFile(NULL, KernelName, Image);
    if (Kernel == NULL)
        return EFI_ERROR((CHAR16 *)L"kernel file couldn't be loaded\r\n"); 
    else {
        Print((CHAR16 *)L"[BOOT] Kernel file loaded successfully.\r\n");
	}

	Elf64_Ehdr *header = GetElfHeader(Kernel);
    if (IsValidElfHeader(header))
        return EFI_ERROR((CHAR16 *) L"kernel file has invalid headers");
    else {
        Print((CHAR16 *)L"[BOOT] Kernel has valid headers.\r\n");
	}

    Elf64_Phdr *phdrs = GetProgramHeaders(Kernel, header);
	AllocateSegments(Kernel, phdrs, header);	

    Print((CHAR16 *)L"[BOOT] Kernel file is loaded\r\n");

    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = GetGOP();
    if (gop == NULL) 
        return EFI_ERROR((CHAR16*) L"couldn't get gop");
    Print((CHAR16*) L"[BOOT] Graphics Output Protocol enabled.\r\n");

    FrameBuffer *buff = NewFrameBuffer(gop);
    Print((CHAR16*) L"[BOOT] Framebuffer recovered.\r\n");

    PSF1_Font *font = LoadFont(NULL, (CHAR16*) L"zap-light16.psf", Image);
    if (font == NULL) {
        return EFI_ERROR((CHAR16*) L"font couldn't be loaded");
    }
    Print((CHAR16*) L"[BOOT] Font file loaded.\r\n");

   	Print((CHAR16*) L"[BOOT] Handing over the runtime to the kernel.\r\n");
    void (*KernelStart)() = ((__attribute__((sysv_abi)) void (*)() ) header->e_entry);
	KernelStart();

    return EFI_SUCCESS;
}

