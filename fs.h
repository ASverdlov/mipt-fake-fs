#ifndef FS_H
#define FS_H

#include <stdio.h>

#include "ondisk.h"

static const int SUPERBLOCK_MAGIC = 8031996;
static const int HARDCODED_BLOCKSIZE = 4096;
static const int HARDCODED_INODES_BLOCKNUM = 1024;

struct superblock_description {
	struct fs_description* fs;
	struct superblock_ondisk* ondisk;
};

struct fs_description {
	FILE*                          device_file;
	struct superblock_description* superblock;
	struct bitmap_description* 	   bitmap;
};

struct superblock_description* fs_superblock_init(struct fs_description* fs);
int fs_superblock_write(struct superblock_description* sb);
struct fs_description* init_fs(char* device_path);
int read_fs(struct fs_description* fs, char* device_path);
int create_fs(char* device_path);
int offset_for_inode(struct fs_description* fs, int inode_id);

#endif
