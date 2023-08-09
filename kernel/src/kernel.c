#include "output.h"

unsigned int _start(FrameBuffer *fbuff, PSF1_Font *font)
{
    draw_line(fbuff, 50, 4, 0xff00ffff);
    return font->psf_header->charsize;
}
