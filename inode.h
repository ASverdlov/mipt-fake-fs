#ifndef INODE_H
#define INODE_H

#include "assert.h"

#include "block.h"
#include "fs.h"
#include "utility.h"

#define MAX_BLOCKS_NUM 64

struct inode_ondisk {
	int permissions;
	int links;
	int user_id;
	int group_id;
	int size;

	time_t access_time;
	time_t modification_time;

	int blocks[MAX_BLOCKS_NUM];
};

struct inode_ondisk* init_inode_ondisk(struct fs_description* fs);

struct inode {
	struct inode_ondisk* ondisk;
	int id;
	struct fs_description* fs;
};

struct inode* create_inode(struct fs_description* fs, int permissions);
struct inode* init_inode(struct fs_description* fs, int id);
int read_inode(struct inode* i);
int save_inode(struct inode* i);

int load_inode_content(struct inode* i, void* where, int size);
int load_inode_content_to_stream(struct inode* i, int size, FILE* to);

int save_inode_content(struct inode* i, void* from, int size);
int save_inode_content_from_stream(struct inode* i, FILE* from);

char is_directory_inode(struct fs_description* fs, int inode_id);


#endif
