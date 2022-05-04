	.extern main
	.extern printf_init
	.global start
start:
	.extern heap_init
	call heap_init
	call printf_init
	call main

	push %eax
loop:
	call exit
	jmp loop
