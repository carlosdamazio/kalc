#include "output.h"

void _start(FrameBuffer *buff, PSF1_Font *font)
{
    for (int i = 0; i < 50; i++)
        print_string_line(buff, font, 0xFFFFFFFF, "Hello, world!");

    for (;;) {}
}
