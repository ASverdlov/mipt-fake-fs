.PHONY: main

main: main.c
	gcc -std=gnu11 -o fakefs main.c
