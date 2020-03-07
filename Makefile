# for development on MacOS
ifeq ($(shell uname),Darwin)
CC := x86_64-elf-gcc
CFLAGS := -m32 -march=i486
endif

IMG := x86os.img

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
$(IMG): ipl.bin entry.bin
	mformat -f 1440 -C -B ipl.bin -i $@ ::
	mcopy -i $@ entry.bin ::

ipl.bin: ipl.s ipl.ld
	$(CC) $(CFLAGS) -o $@ -nostdlib -T ipl.ld ipl.s

entry.bin: entry.s main.c entry.ld
	$(CC) $(CFLAGS) -o $@ -nostdlib -T entry.ld entry.s main.c

clean:
	$(RM) $(IMG) ipl.bin entry.bin