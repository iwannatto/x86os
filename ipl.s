.code16
.text
    jmp entry

    # FAT12
    .byte   0x90
    .ascii  "x86os   "
    .word   512
    .byte   1
    .word   1
    .byte   2
    .word   224
    .word   2880
    .byte   0xf0
    .word   9
    .word   18
    .word   2
    .int    0
    .int    2880
    .byte   0, 0, 0x29
    .int    0xffffffff
    .ascii  "x86os      "
    .ascii  "FAT12   "
    .skip   18, 0x00

entry:
    # init of registers
    movw    $0, %ax
    movw    %ax, %ss
    movw    $0x7c00, %sp
    movw    %ax, %ds
    movw    %ax, %es

    # print "hello, world"
    movw    $msg, %si
putloop:
    movb    (%si), %al
    addw    $1, %si
    cmpb    $0, %al
    je      fin
    movb    $0x0e, %ah
    movw    $15, %bx
    int     $0x10
    jmp     putloop

fin:
    hlt
    jmp     fin

msg:
    .ascii  "\n\nhello, world\n\0"

    # master boot record
    .org    0x1fe, 0x00
    .byte   0x55, 0xaa

