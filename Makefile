all: taller12

taller12: taller12.c
	gcc -Wall taller12.c -o taller12 -lpthread