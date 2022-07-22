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

