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

void
draw_line(FrameBuffer *buff, int y, int bbp, unsigned long long pixel);
