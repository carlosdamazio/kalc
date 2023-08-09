# KalcOS
A kernel for a REPL calculator.

## Rationale
This is a toy project on how to build a Kernel to manage computing resources, not meant to be a full fledged production
one. Also this one is to sharpen my C skillzz. There will be a Rust version of this.

## Building and running
This is what you need in order to build and run this project:
- GCC
- make
- qemu

Using make, build the boot loader and kernel by running:
```bash
$ make
```
This will generate the `boot.efi` file that will be used as a boot loader for our kernel.
