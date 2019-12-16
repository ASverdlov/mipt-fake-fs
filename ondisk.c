#include "ondisk.h"

#include "stdlib.h"
#include "stdio.h"
#include "sys/stat.h"
#include "fcntl.h"

#define SUPERBLOCK_OFFSET 0

struct superblock_ondisk* init_superblock_ondisk() {
	struct superblock_ondisk* ondisk;

	ondisk = (struct superblock_ondisk*)malloc(sizeof(struct superblock_ondisk));
	if (ondisk <= 0) {
		printf("Failed to allocate superblock_ondisk");
		exit(1);
	}

	return ondisk;
}

int read_superblock_ondisk(FILE* f, struct superblock_ondisk* ondisk) {
	int err;
	int have_read;

	err = fseek(f, SUPERBLOCK_OFFSET, SEEK_SET);
	if (err != 0) {
		return err;
	}

    while((have_read = fread(ondisk, sizeof(struct superblock_ondisk), 1, f)) != 1) {
		/* pass */
	} 

	return 0;
}

void print_superblock_ondisk(struct superblock_ondisk* ondisk) {
	printf("Superblock info:\n");
	printf(" - magic: %d\n",     ondisk->magic);
	printf(" - size: %d\n",      ondisk->size);
	printf(" - blocksize: %d\n", ondisk->blocksize);
	printf(" - inodes_offset: %d\n", ondisk->inodes_offset);
}
