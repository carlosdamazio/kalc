#include <efi.h>
#include <efilib.h>

typedef struct {
    void          *frame_buffer_base;
    unsigned long size;
    unsigned int  horizontal_res;
    unsigned int  vertical_res;
    unsigned int  ppsl;
} frame_buffer;

frame_buffer* new_frame_buffer(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop);
EFI_GRAPHICS_OUTPUT_PROTOCOL* get_gop();

