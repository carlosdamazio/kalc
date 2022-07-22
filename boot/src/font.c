#include "font.h"

// get_PSF1_header() retrieves the PSF1 Header fom a font file
static PSF1_header*
get_PSF1_header(EFI_FILE *font)
{
	PSF1_header *header;
	{
		uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, sizeof(PSF1_header), (void **)&header);
		UINTN header_size = sizeof(PSF1_header);
		uefi_call_wrapper(font->Read, 3, font, &header_size, header);
	}
	return header;
}

// new_glyph_buffer() instances a new glyph_buffer
static void*
new_glyph_buffer(EFI_FILE *font, UINTN size)
{
	void *glyph_buffer;
	uefi_call_wrapper(font->SetPosition, 2, font, sizeof(PSF1_header));
	uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, size,
			(void **) &glyph_buffer);
	uefi_call_wrapper(font->Read, 3, font, &size, glyph_buffer);
	return glyph_buffer;
}

// validate_PSF1_header() validates a PSF1 Header
static int
validate_PSF1_header(PSF1_header *header)
{
	if (header->magic[0] != PSF_MAGIC0 || header->magic[1] != PSF_MAGIC1)
		return 1;
	return 0;
}

// load_font() returns a new PSF1 Font instance from a given directory, path and image handle
PSF1_font*
load_font(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE ImageHandle)
{
	EFI_FILE *font = load_file(Directory, Path, ImageHandle);
	if (font == NULL) {
		Print((CHAR16*) L"Font not loaded.\r\n");
		return NULL;
	}

	PSF1_header *header = get_PSF1_header(font);
	if (validate_PSF1_header(header))
		return NULL;

	UINTN glyph_buffer_size = header->charsize * 256;
	if (header->mode == 1) {
		glyph_buffer_size = header->charsize * 512;
	}

	void *glyph_buffer = new_glyph_buffer(font, glyph_buffer_size);
	if (glyph_buffer == NULL)
		return NULL;

	PSF1_font *built_font;
	{
		uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, sizeof(PSF1_font),
			(void **) &built_font);
	}
	built_font->psf_header = header;
	built_font->glyph_buffer = glyph_buffer;
	return built_font;
}

