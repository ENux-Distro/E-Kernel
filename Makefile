all:
	as --32 boot.S -o boot.o
	g++ -c kernel.cpp -o kernel.o -m32 -march=i686 -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector -fno-pie -fno-use-cxa-atexit -Wall -Wextra
	ld -m elf_i386 -T linker.ld -o E-Kernel boot.o kernel.o

run:
	qemu-system-x86_64 -kernel E-Kernel

clear:
	rm -rf *.o E-Kernel

clean:
	rm -rf *.o E-Kernel

tar:
	tar -czvf ../E-Kernel.tar .
	mv ../E-Kernel.tar .