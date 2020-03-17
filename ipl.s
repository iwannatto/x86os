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

# entry point of ipl
entry:
    # init of registers
    movw    $0, %ax
    movw    %ax, %ss
    movw    $0x7c00, %sp
    movw    %ax, %ds

# read from disk using int $0x13 (disk I/O)
# to address 0x08200 ~
# .if successful, jump to entry.s
# .else, jump to error

    # initialize

    # constant value:
    # %dl = index of drive
    movb    $0, %dl
    # %bx = destination address & 0xf (strictly different)
    movw    $0, %bx

    # initial value:
    # %ch = index of cylinder (strictly different)
    movb    $0, %ch
    # %dh = index of head
    movb    $0, %dh
    # %cl = index of sector (strictly different)
    movb    $2, %cl
    # %es = destination address >> 4
    movw    $(0x08200 >> 4), %ax
    movw    %ax, %es

readsector:
    # %si = count of failure
    movw    $0, %si
try:
    # read
    # %ah = 0x02(read)
    # %al = number of sector to process
    movb    $0x02, %ah
    movb    $1, %al
    int     $0x13

    # .if successful, jump to nextsector
    jnc     nextsector

    # .if too many errors(++%si >= 5), jump to error
    addw    $1, %si
    cmpw    $5, %si
    jae     error

    # reset
    # %ah = 0x00 (reset)
    # %dh = index of drive
    movb    $0x00, %ah
    movb    $0x00, %dl
    int     $0x13

    # retry
    jmp     try

nextsector:
    # %es is (destination address >> 4)
    # destination address += 512
    movw    %es, %ax
    addw    $(512 >> 4), %ax
    movw    %ax, %es

    # %cl is sector
    # .if (++%cl <= 18), continue to read sector
    addb    $1, %cl
    cmpb    $18, %cl
    jbe     readsector
    # .else, %cl = 1
    movb    $1, %cl

    # %dh is head
    # .if (++%dh < 2), continue to read sector
    addb    $1, %dh
    cmpb    $2, %dh
    jb      readsector
    # .else, %dh = 0
    movb    $0, %dh

    # %ch is cylindar
    # .if (++%ch < 10), continue to read sector
    addb    $1, %ch
    cmpb    $10, %ch
    jb      readsector
    # .else, jump to entry
    # 0xc200 = 0x8000 + 0x4200
    # 0x8000 is destination of ipl read
    # 0x4200 is offset of entry.s in floppy disk
    # it can be calculated by boot sector info
    # 512(reserved) + 2*9*512(FAT) + 224*32(root directory) = 0x4200
    jmp     0xc200

# print "error" and hlt
error:
    movw    $msg_error, %si
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
msg_error:
    .asciz "\nerror\n\r"

# to indicate that this is master boot record
    .org    0x1fe, 0x00 # 0x1fe is offset of boot signature
    .byte   0x55, 0xaa  # boot signature
