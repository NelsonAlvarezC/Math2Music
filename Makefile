all:
	nasm -f elf64 -g -F dwarf ./test.asm
	gcc ./main.c ./test.o -g -no-pie -lm