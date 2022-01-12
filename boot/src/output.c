#include <efi.h>
#include <efilib.h>

EFI_GRAPHICS_OUTPUT_PROTOCOL* GetGOP() {
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
