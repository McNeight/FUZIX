
	.export __stbyte28

	.setcpu 8080
	.code
__stbyte28:
	mov a,l
	lxi h,28

	mov m,a
	mov l,a
	ret