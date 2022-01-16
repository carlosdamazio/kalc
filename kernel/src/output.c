#include "output.h"

void
draw_line(FrameBuffer *buff, int y, int bbp, unsigned long long pixel)
{
    for (unsigned int x = 0; x < buff->HorizontalRes / 2 * bbp; x += 4)
        *(unsigned int*)(x + (y * buff->PixelsPerScanLine * bbp) + buff->FrameBufferBase) = pixel;
}

void
kput_char(FrameBuffer *buff, PSF1_Font, unsigned int colour, char chr, unsigned int x_off)

void
kprint(FrameBuffer *buff, PSF1_Font *font, unsigned int colour, char *string)
{
    
}
