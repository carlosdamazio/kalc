#ifndef OUTPUT_H
#define OUTPUT_H
#endif

#include "ktypes.h"


typedef struct {
    void         *base_address;
    u64          buffer_sz;
    unsigned int width;
    unsigned int height;
    unsigned int pixels_per_scan_line;
} Framebuffer;

void plot_line(Framebuffer *buff, int x, int y, u32 pixel);
