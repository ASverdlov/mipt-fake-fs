#include "cli.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libgen.h"

#include "dir.h"
#include "fs.h"
#include "ondisk.h"

void print_help_and_exit(int status) {
    printf("Help\n");
	exit(status);
}

int mkfs_command(int argc, char** argv) {
	// $ fakefs mkfs <device-file>

	if (argc != 3) {
		printf("$ fakefs mkfs <device-file>\n\n");
		printf("ERROR: 3 arguments required, but got %d arguments.\n", argc);
		return EXIT_FAILURE;
	}

	char* device_path = argv[2];

	return create_fs(device_path);
}

char* format_permissions(int permissions) {
	char* s = malloc(11 * sizeof(char));

	s[0] = ((permissions & (1 << 9)) > 0 ? 'd' : '-');
	s[1] = ((permissions & (1 << 8)) > 0 ? 'r' : '-');
	s[2] = ((permissions & (1 << 7)) > 0 ? 'w' : '-');
	s[3] = ((permissions & (1 << 6)) > 0 ? 'x' : '-');
	s[4] = ((permissions & (1 << 5)) > 0 ? 'r' : '-');
	s[5] = ((permissions & (1 << 4)) > 0 ? 'w' : '-');
	s[6] = ((permissions & (1 << 3)) > 0 ? 'x' : '-');
	s[7] = ((permissions & (1 << 2)) > 0 ? 'r' : '-');
	s[8] = ((permissions & (1 << 1)) > 0 ? 'w' : '-');
	s[9] = ((permissions & (1 << 0)) > 0 ? 'x' : '-');
	s[10] = '\0';

	return s;
}

int ls_command(int argc, char** argv) {
	// $ fakefs ls <device-file> <path>
	int err;
	char* device_path;
	char* path;
	struct fs_description* fs;
	int inode_id;
	struct dir_description* dir;
	int dirent_id;
	struct dirent_ondisk* ent;
	struct inode* i;

	if (argc != 4) {
		printf("$ fakefs ls <device-file> <path>\n\n");
		printf("ERROR: 4 arguments required, but got %d arguments.\n", argc);
		return EXIT_FAILURE;
	}

	device_path = argv[2];
	path = argv[3];

	fs = init_fs(device_path);
	if (fs <= 0) {
		return (int)fs;
	}

	err = read_fs(fs, device_path);
	if (err != 0) {
		return err;
	}

	inode_id = find_inode_by_path(fs, path);

	dir = dir_from_inode(fs, inode_id);

	for (dirent_id = 0; dirent_id < dir->ondisk->num_entries; ++dirent_id) {
		ent = &dir->ondisk->dirents[dirent_id];

		i = init_inode(fs, ent->inode_id);
		read_inode(i);

		printf("%d\t%s\t%s\n", i->id, format_permissions(i->ondisk->permissions), ent->name);
	}

	return 0;
}

int write_command(int argc, char** argv) {
	// $ fakefs write <device-file> <path>
	int err;
	char* device_path;
	char* path;
	char* parent_path;
	char* name;
	struct fs_description* fs;
	int inode_id;
	struct inode* i;

	if (argc != 4) {
		printf("$ fakefs mkdir <device-file> <path>\n\n");
		printf("ERROR: 4 arguments required, but got %d arguments.\n", argc);
		return EXIT_FAILURE;
	}

	device_path = argv[2];
	path = argv[3];

	fs = init_fs(device_path);
	if (fs <= 0) {
		return (int)fs;
	}

	err = read_fs(fs, device_path);
	if (err != 0) {
		return err;
	}

	parent_path = dirname(path);
	name = basename(path);

	// TODO: delete
	fprintf(stderr, "parent_path: '%s'", parent_path);
	fprintf(stderr, "filename: '%s'", name);

	inode_id = find_inode_by_path(fs, parent_path);

	i = create_file(fs, inode_id, name);

	// TODO: write to file from stdin

	return 0;
}

int mkdir_command(int argc, char** argv) {
	// $ fakefs mkdir <device-file> <path>
	int err;
	char* device_path;
	char* path;
	char* parent_path;
	char* name;
	struct fs_description* fs;
	int inode_id;
	struct dir_description* dir;

	if (argc != 4) {
		printf("$ fakefs mkdir <device-file> <path>\n\n");
		printf("ERROR: 4 arguments required, but got %d arguments.\n", argc);
		return EXIT_FAILURE;
	}

	device_path = argv[2];
	path = argv[3];

	fs = init_fs(device_path);
	if (fs <= 0) {
		return (int)fs;
	}

	err = read_fs(fs, device_path);
	if (err != 0) {
		return err;
	}

	parent_path = dirname(path);
	name = basename(path);
	
	inode_id = find_inode_by_path(fs, parent_path);

	//dir = dir_from_inode(fs, inode_id);

	(void)create_dir(fs, inode_id, name);

	return 0;
}

int info_command(int argc, char** argv) {
	// $ fakefs info <device-file>
	int err;

	if (argc != 3) {
		printf("$ fakefs info <device-file>\n\n");
		printf("ERROR: 3 arguments required, but got %d arguments.\n", argc);
		return EXIT_FAILURE;
	}

	char* device_path = argv[2];

	struct fs_description* fs = init_fs(device_path);
	if (fs <= 0) {
		return (int)fs;
	}

	err = read_fs(fs, device_path);
	if (err != 0) {
		return err;
	}

	print_superblock_ondisk(fs->superblock->ondisk);
	return 0;
}

int cli_dispatch_command(int argc, char** argv) {
	if (argc <= 1) {
		print_help_and_exit(EXIT_SUCCESS);
	}

	if (!strcmp(argv[1], "mkfs")) {
		return mkfs_command(argc, argv);
	} else if (!strcmp(argv[1], "info")) {
		return info_command(argc, argv);
	} else if (!strcmp(argv[1], "mkdir")) {
		return mkdir_command(argc, argv);
	} else if (!strcmp(argv[1], "ls")) {
		return ls_command(argc, argv);
	} else if (!strcmp(argv[1], "write")) {
		return write_command(argc, argv);
	} else if (!strcmp(argv[1], "help")) {
		print_help_and_exit(EXIT_SUCCESS);
	}

	printf("Unknown command.\n");
	print_help_and_exit(EXIT_FAILURE);

	return EXIT_FAILURE;
}
