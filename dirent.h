#ifndef DIRENT_H
#define DIRENT_H

#include "stdio.h"
#include "string.h"

#include "fs.h"
#include "inode.h"

struct dirent_ondisk {
	int inode_id : 2;
	char name[14];
};

struct dirent_ondisk* init_dirent_ondisk();

struct dirent {
	struct dirent_ondisk* ondisk;
	struct dirent* parent;
	struct fs_description* fs;
};

struct dirent* init_dirent(struct fs_description* fs, struct dirent* parent);

#endif
