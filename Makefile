 .PHONY: main

main: main.c
	gcc -std=gnu11 -g -o fakefs utility.c inode.c block.c dir.c ondisk.c dirent.c fs.c cli.c main.c
