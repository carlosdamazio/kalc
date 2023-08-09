# kalc
A kernel for a REPL calculator.

## Rationale
This is a toy project on how to build a Kernel to manage computing resources, not meant to be a full fledged production
one. Also this one is to sharpen my C skillzz. There will be a Rust version of this.

## Building and running
This is what you need in order to build and run this project:
- GCC
- make
- qemu

Using make, just run the application and then clean up everything with:
```bash
$ make all
```
This is used to generate a binary, but this is application level. When I progress in building a kernel, the building
phase is going to be totally changed.
