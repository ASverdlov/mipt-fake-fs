 .PHONY: main

main: main.c
	gcc -std=gnu11 -g -o fakefs main.c
