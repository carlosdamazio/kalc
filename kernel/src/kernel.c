#include "boot.h"

void _start(BootInfo *boot)
{
    clear(boot->framebuffer);
    kprintln(boot->framebuffer, boot->psf1_font, 0xFFFFFFFF, "Hello, world!");
    __asm__("hlt");
}