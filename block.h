#ifndef BLOCK_H
#define BLOCK_H

#include "fs.h"

struct bitmap_description {
	char* bitmap;
};

void init_bitmap(struct fs_description* fs);
void read_bitmap(struct fs_description* fs);
void create_bitmap(struct fs_description* fs);
int save_bitmap(struct fs_description* fs);

void deallocate_block(struct fs_description* fs, int block);
int allocate_internal(struct fs_description* fs, int start, int end);
int allocate_inode(struct fs_description* fs);
int allocate_inode_block(struct fs_description* fs);

void print_allocation_stats(struct fs_description* fs);

#endif
