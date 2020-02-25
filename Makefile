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
$(IMG): ipl.bin
	mformat -f 1440 -C -B $< -i $@ ::

ipl.bin: ipl.s ipl.ld
	$(CC) -o $@ -nostdlib -T ipl.ld ipl.s

clean:
	$(RM) $(IMG) ipl.bin