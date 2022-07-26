#include "boot.h"

void
initialize_graphics(BootInfo *boot)
{
	Renderer.framebuffer = boot->framebuffer;
	Renderer.font        = boot->psf1_font;
	Renderer.cursor      = (OutputCursor) {
		.x = 0,
		.y = 0
	};
}

void _start(BootInfo *boot)
{
	initialize_graphics(boot);
	kprint(0xffffffff, "Hello, world");
	for (;;) {}
}
