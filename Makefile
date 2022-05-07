QEMU=qemu-system-x86_64
MKISOFS := xorriso -as mkisofs

OS=KalcOS

OVMFDIR := /usr/share/edk2-ovmf/x64
EFIDIR := boot
KERNELDIR := kernel
BUILDDIR := build

KERNEL     := $(BUILDDIR)/kernel.elf
BOOTLOADER := $(BUILDDIR)/main.efi
IMAGE 	   := $(BUILDDIR)/$(OS).img
TARGET 	   := $(BUILDDIR)/$(OS).iso
FONT   	   := ./zap-light16.psf

QEMUARGS := -drive file=$(TARGET) -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="./OVMF_CODE.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="./OVMF_VARS.fd" -net none -vga cirrus

all: $(TARGET)

$(BUILDDIR):
	@echo "[MAKE] Creating build directory..."
	@mkdir $@

$(BOOTLOADER): |$(BUILDDIR)
	@echo "[MAKE] Building $@..."
	$(MAKE) -C $(EFIDIR)
	@mv $(EFIDIR)/main.efi $@

$(KERNEL): |$(BUILDDIR)
	@echo "[MAKE] Building $@..."
	$(MAKE) -C $(KERNELDIR)
	cd kernel-rs && cargo build
	mv ./kernel-rs/target/kernel/debug/kernel-rs $(BUILDDIR)/kernel.elf
	#mv $(KERNELDIR)/bin/kernel.elf $(BUILDDIR)

$(IMAGE): $(KERNEL) $(BOOTLOADER)
	@echo "[MAKE] Building $@..."
	dd if=/dev/zero of=$(IMAGE) bs=1M count=50
	mformat -i $(IMAGE) -f 1440
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -i $@ $(BOOTLOADER) ::/EFI/BOOT
	mcopy -i $@ $(EFIDIR)/startup.nsh :: 
	mcopy -i $@ $(KERNEL) ::
	mcopy -i $@ $(FONT) ::
		
$(TARGET): $(IMAGE)
	@echo "[MAKE] Building $@..."
	$(MKISOFS) -o $@ -iso-level 3 -V "UEFI" $(IMAGE) -e /$(OS).img -no-emul-boot	

clean:
	@echo "[MAKE] Cleaning build objects..."
	@rm -rf build/*
	@$(MAKE) -C $(KERNELDIR) clean
	@$(MAKE) -C $(EFIDIR) clean

run: $(TARGET)
	@echo "[MAKE] Running QEMU..."
	@$(QEMU) $(QEMUARGS) 
