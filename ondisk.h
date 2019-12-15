#ifndef ONDISK_H
#define ONDISK_H

#include "stdio.h"
#include "sys/stat.h"
#include "fcntl.h"

struct superblock_ondisk {
	int magic;
	int size;
	int blocksize;

	int bitmap_offset;

	int inodes_offset;
	int inodes_blocknum;

	int files_and_directories_offset;
	int files_and_directories_blocknum;
};

struct superblock_ondisk* init_superblock_ondisk();
int read_superblock_ondisk(FILE* f, struct superblock_ondisk* ondisk);
void print_superblock_ondisk(struct superblock_ondisk* ondisk);

#endif
