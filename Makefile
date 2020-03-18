# for development on MacOS
ifeq ($(shell uname),Darwin)
CC := x86_64-elf-gcc
AS := x86_64-elf-as
LD := x86_64-elf-ld
endif
CFLAGS := -m32 -march=i486
ASFLAGS := --32 -march=i486

IMG := x86os.img
OS_SRCS := entry.o main.o asm.o graphic.o hankaku.o interrupt.o pic.o util.o

.PHONY: run clean

# default machine is
# pc-i440fx-4.1: Standard PC (i440FX + PIIX, 1996) (default)
run: $(IMG)
	qemu-system-i386 -boot order=a -m size=32M \
		-drive file=$(IMG),if=floppy,index=0,format=raw

debug: $(IMG)
	qemu-system-i386 -boot order=a -m size=32M \
		-drive file=$(IMG),if=floppy,index=0,format=raw \
		-s -S

# -f: size
# -C: create
# -B: boot sector
# -i, :: -> drive letter
$(IMG): ipl.bin x86os.bin
	mformat -f 1440 -C -B ipl.bin -i $@ ::
	mcopy -i $@ x86os.bin ::

ipl.bin: ipl.s ipl.ld
	$(CC) $(CFLAGS) -o $@ -nostdlib -T ipl.ld ipl.s

x86os.bin: $(OS_SRCS) x86os.ld
	$(LD) -o $@ -nostdlib -T x86os.ld $(OS_SRCS)

# this is wrong
# because hankaku.py does not generate "include "hankaku.h"" in hankaku.c
#hankaku.c: hankaku.txt hankaku.py
#	python3 hankaku.py

clean:
	$(RM) $(IMG) *.bin *.o
