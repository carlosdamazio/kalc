#ifndef OUTPUT_H
#define OUTPUT_H
#endif

typedef struct { 
    unsigned char magic[2];
    unsigned char mode;
    unsigned char charsize;
} PSF1_Header;

typedef struct {
    PSF1_Header *psf_header;
    void       *glyph_buffer;
} PSF1_Font;

typedef struct {
    void          *FrameBufferBase;
    unsigned long FrameBufferSize;
    unsigned int  HorizontalRes;
    unsigned int  VerticalRes;
    unsigned int  PixelsPerScanLine;
} FrameBuffer;

typedef struct {
    unsigned int x;
    unsigned int y;
} OutputCursor;

static OutputCursor output_cursor = {.x = 0, .y = 0};

void draw_line(FrameBuffer *buff, int y, int bpp, unsigned long long colour);
void put_char(FrameBuffer *buff, PSF1_Font *psf1_font, unsigned int colour, 
              char chr, unsigned int x_offset, unsigned int y_offset);
void print_string_line(FrameBuffer *buff, PSF1_Font *psf1_font, unsigned int colour,
                  const char *string);
