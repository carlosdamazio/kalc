#ifndef OUTPUT_H
#define OUTPUT_H
#endif

typedef struct {
    void          *FrameBufferBase;
    unsigned long FrameBufferSize;
    unsigned int  HorizontalRes;
    unsigned int  VerticalRes;
    unsigned int  PixelsPerScanLine;
} FrameBuffer;

void
draw_line(FrameBuffer *buff, int y, int bbp, unsigned long long pixel);
