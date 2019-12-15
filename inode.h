#ifndef INODE_H
#define INODE_H

#include "assert.h"

#include "block.h"
#include "fs.h"
#include "utility.h"

struct inode_ondisk {
	int permissions;
	int links;
	int user_id;
	int group_id;
	int size;

	int access_time;
	int modification_time;

	int num_blocks;
	int blocks[12]; // TODO: hardcode max number of blocks
};

struct inode_ondisk* init_inode_ondisk(struct fs_description* fs);
struct inode {
	struct inode_ondisk* ondisk;
	int id;
	struct fs_description* fs;
};

struct inode* init_inode(struct fs_description* fs, int id);
int read_inode(struct inode* i);
char is_directory_inode(struct fs_description* fs, int inode_id);
int load_inode_content(struct inode* i, void* where, int size);
int save_inode_content(struct inode* i, void* from, int size);

#endif
