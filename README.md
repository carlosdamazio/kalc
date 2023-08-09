## Rationale
This is a toy project on how to build a Kernel to manage computing resources, not meant to be a full fledged production
one. Also this one is to sharpen my C skillzz. There will be a Rust version of this.

## Building and running
This is what you need in order to build and run this project:
- Qemu - Emulator for our Kernel.
- Nasm - Assembler to compile our .asm files.
- Gcc - The GNU Compiler Collection, to compile our .c files when we make the transition from Assembly to C.
- make - Automation tool to execute the compiling steps necessary to build the kernel.
- edk2-ovmf - OVMF shared libraries for executing UEFI applications in QEMU.
- cdrkit - Contains tool to create ISO files.

Using your favourite package manager, install these dependencies in order to get this thing running:
```bash
$ sudo pacman -S qemu nasm gcc make edk2-ovmf cdrkit
```
After that, clone the gnu-efi repository into the `boot/gnu-efi` folder and build its libraries.

```bash
$ git clone https://git.code.sf.net/p/gnu-efi/code boot/gnu-efi
$ make -C boot/gnu-efi
```

Using `make`, build the boot loader and kernel by running `make` and `make run` with sudo to execute
the image in QEMU, because of the OVMF libraries being located in a folder owned by root:
```bash
$ make
$ sudo make run
```

This will generate the `KalcOS.iso` file in `build/` directory, of which is the image to be ran on QEMU.

To see what's going to be developed, please refer to the TODO.md file in root directory folder
or check the issues tab in GitHub repo.
