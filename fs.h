#ifndef FS_H
#define FS_H

#include "stdlib.h"
#include "sys/stat.h"
#include "fcntl.h"

static const int SUPERBLOCK_MAGIC = 8031996;
static const int HARDCODED_BLOCKSIZE = 4096;
static const int HARDCODED_INODES_BLOCKNUM = 1024;

struct superblock_description {
	struct fs_description* fs;

	int magic;
	int size;
	int blocksize;

	int inodes_offset;
	int inodes_blocknum;

	int files_and_directories_offset;
	int files_and_directories_blocknum;
};

struct fs_description {
	FILE*                          device_file;
	struct superblock_description* superblock;
};

struct superblock_description* fs_superblock_init(struct fs_description* fs) {
	struct superblock_description* sb;

	sb = (struct superblock_description*)malloc(sizeof(struct superblock_description));
	if (sb <= 0) {
		return sb;
	}

	sb->fs = fs;
	return sb;
}

int fs_superblock_read(struct superblock_description* sb) {
	int err;

	printf("fs_superblock_read STUB\n");

	err = fseek(sb->fs->device_file, 0, SEEK_SET);
	if (err != 0) {
		return err;
	}


	int cnt;
    while(cnt = fread(sb, sizeof(struct superblock_description), 1, sb->fs->device_file)) {
		/* pass */
		printf("fs_superblock_read INSIDE WHILE(FREAD)!!! COUNT READ: %d, SIZEOF(SB): %lu\n", cnt, sizeof(struct superblock_description));
	} 

	printf("fs_superblock_read AFTER READ!!!\n");

	return 0;
}

int fs_superblock_write(struct superblock_description* sb) {
	int err;

	printf("fs_superblock_write STUB\n");

	err = fseek(sb->fs->device_file, 0, SEEK_SET);
	if (err != 0) {
		perror("fs_superblock_write: fseek() failed");
		return err;
	}

	err = fwrite(sb, sizeof(struct superblock_description), 1, sb->fs->device_file);
	if (err != 0) {
		perror("fs_superblock_write: fwrite() failed");
		return err;
	}

	return 0;
}

int _get_file_size(char* path) {
	struct stat st;
	stat(path, &st);
	return st.st_size;
}

struct fs_description* fs_init() {
	struct fs_description* fs;

	fs = (struct fs_description*)malloc(sizeof(struct fs_description));
	if (fs <= 0) {
		return fs;
	}

	fs->superblock = fs_superblock_init(fs);
	if (fs->superblock <= 0) {
		/* I know this looks strange, but it's just an error, deadline is tight
		and I'm not a true C programmer anyways */
		return fs->superblock;
	}

	return fs;
}

int fs_read(struct fs_description* fs, char* device_path) {
	int err;

	fs->device_file = fopen(device_path, "r+");
	if (fs->device_file == 0) {
		printf("Failed to open device file %s.\n", device_path);
		return EXIT_FAILURE;
	}

	err = fs_superblock_read(fs->superblock);
	if (err != 0) {
		return err;
	}

	return 0;
}

int create_fs(char* device_path) {
	struct fs_description* fs;
	int device_size;
	int device_blocks;

	fs = fs_init();
	if (fs <= 0) {
		return fs;
	}

	device_size = _get_file_size(device_path);
	device_blocks = device_size / HARDCODED_BLOCKSIZE;

	fs->device_file = fopen(device_path, "r+");
	if (fs->device_file == 0) {
		printf("Failed to open device file %s.\n", device_path);
		return EXIT_FAILURE;
	}

	struct superblock_description* sb = fs->superblock;

	sb->magic     = SUPERBLOCK_MAGIC;
	sb->size      = device_size;
	sb->blocksize = HARDCODED_BLOCKSIZE;

	sb->inodes_offset = HARDCODED_BLOCKSIZE;
	sb->inodes_blocknum = HARDCODED_INODES_BLOCKNUM;

	sb->files_and_directories_offset = HARDCODED_BLOCKSIZE * (1 /* <- superblock */ + HARDCODED_INODES_BLOCKNUM /* <- inodes */);
	sb->files_and_directories_blocknum = device_blocks - (1 + HARDCODED_INODES_BLOCKNUM);

	int err = fs_superblock_write(sb);
	if (err != 0) {
		return err;
	}

	printf("TODO: create a root directory!\n");

	return 0;
}

#endif
