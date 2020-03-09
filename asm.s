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

.globl  out8
out8:
    movw    4(%esp), %dx
    movb    8(%esp), %al
    outb    %al, %dx
    ret
