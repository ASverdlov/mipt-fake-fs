#include "block.h"
#include "fs.h"
#include "inode.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

void init_bitmap(struct fs_description* fs) {
	struct bitmap_description* bitmap;
	struct superblock_ondisk* ondisk;
	int num_blocks;

	ondisk = fs->superblock->ondisk;
	num_blocks = ondisk->size / ondisk->blocksize;

	bitmap = (struct bitmap_description*)malloc(sizeof(struct bitmap_description));
	if (bitmap <= 0) {
		printf("Failed to allocate bitmap_description");
		exit(1);
	}

	bitmap->bitmap = (char*)malloc(num_blocks * sizeof(char));
	if (bitmap <= 0) {
		printf("Failed to allocate bitmap_description");
		exit(1);
	}

	fs->bitmap = bitmap;
}

void read_bitmap(struct fs_description* fs) {
	struct bitmap_description* bitmap;
	struct superblock_ondisk* ondisk;
	int err;
	int have_read;
	FILE* f;
	int num_blocks;

	f = fs->device_file;
	bitmap = fs->bitmap;
	ondisk = fs->superblock->ondisk;
	num_blocks = ondisk->size / ondisk->blocksize;

	err = fseek(f, ondisk->bitmap_offset, SEEK_SET);
	if (err != 0) {
		printf("Failed to fseek while reading bitmap_description");
		exit(1);
	}
    while((have_read = fread(bitmap->bitmap, num_blocks * sizeof(char), 1, f)) != 1) {
		/* pass */
	} 
}

int save_bitmap(struct fs_description* fs) {
	int err;
	int written;
	FILE* f;
	struct superblock_ondisk* ondisk;
	int num_blocks;
	char* bitmap;
	
	bitmap = fs->bitmap->bitmap;
	ondisk = fs->superblock->ondisk;
	num_blocks = ondisk->size / ondisk->blocksize;
	f = fs->device_file;

	err = fseek(f, ondisk->bitmap_offset, SEEK_SET);
	if (err != 0) {
		return err;
	}

    while((written = fwrite(bitmap, num_blocks * sizeof(char), 1, f)) != 1) {
		/* pass */
	} 

	return 0;
}

void create_bitmap(struct fs_description* fs) {
	char* bitmap;
	struct superblock_ondisk* ondisk;
	int num_blocks;

	ondisk = fs->superblock->ondisk;
	num_blocks = ondisk->size / ondisk->blocksize;

	init_bitmap(fs);
	bitmap = fs->bitmap->bitmap;

	// fill with zeros
	memset(bitmap, 0, num_blocks * sizeof(char));
	*(bitmap + 0) = (char)1; // superblock

	save_bitmap(fs);
}

int allocate_internal(struct fs_description* fs, int start, int end) {
	char* bitmap;
	int block;
	char found_unused;
	int err;

	bitmap = fs->bitmap->bitmap;

	found_unused = 0;
	for (block = start; block < end; ++block) {
		if (*(bitmap + block) == (char)0) {
			found_unused = 1;
			break;
		}
	}
	if (!found_unused) {
		fprintf(stderr, "failed to find free block to allocate in fs");
		exit(1);
	}

	*(bitmap + block) = (char)1;

	err = save_bitmap(fs);
	if (err != 0) {
		fprintf(stderr, "failed to save bitmap");
		exit(1);
	}

	return block;
}

int allocate_inode(struct fs_description* fs) {
	struct superblock_ondisk* ondisk;
	int start;
	int cnt;

	ondisk = fs->superblock->ondisk;
	start = ondisk->inodes_offset / ondisk->blocksize;
	cnt = ondisk->inodes_blocknum;

	return allocate_internal(fs, start, start + cnt);
}

int allocate_inode_block(struct fs_description* fs) {
	struct superblock_ondisk* ondisk;
	int start;
	int cnt;

	ondisk = fs->superblock->ondisk;
	start = ondisk->files_and_directories_offset / ondisk->blocksize;
	cnt = ondisk->files_and_directories_blocknum;

	int res = allocate_internal(fs, start, start + cnt);
	return res;
}
