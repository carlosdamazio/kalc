#include "output.h"

// Changes cursor line
void
put_new_line()
{
    output_cursor.x = 0;
    output_cursor.y += 16;
}

// draw_line draws line to the top of the screen with the desired colour. 
void
draw_line(FrameBuffer *buff, int y, int bpp, unsigned long long colour)
{
    for (unsigned int x = 0; x < buff->HorizontalRes / 2 * bpp; x += 4)
        *(unsigned int*)(x + (y * buff->PixelsPerScanLine * bpp) + buff->FrameBufferBase) = colour;
}

// put_char prints a single character in a certain colour in x and y offset from the
// starting position
void
put_char(FrameBuffer *buff, PSF1_Font *psf1_font, unsigned int colour, char chr,
         unsigned int x_offset, unsigned int y_offset)
{
    unsigned int *pixel = (unsigned int*) buff->FrameBufferBase;
    char *font = psf1_font->glyph_buffer + (chr * psf1_font->psf_header->charsize);

    // iterate over y and x with its offsets plus padding
    for (unsigned long y = y_offset; y < y_offset + 16; y++) {
        for (unsigned long x = x_offset; x < x_offset + 8; x++) {
            // This checks if current font hex value in the bitmap
            // is greater than 0 through masking 1 bit value shifted 
            // x - x_offset times so it knows when to fill the address with
            // the colour value.
            if ((*font & (0b10000000 >> (x - x_offset))) > 0)
                *(unsigned int*)(pixel + x + (y * buff->PixelsPerScanLine)) = colour;
        }

        font++;
    }
}

// print_string_line prints a string from the starting cursor position
void
print_string_line(FrameBuffer *buff, PSF1_Font *psf1_font, unsigned int colour,
             const char *string)
{
    char *chr = (char*) string;
    while (*chr != 0) {
        put_char(buff, psf1_font, colour, *chr, output_cursor.x, output_cursor.y);
        output_cursor.x += 8;
        // Wrap long line
        if (output_cursor.x + 8 > buff->HorizontalRes)
            put_new_line();
        chr++;
    } 
    // Jump to next line
    put_new_line();
}

