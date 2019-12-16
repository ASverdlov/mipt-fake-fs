#ifndef DIR_H
#define DIR_H

#include "dirent.h"
#include "fs.h"

struct dir_ondisk {
	int num_entries;
	struct dirent_ondisk dirents[20]; // TODO: change size to something else
};

struct dir_ondisk* init_dir_ondisk();

struct dir_description {
	struct dir_ondisk* ondisk;
	struct fs_description* fs;
};

struct dir_description* create_dir(struct fs_description* fs, int parent_inode_id, char* dirname);
struct dir_description* init_dir(struct fs_description* fs);
void add_dirent_ondisk(struct dir_description* dir, struct dirent_ondisk* d);
struct dir_description* dir_from_inode(struct fs_description* fs, int inode_id);
int find_inode_by_path(struct fs_description* fs, char* path);
#endif
