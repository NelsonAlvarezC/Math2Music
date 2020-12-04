all:
	nasm -f elf64 -g -F dwarf ./math_n_music.asm
	gcc ./main.c ./py_interface.c ./filewave.c ./math_n_music.o -g -no-pie -lm -I/usr/include/python3.8 -lpython3.8 -o ./math_n_music
