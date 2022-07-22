#include "font.h"

// GetPSF1Header retrieves the PSF1 Header fom a font file
PSF1_Header*
GetPSF1Header(EFI_FILE *font)
{
	PSF1_Header *header;
	{
		uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, sizeof(PSF1_Header), (void **)&header);
		UINTN header_size = sizeof(PSF1_Header);
		uefi_call_wrapper(font->Read, 3, font, &header_size, header);
	}
	return header;
}

// NewGlyphBuffer instances a new glyph_buffer
void*
NewGlyphBuffer(EFI_FILE *font, UINTN size)
{
	void *glyph_buffer;
	uefi_call_wrapper(font->SetPosition, 2, font, sizeof(PSF1_Header));
	uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, size,
			(void **) &glyph_buffer);
	uefi_call_wrapper(font->Read, 3, font, &size, glyph_buffer);
	return glyph_buffer;
}

// ValidatePSF1Header validates a PSF1 Header
int
ValidatePSF1Header(PSF1_Header *header)
{
	if (header->magic[0] != PSF_MAGIC0 || header->magic[1] != PSF_MAGIC1)
		return 1;
	return 0;
}

// LoadFont returns a new PSF1 Font instance from a given directory, path and image handle
PSF1_Font*
LoadFont(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE ImageHandle)
{
	EFI_FILE *font = LoadFile(Directory, Path, ImageHandle);
	if (font == NULL) {
		Print((CHAR16*) L"Font not loaded.\r\n");
		return NULL;
	}

	PSF1_Header *header = GetPSF1Header(font);
	if (ValidatePSF1Header(header))
		return NULL;

	UINTN glyph_buffer_size = header->charsize * 256;
	if (header->mode == 1) {
		glyph_buffer_size = header->charsize * 512;
	}

	void *glyph_buffer = NewGlyphBuffer(font, glyph_buffer_size);
	if (glyph_buffer == NULL)
		return NULL;

	PSF1_Font *built_font;
	{
		uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, sizeof(PSF1_Font),
			(void **) &built_font);
	}
	built_font->psf_header = header;
	built_font->glyph_buffer = glyph_buffer;
	return built_font;
}

