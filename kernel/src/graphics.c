#include "graphics.h"

// Changes cursor line
void
put_new_line()
{
    Renderer.cursor.x = 0;
	Renderer.cursor.y += 16;
}

// Draw a single line
void
draw_line(FrameBuffer *buff, int y, int bpp, unsigned long long colour)
{
    for (unsigned int x = 0; x < buff->x_resolution / 2 * bpp; x += 4)
        *(unsigned int*)(x + (y * buff->ppsl * bpp) + buff->base) = colour;
}

// Prints a single character in a certain colour in x and y offset from the
// starting position
void
put_char(unsigned int colour, char chr, unsigned int x_offset, unsigned int y_offset)
{
    unsigned int *pixel = (unsigned int*) Renderer.framebuffer->base;
    char *font = (char*)(Renderer.font->glyph_buffer + (chr * Renderer.font->header->charsize));

    // iterate over y and x with its offsets plus padding
    for (unsigned long y = y_offset; y < y_offset + 16; y++) {
        for (unsigned long x = x_offset; x < x_offset + 8; x++) {
            // This checks if current font hex value in the bitmap
            // is greater than 0 through masking 1 bit value shifted
            // x - x_offset times so it knows when to fill the address with
            // the colour value.
            if ((*font & (0b10000000 >> (x - x_offset))) > 0)
                *(unsigned int*)(pixel + x + (y * Renderer.framebuffer->ppsl)) = colour;
        }

        font++;
    }
    draw_line(Renderer.framebuffer, 50, 4, 0xffffffff);
}

// Prints a string from the starting cursor position
void
kprint(unsigned int colour, const char *string)
{
    char *chr = (char*) string;
    while (*chr != 0) {
        put_char(colour, *chr, Renderer.cursor.x, Renderer.cursor.y);
        Renderer.cursor.x += 8;
        // Wrap long line
        if (Renderer.cursor.x + 8 > Renderer.framebuffer->x_resolution)
            put_new_line();
        chr++;
    }
}

// Prints a string from the starting cursor position, and jumps a column after finishing
void
kprintln(unsigned int colour, const char *string)
{
    kprint(colour, string);
	// Jump to next line
    put_new_line();
}

// Clean screen
void
clear()
{
    unsigned int *pixel = (unsigned int*) Renderer.framebuffer->base;
	for (
		unsigned int i = 0;
		i < Renderer.framebuffer->x_resolution * Renderer.framebuffer->y_resolution;
		i++
	)
		*(unsigned int*)(pixel + i) = 0x00000000;
}
