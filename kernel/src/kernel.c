#include "output.h"

unsigned int _start(FrameBuffer *fbuff)
{
    draw_line(fbuff, 50, 4, 0xff00ffff);
    return fbuff->PixelsPerScanLine;
}
