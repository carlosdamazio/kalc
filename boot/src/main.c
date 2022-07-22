#include <efi.h>
#include <efilib.h>
#include <elf.h>

#include "file.h"
#include "font.h"
#include "output.h"

UINTN KeyEvent = 0;

// efi_main executes the bootloader
EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE image, EFI_SYSTEM_TABLE *st)
{
	InitializeLib(image, st);
	Print((CHAR16*) L"[BOOT] Initialized boot services\r\n");

	CHAR16   *kernel_file_name  = (CHAR16 *)L"kernel.elf";

	EFI_FILE *kernel = load_file(NULL, kernel_file_name, image);
	if (kernel == NULL)
		return EFI_ERROR((CHAR16 *)L"kernel file couldn't be loaded\r\n");
	else
		Print((CHAR16 *)L"[BOOT] Kernel file loaded successfully\r\n");

	Elf64_Ehdr *header = get_elf_header(kernel);
	if (is_valid_elf_header(header))
		return EFI_ERROR((CHAR16 *) L"kernel file has invalid headers");
	else
		Print((CHAR16 *)L"[BOOT] Kernel has valid headers\r\n");

	Elf64_Phdr *phdrs = get_program_headers(kernel, header);
	allocate_segments(kernel, phdrs, header);

	Print((CHAR16 *)L"[BOOT] Kernel file is loaded\r\n");

	EFI_GRAPHICS_OUTPUT_PROTOCOL *gop = get_gop();
	if (gop == NULL)
		return EFI_ERROR((CHAR16*) L"couldn't get gop");
	Print((CHAR16*) L"[BOOT] Graphics Output Protocol enabled\r\n");

	frame_buffer *buff = new_frame_buffer(gop);
	Print((CHAR16*) L"[BOOT] Framebuffer recovered\r\n");

	PSF1_font *font = load_font(NULL, (CHAR16*) L"zap-light16.psf", image);
	if (font == NULL)
		return EFI_ERROR((CHAR16*) L"font couldn't be loaded");
	Print((CHAR16*) L"[BOOT] Font file loaded\r\n");

	void (*entry)(frame_buffer*, PSF1_font*) = ((__attribute__((sysv_abi)) void (*)(frame_buffer*, PSF1_font*) ) header->e_entry);
	Print((CHAR16*) L"[BOOT] Kernel entrypoint loaded\r\n");
	uefi_call_wrapper(st->BootServices->Stall, 1, 5000000);

	Print((CHAR16*) L"[BOOT] Handing over the runtime to the kernel\r\n");
	entry(buff, font);
	return EFI_SUCCESS;
}

