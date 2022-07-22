#include "output.h"

// new_frame_buffer() returns a new instance of a FrameBuffer from a given GOP
frame_buffer*
new_frame_buffer(EFI_GRAPHICS_OUTPUT_PROTOCOL *gop)
{
	static frame_buffer buff;
	buff.frame_buffer_base = (void*)         gop->Mode->FrameBufferBase;
	buff.size              = (unsigned long) gop->Mode->FrameBufferSize;
	buff.horizontal_res    = (unsigned int)  gop->Mode->Info->HorizontalResolution;
	buff.vertical_res      = (unsigned int)  gop->Mode->Info->VerticalResolution;
	buff.ppsl              = (unsigned int)  gop->Mode->Info->PixelsPerScanLine;
	return &buff;
}

// get_gop() returns a new instance of the Graphics Output Protocol (GOP)
EFI_GRAPHICS_OUTPUT_PROTOCOL*
get_gop()
{
	EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

	EFI_STATUS status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL,
                                          (void **)&gop);
	if (EFI_ERROR(status))
		return NULL;

	return gop;
}
