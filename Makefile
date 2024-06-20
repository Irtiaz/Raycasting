all:
	gcc -ansi -pedantic -Wall -Wextra *.c -lm -lSDL2

debug:
	gcc *.c -g3 -lm -lSDL2
