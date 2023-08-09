; boot/hello.asm
	org  0x7c00 ; Start the program in offset 7C00;
	bits 16		; We start in 16-bits mode

	mov ax, 0x00      ; Assign AX with offset 0x0
	mov es, ax		  ; Create a pointer to the register
	mov bp, message	  ; Assign BP with offset to string
	
	mov ah, 0x06	  ; Scroll page up to clear output
	mov al, 0x00      ; Clear entire window
	int 0x10		  ; Interrupt INT 10H call

	mov	ah, 0x13			 ; Graphics mode (EGA + minimum PC AT)
	mov	al, 0x01		     ; Write mode
	mov	bl, 0x02			 ; Color: light cyan
	mov	cx, [message_length] ; String length
	mov	dh, [row]			 ; Row position
	mov dl, [col]			 ; Column position
	int	0x10
	jmp $					 ; Never end this program

	message db "Hello, world!"       ; String written into memory with label 'message'
	message_length dw 0x0d           ; Hard-coded string length
	row dw 1					     ; Row in video output
	col dw 1					     ; Col in video output
	
	times 510 - ($-$$) db 0 ; initialize the first 510 bytes - size of the previous code with 0s
	dw	  0xaa55			; mark as a bootable device for BIOS in the last 2 bytes from sector


