.code16
.text

hello:
    movw    $msg_hello, %si
    jmp     print

print:
    movb    (%si), %al
    addw    $1, %si
    cmpb    $0, %al
    je      fin
    movb    $0x0e, %ah
    movw    $15, %bx
    int     $0x10
    jmp     print

fin:
    hlt
    jmp     fin

msg_hello:
    .ascii  "\nentry.s\n\r\0"
