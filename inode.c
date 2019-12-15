#include "inode.h"

#include "assert.h"
#include "stdlib.h"

#include "block.h"
#include "fs.h"
#include "utility.h"

struct inode_ondisk* init_inode_ondisk(struct fs_description* fs) {
	struct inode_ondisk* ondisk;

	ondisk = (struct inode_ondisk*)malloc(sizeof(struct inode_ondisk));
	if (ondisk <= 0) {
		printf("Failed to allocate inode_ondisk");
		exit(1);
	}

	return ondisk;
}

struct inode* init_inode(struct fs_description* fs, int id) {
	struct inode* i;

	i = (struct inode*)malloc(sizeof(struct inode));
	if (i <= 0) {
		printf("Failed to allocate inode");
		exit(1);
	}

	i->ondisk = init_inode_ondisk(fs);
	i->id = id;
	i->fs = fs;

	return i;
}

int read_inode(struct inode* i) {
	FILE* f;
	int err;
	int have_read;
	int offset;

	f = i->fs->device_file;

	offset = offset_for_inode(i->fs, i->id);
	err = fseek(f, offset, SEEK_SET);
	if (err != 0) {
		return err;
	}

    while((have_read = fread(i->ondisk, sizeof(struct inode_ondisk), 1, f)) != 1) {
		/* pass */
	} 

	return 0;
}

char is_directory_inode(struct fs_description* fs, int inode_id) {
	struct inode* i;

	i = init_inode(fs, inode_id);
	read_inode(i);
	
	return (i->ondisk->permissions & (1 << 9)) > 0;
}

int load_inode_content(struct inode* i, void* where, int size) {
	int blocksize;
	struct inode_ondisk* ondisk;
	FILE* f;
	int id;
	int block;
	int num_loaded;
	int need_in_block;
	int err;
	int offset;
	int have_read;

	f = i->fs->device_file;
	ondisk = i->ondisk;
	blocksize = i->fs->superblock->ondisk->blocksize;

	num_loaded = 0;
	for (id = 0; num_loaded < size; ++id) {
		block = i->ondisk->blocks[id];
		assert(block != 0);    // expecting size not to exceed what blocks can hold

		offset = block * blocksize;
		err = fseek(f, offset, SEEK_SET);
		if (err != 0) {
			return err;
		}

		need_in_block = MIN(blocksize, size - num_loaded);
		while((have_read = fread(where, need_in_block, 1, f)) != 1) {
			/* pass */
		} 
		where += need_in_block;
		num_loaded += need_in_block;
	}

	return 0;
}

int save_inode_content(struct inode* i, void* from, int size) {
	int blocksize;
	struct inode_ondisk* ondisk;
	FILE* f;
	int id;
	int block;
	int num_saved;
	int need_in_block;
	int err;
	int offset;
	int have_read;

	f = i->fs->device_file;
	ondisk = i->ondisk;
	blocksize = i->fs->superblock->ondisk->blocksize;

	offset = offset_for_inode(i->fs, i->id);
	err = fseek(f, offset, SEEK_SET);
	if (err != 0) {
		return err;
	}

	num_saved = 0;
	for (id = 0; num_saved < size; ++id) {
		block = i->ondisk->blocks[id];
		if (!block) {
			block = allocate_inode_block(i->fs);
			i->ondisk->blocks[id] = block;
		}

		offset = block * blocksize;
		err = fseek(f, offset, SEEK_SET);
		if (err != 0) {
			return err;
		}

		need_in_block = MIN(blocksize, size - num_saved);
		while((have_read = fwrite(from, need_in_block, 1, f)) != 1) {
			/* pass */
		} 
		from += need_in_block;
		num_saved += need_in_block;
	}

	return 0;
}
