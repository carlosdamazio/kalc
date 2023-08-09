#include "ktypes.h"
#include "output.h"

void
plot_line(Framebuffer *buff, int y, u32 bbp, u32 pixel)
{
    for (u32 x = 0; x < buff->width / 2 * bbp; x++)
        *(u32*)(x + (y * buff->pixels_per_scan_line * bbp) + buff->base_address) = pixel;
}
