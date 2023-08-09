void kprint(int colour, const char *string)
{
    volatile char *vga_buff = (volatile char*) 0xB8000;

    while (*string != 0) { 
        *vga_buff++ = *string++;
        *vga_buff++ = colour;
    }
}

void _start()
{
    kprint(0xF, "Hello, world!");
}
