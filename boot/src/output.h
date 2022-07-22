#include <efi.h>
#include <efilib.h>

typedef struct {
    void          *FrameBufferBase;
    unsigned long FrameBufferSize;
    unsigned int  HorizontalRes;
    unsigned int  VerticalRes;
    unsigned int  PixelsPerScanLine;
} FrameBuffer;

typedef struct {
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN info_sz;
    UINTN native_mode;
    UINTN num_modes;
} EnhancedVideoModeInfo;

FrameBuffer* NewFrameBuffer(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop);
EFI_GRAPHICS_OUTPUT_PROTOCOL* GetGOP();
FrameBuffer* GetFrameBufferFromMode(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop, UINTN mode);

