        .module crt0

        ; Ordering of segments for the linker.
        ; WRS: Note we list all our segments here, even though
        ; we don't use them all, because their ordering is set
        ; when they are first seen.

	; 0x9000 up : code
	.area _CODE
	.area _HOME
        .area _CONST
        .area _INITIALIZED
        .area _DATA
        .area _BSEG
        .area _BSS
        .area _HEAP
	; These get overwritten and don't matter
        .area _GSINIT      ; unused
        .area _GSFINAL     ; unused
	.area _PAGE0	   ; force the bin packer to leave it alone
	; Discard at top so we can turn into buffers
        .area _BUFFERS     ; _BUFFERS grows to consume all before it (up to KERNTOP)
        .area _DISCARD

	; 0x7200 up : mapped always for Kernel but not for user
        .area _COMMONMEM
	.area _STUBS

	; Banked 0x4000-0x67FF
	.area _CODE1

	; Banked 0x4000-0x67FF
	.area _CODE2


	; Map over ROM as we don't really use it
        .area _INITIALIZER ; binman copies this to the right place for us


        ; exported symbols
        .globl init

        ; imported symbols
        .globl _fuzix_main
        .globl init_hardware
        .globl s__INITIALIZER
        .globl s__COMMONMEM
        .globl l__COMMONMEM
        .globl s__DISCARD
        .globl l__DISCARD
        .globl s__DATA
        .globl l__DATA
        .globl kstack_top

	.globl interrupt_handler
	.globl nmi_handler

	.include "kernel.def"

;
;	We land here after the booter has done it's magic. The booter
;	runs in the top of the kernel bank where bss will end up
;
	.area _CODE
init:  
        di
	ld	sp,#kstack_top

	;	Clear the BSS (will blow away the boot loader)
	ld	hl,#s__DATA
	ld	de,#s__DATA+1
	ld	bc,#l__DATA-1
	ld	(hl),#0
	ldir

	; Clear the screen
	ld	hl,#0x7800
	ld	de,#0x7801
	ld	bc,#0x13FF
	ld	(hl),#0
	ldir

        ; Hardware setup
        call	init_hardware

        ; Call the C main routine
        call	_fuzix_main
    
        ; fuzix_main() shouldn't return, but if it does...
        di
stop:   halt
        jr	stop

	.area _STUBS
	.ds 600
