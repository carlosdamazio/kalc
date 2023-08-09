#include <efi.h>
#include <efilib.h>

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
   InitializeLib(ImageHandle, SystemTable);
   Print(L"[INFO] Hello, KalcOS\n");
   return EFI_SUCCESS;
}

