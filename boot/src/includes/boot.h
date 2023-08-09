#ifndef BOOT_H
#define BOOT_H
#include <font.h>
#include <graphics.h>
#endif

typedef struct {
	PSF1_Font    *psf1_font;
	FrameBuffer  *framebuffer;
    OutputCursor *cursor;
} BootInfo;
