EFIDIR=boot
BUILDDIR=build

all: boot.efi

boot.efi: | $(BUILDDIR)
	@echo "[MAKE] Building UEFI application for boot loader..."
	$(MAKE) -C $(EFIDIR)
	mv $(EFIDIR)/$@ $(BUILDDIR)/$@
	$(MAKE) -C $(EFIDIR) clean

$(BUILDDIR):
	@echo "[MAKE] Preparing build directories..."
	mkdir $(BUILDDIR)

