;ELF64 syscall registers: [rax][rdi,rsi,rdx,rcx,r8,r9]

section     .data
msg     db  'Hello, world!',0x0a,0x00           ;our dear string

section     .text
global      _start                              ;must be declared for linker (ld)
_start:                                         ;entry point
    mov     rdx,14                              ;message length
    mov     rsi,msg                             ;message to write
    mov     rdi,1                               ;file descriptor (stdout)
    mov     rax,1                               ;system call number (sys_write)
	syscall                                     ;call kernel

    mov     rdi,0
    mov     rax,60                              ;system call number (sys_exit)
	syscall                                     ;call kernel