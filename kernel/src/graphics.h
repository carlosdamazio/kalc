#ifndef GRAPHICS_H
#define GRAPHICS_H

typedef struct {
    unsigned char magic[2];
    unsigned char mode;
    unsigned char charsize;
} PSF1_Header;

typedef struct {
    PSF1_Header *header;
    void        *glyph_buffer;
} PSF1_Font;

typedef struct {
    void          *base;
    unsigned long size;
    unsigned int  x_resolution;
    unsigned int  y_resolution;
    unsigned int  ppsl;
} FrameBuffer;

typedef struct {
    unsigned int x;
    unsigned int y;
} OutputCursor;

typedef struct {
	OutputCursor cursor;
	PSF1_Font    *font;
	FrameBuffer  *framebuffer;
} GraphicsRenderer;

static GraphicsRenderer Renderer;

void clear();
void draw_line(FrameBuffer *buff, int y, int bpp, unsigned long long colour);
void start_renderer(GraphicsRenderer *renderer, PSF1_Font *font, FrameBuffer *framebuffer);
void kprint(unsigned int colour, const char *string);

#endif
