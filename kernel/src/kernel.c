#include "output.h"

unsigned int _start(FrameBuffer *fbuff, PSF_Font *font)
{
    draw_line(fbuff, 50, 4, 0xff00ffff);
    return fbuff->PixelsPerScanLine;
}
