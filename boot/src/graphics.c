#include <graphics.h>

FrameBuffer* new_framebuffer(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop)
{
    FrameBuffer *buff  = NULL;
    buff->base         = (void*)         gop->Mode->FrameBufferBase;
    buff->size         = (unsigned long) gop->Mode->FrameBufferSize;
    buff->x_resolution = (unsigned int)  gop->Mode->Info->HorizontalResolution;
    buff->y_resolution = (unsigned int)  gop->Mode->Info->VerticalResolution;
    buff->ppsl         = (unsigned int)  gop->Mode->Info->PixelsPerScanLine;
    return buff;
}

EFI_GRAPHICS_OUTPUT_PROTOCOL*
get_gop()
{
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

    EFI_STATUS status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL,
                                          (void **)&gop);
    if (EFI_ERROR(status)) {
        Print((CHAR16*)L"Could not locate GOP\n");
        return NULL;
    }
    return gop;
}
