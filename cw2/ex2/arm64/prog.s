        .section .rodata

data:   .byte  47, 101, 116,  99
        .byte  47, 111, 115,  45
        .byte 114, 101, 108, 101
        .byte  97, 115, 101,   0
        .byte  73,  83,  69,  76

        .text
        .globl _start
_start:
        mov x?, #-100
        ldr x?, =data
        mov x?, #0
        mov w8, #??
        svc #0

        mov x21, x0

        mov x?, #2
        mov x?, #0
        mov x?, x21
        mov w8, #??
        svc #0

        mov x20, x0

        mov x?, #0
        mov x?, #0
        mov x?, #1
        mov x?, #2
        mov x?, x20
        mov x?, x21
        mov w8, #??
        svc #0

        mov x19, x0

        mov x?, x19
        mov x?, x20
        mov x?, #1
        mov w8, #??
        svc #0

        mov x?, #0
        mov w8, #??
        svc #0

        .end
