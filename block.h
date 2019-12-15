#ifndef BLOCK_H
#define BLOCK_H

#include "fs.h"

struct bitmap_description {
	char* bitmap;
};

void init_and_read_bitmap(struct fs_description* fs);
int save_bitmap(struct fs_description* fs);

int allocate_internal(struct fs_description* fs, int start, int end);
int allocate_inode(struct fs_description* fs);
int allocate_inode_block(struct fs_description* fs);

#endif
