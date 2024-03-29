#include "fs.h"

#include "stdlib.h"
#include "errno.h"
#include "assert.h"

#include "block.h"
#include "dir.h"
#include "dirent.h"
#include "ondisk.h"
#include "utility.h"
#include "inode.h"

struct superblock_description* fs_superblock_init(struct fs_description* fs) {
	struct superblock_description* sb;

	sb = (struct superblock_description*)malloc(sizeof(struct superblock_description));
	if (sb <= 0) {
		return sb;
	}

	sb->ondisk = init_superblock_ondisk();
	sb->fs = fs;

	return sb;
}

int fs_superblock_write(struct superblock_description* sb) {
	int err;
	int written;

	printf("fs_superblock_write STUB\n");

	err = fseek(sb->fs->device_file, 0, SEEK_SET);
	if (err != 0) {
		perror("fs_superblock_write: fseek() failed");
		return err;
	}

	written = fwrite((void*)sb->ondisk, sizeof(struct superblock_ondisk), 1, sb->fs->device_file);
	assert(written);
	fflush(sb->fs->device_file);

	return 0;
}

struct fs_description* init_fs(char* device_path) {
	struct fs_description* fs;

	fs = (struct fs_description*)malloc(sizeof(struct fs_description));
	if (fs <= 0) {
		printf("Failed to allocate fs_description structure\n");
		exit(1);
	}

	fs->device_file = fopen(device_path, "r+");
	if (fs->device_file == 0) {
		printf("Failed to open device file %s.\n", device_path);
		exit(1);
	}

	fs->superblock = fs_superblock_init(fs);
	if (fs->superblock <= 0) {
		printf("Failed to initialize superblock_description.\n");
		exit(1);
	}

	return fs;
}

int read_fs(struct fs_description* fs, char* device_path) {
	int err;
	struct superblock_ondisk* ondisk;

	ondisk = fs->superblock->ondisk;
	err = read_superblock_ondisk(fs->device_file, ondisk);
	if (err != 0) {
		return err;
	}

	init_bitmap(fs);
	read_bitmap(fs);

	return 0;
}

int block_by_node_id(struct fs_description* fs, int inode_id) {
	struct superblock_ondisk* ondisk;

	ondisk = fs->superblock->ondisk;
	
	return ondisk->inodes_offset / ondisk->blocksize + inode_id;
}

int inode_id_by_block(struct fs_description* fs, int block) {
	struct superblock_ondisk* ondisk;

	ondisk = fs->superblock->ondisk;
	
	return block - ondisk->inodes_offset / ondisk->blocksize;
}

int create_fs(char* device_path) {
	struct fs_description* fs;
	struct dir_description* dir;
	int device_size;
	int device_blocks;

	fs = init_fs(device_path);

	device_size = get_file_size(device_path);
	device_blocks = device_size / HARDCODED_BLOCKSIZE;

	struct superblock_description* sb = fs->superblock;

	sb->ondisk->magic     = SUPERBLOCK_MAGIC;
	sb->ondisk->size      = device_size;
	sb->ondisk->blocksize = HARDCODED_BLOCKSIZE;

	sb->ondisk->bitmap_offset = HARDCODED_BLOCKSIZE;

	sb->ondisk->inodes_offset = 64 * HARDCODED_BLOCKSIZE;
	sb->ondisk->inodes_blocknum = HARDCODED_INODES_BLOCKNUM;

	sb->ondisk->files_and_directories_offset = HARDCODED_BLOCKSIZE * (64 /* <- superblock + large bitmap */ + HARDCODED_INODES_BLOCKNUM /* <- inodes */);
	sb->ondisk->files_and_directories_blocknum = device_blocks - (1 + HARDCODED_INODES_BLOCKNUM);

	int err = fs_superblock_write(sb);
	if (err != 0) {
		return err;
	}

	create_bitmap(fs);
	(void)create_dir(fs, 0, "");

	return 0;
}

int offset_for_inode(struct fs_description* fs, int inode_id) {
	struct superblock_ondisk* ondisk;

	ondisk = fs->superblock->ondisk;
	return ondisk->inodes_offset + ondisk->blocksize * inode_id;
}
