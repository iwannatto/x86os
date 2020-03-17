.code16
.text

# set video mode
    movb    $0x00, %ah
    movb    $0x13, %al  # 320*200*8bit
    int     $0x10

# into protected mode

    # disable all interrupts to PIC
    # IMR(master) = 0xff
    movb    $0xff, %al
    outb    %al, $0x21
    nop
    # IMR(slave) = 0xff
    outb    %al, $0xa1

    # disable interrupts to CPU
    cli

    # enable A20 gate
    call    waitkbdout
    movb    $0xd1, %al
    outb    %al, $0x64
    call    waitkbdout
    movb    $0xdf, %al  # 0xdf is magic number for me...
    outb    %al, $0x60
    call    waitkbdout

    # set GDT
    lgdtl   gdtr
    # disable paging and enable protected mode
    movl    %cr0, %eax
    andl    $0x7fffffff, %eax
    orl     $0x00000001, %eax
    movl    %eax, %cr0
    jmp     pipelineflush

pipelineflush:

    # reset segment registers
    movw    $(1 << 3), %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
    movw    %ax, %ss

    # set IDT
    lidtl   idtr

    # jump to main in main.c and reset cs
    ljmpl   $(2 << 3), $main

# wait until data transmission to keyboard controller is allowed
waitkbdout:
    # read status register
    inb     $0x64, %al
    andb    $(1 << 1), %al
    # read keyboard data and ignore it
    inb     $0x60, %al
    # .if OK, return
    jnz     waitkbdout
    ret

# definition of GDT
.align 8
gdt:
    # null selector
    .word   0x0000, 0x0000, 0x0000, 0x0000
    # limit: 0xfffff000, base:0x00000000, R/W
    .word   0xffff, 0x0000, 0x9200, 0x00cf
    # limit: 0x7ffff, base:0x00000000, E/R
    .word   0xffff, 0x0000, 0x9a00, 0x0047
gdtr:
    .word   8*3-1   # limit (16bit)
    .int    gdt     # base address (32bit)

# definition of IDT
.align 8
.globl  idt
idt:
.rept   256
    .word   0x0000, 0x0000, 0x0000, 0x0000
.endr
idtr:
    .word   8*256-1
    .int    idt
