#include "output.h"

volatile int *vga_buff = (int *) 0xB8000;

void kprint(const char *str)
{
    for (int i = 0; str[i] != '\0'; i++) 
        write_char((unsigned char)str[i], 'F', '8', 20+i, 40);
}

void write_char(unsigned char c, unsigned char forecolour,
        unsigned char backcolour, int x, int y)
{
    int attrib = (backcolour << 4) | (forecolour & 0x0F);
    volatile int *location;
    location = vga_buff + (y * 80 + x);
    *location = c | (attrib << 8);
}
