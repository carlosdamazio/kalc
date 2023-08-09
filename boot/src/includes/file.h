#ifndef FILE_H
#define FILE_H
#include <efi.h>
#include <efilib.h>

EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FS_PROTOCOL;
EFI_LOADED_IMAGE_PROTOCOL       *ROOT_IMAGE;
#endif

EFI_FILE* load_file(EFI_FILE *dir, CHAR16 *path, EFI_HANDLE image);
