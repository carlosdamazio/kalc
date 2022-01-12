typedef unsigned long long size_t;

typedef struct {
    void* base_address;
    size_t buffer_sz;
    unsigned int width;
    unsigned int height;
    unsigned int pixels_per_scan_line;
} Framebuffer;

void kprint(int colour, const char *string)
{
    volatile char *vga_buff = (volatile char*) 0xB8000;

    while (*string != 0) { 
        *vga_buff++ = *string++;
        *vga_buff++ = colour;
    }
}

unsigned int _start(Framebuffer *buff)
{
    return buff->width;
}
