#include "dir.h"

#include "stdlib.h"

#include "dirent.h"
#include "fs.h"

struct dir_ondisk* init_dir_ondisk() {
	struct dir_ondisk* ondisk;

	ondisk = (struct dir_ondisk*)malloc(sizeof(struct dir_ondisk));
	if (ondisk <= 0) {
		printf("Failed to allocate dir_ondisk");
		exit(1);
	}

	return ondisk;
}

struct dir_description* init_dir(struct fs_description* fs) {
	struct dir_description* dir;

	dir = (struct dir_description*)malloc(sizeof(struct dir_description));
	if (dir <= 0) {
		printf("Failed to allocate dir_description");
		exit(1);
	}

	dir->ondisk = init_dir_ondisk();
	dir->fs = fs;

	return dir;
}

struct dir_description* dir_from_inode(struct fs_description* fs, int inode_id) {
	int err;
	struct dir_description* dir;
	struct inode* i;
	int offset;

	i = init_inode(fs, inode_id);
	err = read_inode(i);
	if (err != 0) {
		fprintf(stderr, "Failed to read inode %d", inode_id);
		exit(1);
	}
	
	dir = init_dir(fs);
	err = load_inode_content(i, dir->ondisk, sizeof(struct dir_ondisk));
	if (err != 0) {
		fprintf(stderr, "Failed to load inode %d content", inode_id);
		exit(1);
	}

	return dir;
}

int find_inode_by_path(struct fs_description* fs, char* path) {
	struct dirent_ondisk* ent;
	struct dir_description* dir;
	struct inode* i;
	int inode_id;
	char* part;
	int entry_id;
	char found_entry;
	int err;

	inode_id = 0; // root directory

	part = strtok(path, "/");
	while (part != NULL) {
		if (!is_directory_inode(fs, inode_id)) {
			printf("%s is not a directory", part);
			exit(1);
		}

		dir = dir_from_inode(fs, inode_id);

		found_entry = 0;
		for (entry_id = 0; entry_id < dir->ondisk->num_entries; ++entry_id) {
			ent = &dir->ondisk->dirents[entry_id];
			if (!strcmp(ent->name, part)) {
				inode_id = ent->inode_id;
				found_entry = 1;
				break;
			}
		}

		if (!found_entry) {
			printf("Entry not found");
			exit(1);
		}

	    printf("\n%s", part);
	    part = strtok(NULL, "/");
	}

	return inode_id;
}
