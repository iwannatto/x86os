.text

.globl  load_eflags
load_eflags:
    pushfl
    popl    %eax
    ret

.globl  store_eflags
store_eflags:
    movl    4(%esp), %eax
    pushl   %eax
    popfl
    ret

.globl  in8
in8:
    movw    4(%esp), %dx
    movl    $0, %eax
    inb     %dx, %al
    ret

.globl  out8
out8:
    movw    4(%esp), %dx
    movb    8(%esp), %al
    outb    %al, %dx
    ret

.globl  asm_inthandler21
asm_inthandler21:
    pushw   %es
    pushw   %ds
    pushal
    movl    %esp, %eax
    pushl   %eax
    movw    %ss, %ax
    movw    %ax, %ds
    movw    %ax, %es
    call    inthandler21
    popl    %eax
    popal
    popw    %ds
    popw    %es
    iret

.globl  asm_inthandler2c
asm_inthandler2c:
    pushw   %es
    pushw   %ds
    pushal
    movl    %esp, %eax
    pushl   %eax
    movw    %ss, %ax
    movw    %ax, %ds
    movw    %ax, %es
    call    inthandler2c
    popl    %eax
    popal
    popw    %ds
    popw    %es
    iret
