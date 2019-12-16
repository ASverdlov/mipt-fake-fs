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

struct inode* create_file(struct fs_description* fs, int parent_inode_id, char* filename) {
	struct dir_description* parent_dir;
	struct inode* i;
	struct inode* parent_i;
	struct dirent_ondisk dirent_for_parent;
	int err;

	// 1. create inode
	i = create_inode(fs, 0666); // 666 - permissions for everything (except execution) to all users

	// 2. save dirent for newly created directory in parent inode (if it's not root)
	parent_dir = dir_from_inode(fs, parent_inode_id);

	if (strlen(filename) > MAX_FILENAME) {
		fprintf(stderr, "filename length exceeds %d", MAX_FILENAME);
		exit(1);
	}
	strcpy(dirent_for_parent.name, filename);
	
	dirent_for_parent.inode_id = i->id;
	add_dirent_ondisk(parent_dir, &dirent_for_parent);

	parent_i = init_inode(fs, parent_inode_id);
	read_inode(parent_i);


	err = save_inode_content(parent_i, parent_dir->ondisk, sizeof(struct dir_ondisk));
	if (err != 0) {
		fprintf(stderr, "Failed to save parent dir content for inode %d", parent_inode_id);
		exit(1);
	}

	return i;
}

struct dir_description* create_dir(struct fs_description* fs, int parent_inode_id, char* dirname) {
	struct dir_description* dir;
	struct dir_description* parent_dir;
	struct inode* i;
	struct inode* parent_i;
	struct dirent_ondisk self_dirent;
	struct dirent_ondisk parent_dirent;
	struct dirent_ondisk dirent_for_parent;
	int err;

	// 1. create inode
	i = create_inode(fs, 01666); // 1 - dir, 666 - permissions for everything (except execution) to all users

	// 2. create dir
	dir = init_dir(fs);
	dir->ondisk->num_entries = 0;

	// 3. add dirents
	strncpy(self_dirent.name, ".\0", 2);
	self_dirent.inode_id = i->id;
	add_dirent_ondisk(dir, &self_dirent);

	strncpy(parent_dirent.name, "..\0", 3);
	parent_dirent.inode_id = parent_inode_id;
	add_dirent_ondisk(dir, &parent_dirent);

	// 4. save dir
	err = save_inode_content(i, dir->ondisk, sizeof(struct dir_ondisk));
	if (err != 0) {
		fprintf(stderr, "Failed to save dir content for inode %d", i->id);
		exit(1);
	}

	// 5. save dirent for newly created directory in parent inode (if it's not root)
	if (i->id != 0) {
		parent_dir = dir_from_inode(fs, parent_inode_id);

		if (strlen(dirname) > MAX_FILENAME) {
			fprintf(stderr, "filename length exceeds %d", MAX_FILENAME);
			exit(1);
		}
		strcpy(dirent_for_parent.name, dirname);
		dirent_for_parent.inode_id = i->id;
		add_dirent_ondisk(parent_dir, &dirent_for_parent);

		parent_i = init_inode(fs, parent_inode_id);
		read_inode(parent_i);

		err = save_inode_content(parent_i, parent_dir->ondisk, sizeof(struct dir_ondisk));
		if (err != 0) {
			fprintf(stderr, "Failed to save parent dir content for inode %d", parent_inode_id);
			exit(1);
		}
	}

	return dir;
}

void add_dirent_ondisk(struct dir_description* dir, struct dirent_ondisk* d) {
	int entry_id;

	entry_id = dir->ondisk->num_entries++;
	memcpy(&dir->ondisk->dirents[entry_id], d, sizeof(struct dirent));
}

char dirent_exists(struct dir_description* dir, char* name) {
	int entry_id;
	struct dirent_ondisk* ent;

	for (entry_id = 0; entry_id < dir->ondisk->num_entries; ++entry_id) {
		ent = &dir->ondisk->dirents[entry_id];
		if (!strcmp(ent->name, name)) {
			return 1;
		}
	}

	return 0;
}

void delete_dirent_ondisk_by_name(struct dir_description* dir, char* name) {
	int entry_id;
	struct dirent_ondisk* ent;
	struct dirent_ondisk* next_ent;
	int found;

	found = -1;
	for (entry_id = 0; entry_id < dir->ondisk->num_entries; ++entry_id) {
		ent = &dir->ondisk->dirents[entry_id];
		if (!strcmp(ent->name, name)) {
			found = entry_id;
			break;
		}
	}
	assert(found != -1);

	for (entry_id = found; entry_id + 1 < dir->ondisk->num_entries; ++entry_id) {
		ent = &dir->ondisk->dirents[entry_id];
		next_ent = &dir->ondisk->dirents[entry_id + 1];

		strcpy(ent->name, next_ent->name);
		ent->inode_id = next_ent->inode_id;
	}

	--dir->ondisk->num_entries;
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

int find_inode_by_path(struct fs_description* fs, char* orig_path) {
	struct dirent_ondisk* ent;
	struct dir_description* dir;
	struct inode* i;
	int inode_id;
	char* part;
	int entry_id;
	char found_entry;
	int err;
	char* path;

	inode_id = 0; // root directory

	path = (char*)malloc(strlen(orig_path) + 1);
	strcpy(path, orig_path);

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
			fprintf(stderr, "entry '%s' not found\n", part);
			exit(1);
		}

	    part = strtok(NULL, "/");
	}

	return inode_id;
}

void recursively_delete_inode(struct fs_description* fs, int inode_id, int parent_inode_id, char* name) {
	assert(inode_id != 0);

	fprintf(stderr, "recursively_delete_inode %d, name '%s'\n", inode_id, name);

	struct dir_description* dir;
	struct dir_description* parent_dir;
	struct dirent_ondisk* ent;
	int entry_id;
	struct inode_ondisk* ondisk;
	struct inode* i;
	struct inode* parent_i;
	int	block;
	int err;

	i = init_inode(fs, inode_id);
	read_inode(i);

	if (is_directory_inode(fs, parent_inode_id)) {
		dir = dir_from_inode(fs, inode_id);
		for (entry_id = 0; entry_id < dir->ondisk->num_entries; ++entry_id) {
			ent = &dir->ondisk->dirents[entry_id];
			if (!strcmp(ent->name, ".") || !strcmp(ent->name, "..")) {
				continue;
			}

			recursively_delete_inode(fs, ent->inode_id, inode_id, ent->name);
		}
	} else {
		ondisk = i->ondisk;
		for (entry_id = 0; entry_id < MAX_BLOCKS_NUM; ++entry_id) {
			block = ondisk->blocks[entry_id];
			if (block != 0) {
				deallocate_block(fs, block);
			}
		}
	}

	deallocate_block(fs, block_by_node_id(fs, inode_id));

	parent_dir = dir_from_inode(fs, parent_inode_id);
	parent_i = init_inode(fs, parent_inode_id);
	read_inode(parent_i);

	fprintf(stderr, "delete entry from parent inode %d, entry with name %s\n", parent_i->id, name);
	delete_dirent_ondisk_by_name(parent_dir, name);

	err = save_inode_content(parent_i, parent_dir->ondisk, sizeof(struct dir_ondisk));
	if (err != 0) {
		fprintf(stderr, "failed to save deleted direntry for inode %d\n", parent_inode_id);
		exit(1);
	}
}

