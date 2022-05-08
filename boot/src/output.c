#include "output.h"

// NewFrameBuffer returns a new instance of a FrameBuffer from a given GOP
FrameBuffer*
NewFrameBuffer(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop)
{
    static FrameBuffer buff;
    buff.FrameBufferBase   = (void*)         gop->Mode->FrameBufferBase;
    buff.FrameBufferSize   = (unsigned long) gop->Mode->FrameBufferSize;
    buff.HorizontalRes     = (unsigned int)  gop->Mode->Info->HorizontalResolution;
    buff.VerticalRes       = (unsigned int)  gop->Mode->Info->VerticalResolution;
    buff.PixelsPerScanLine = (unsigned int)  gop->Mode->Info->PixelsPerScanLine;
    return &buff;
}

// GetGOP returns a new instance of the Graphics Output Protocol (GOP)
EFI_GRAPHICS_OUTPUT_PROTOCOL*
GetGOP()
{
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

    EFI_STATUS status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL,
                                          (void **)&gop);
    if (EFI_ERROR(status)) 
        return NULL;

    return gop;
}

// GetVideoModesInfo returns a new instance of EnhancedVideoModeInfo
EnhancedVideoModeInfo*
GetVideoModesInfo(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop) 
{
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    EFI_STATUS status;
    UINTN SizeOfInfo;
    EnhancedVideoModeInfo *einfo;

    status = uefi_call_wrapper(gop->QueryMode, 4, gop, 
                               gop->Mode==NULL?0:gop->Mode->Mode, &SizeOfInfo,
                               &info);
    if (status == EFI_NOT_STARTED)
        status = uefi_call_wrapper(gop->SetMode, 2, gop, 0);
    if (EFI_ERROR(status)) 
		return NULL;
    else {
        einfo->info = info;
        einfo->info_sz = SizeOfInfo;
        einfo->native_mode = gop->Mode->Mode;
        einfo->num_modes = gop->Mode->MaxMode;
    }

    return einfo;
}

// PrintVideoModesInfo prints to the screen the video modes available
void
PrintVideoModesInfo(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop,
                    EnhancedVideoModeInfo *einfo) 
{
    
    for (UINTN i = 0; i < einfo->num_modes - 1; i++) {
        uefi_call_wrapper(gop->QueryMode, 4, gop, i, &einfo->info_sz, 
                                   &einfo->info);
        Print((CHAR16*) L"Mode %03d, Width %d, Height %d, Format %x %s\r\n",
                i,
                einfo->info->HorizontalResolution,
                einfo->info->VerticalResolution,
                einfo->info->PixelFormat,
                i == einfo->native_mode ? "x": "");
    } 
}

// GetFrameBufferFromMode returns a FrameBuffer according to a given mode
FrameBuffer*
GetFrameBufferFromMode(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop, UINTN mode)
{
    EFI_STATUS status = uefi_call_wrapper(gop->SetMode, 2, gop, mode);
    if (EFI_ERROR(status)) {
        return NULL;
    }
    return NewFrameBuffer(gop);
}

