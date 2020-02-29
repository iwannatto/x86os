# for development on MacOS
ifeq ($(shell uname),Darwin)
CC := x86_64-elf-gcc
endif

IMG := x86os.img

.PHONY: run clean

run: $(IMG)
	qemu-system-i386 -boot order=a -m size=32M \
		-drive file=$(IMG),if=floppy,index=0,format=raw

# -f: size
# -C: create
# -B: boot sector
# -i, :: -> drive letter
$(IMG): ipl.bin entry.bin
	mformat -f 1440 -C -B ipl.bin -i $@ ::
	mcopy -i $@ entry.bin ::

ipl.bin: ipl.s ipl.ld
	$(CC) -o $@ -nostdlib -T ipl.ld ipl.s

entry.bin: entry.s entry.ld
	$(CC) -o $@ -nostdlib -T entry.ld entry.s

clean:
	$(RM) $(IMG) ipl.bin entry.bin