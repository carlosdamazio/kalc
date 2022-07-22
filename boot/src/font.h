#include <efi.h>
#include <efilib.h>

#include "file.h"

#define PSF_MAGIC0 0x36
#define PSF_MAGIC1 0x04

typedef struct {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_header;

typedef struct {
	PSF1_header *psf_header;
	void        *glyph_buffer;
} PSF1_font;

PSF1_font* load_font(EFI_FILE *Directory, CHAR16 *Path, EFI_HANDLE ImageHandle);
