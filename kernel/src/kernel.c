#include "output.h"


unsigned int _start(Framebuffer *buff)
{
    plot_pixel(buff, 32, 32, 255);
    return buff->width;
}
