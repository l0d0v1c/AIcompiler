    .section __TEXT,__text,regular,pure_instructions
    .globl _main
    .p2align 2
_main:
    adrp    x0, L_str0@PAGE
    add     x0, x0, L_str0@PAGEOFF
    bl      _puts
    mov w0, 0
    ret

    .section __TEXT,__cstring,cstring_literals
L_str0:
    .asciz "Hello World!"
