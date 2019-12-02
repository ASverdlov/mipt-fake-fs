#ifndef CLI_H
#define CLI_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fs.h"

void print_help_and_exit(int status) {
    printf("Help\n");
	exit(status);
}

int process_mkfs_command(int argc, char** argv) {
	// $ fakefs mkfs <device-file>

	if (argc != 3) {
		printf("$ fakefs mkfs <device-file>\n\n");
		printf("ERROR: 3 arguments required, but got %d arguments.\n", argc);
		return EXIT_FAILURE;
	}

	char* device_path = argv[2];

	return create_fs(device_path);
}

int process_info_command(int argc, char** argv) {
	// $ fakefs info <device-file>
	int err;

	if (argc != 3) {
		printf("$ fakefs info <device-file>\n\n");
		printf("ERROR: 3 arguments required, but got %d arguments.\n", argc);
		return EXIT_FAILURE;
	}

	char* device_path = argv[2];

	struct fs_description* fs = fs_init();
	if (fs <= 0) {
		return (int)fs;
	}

	err = fs_read(fs, device_path);
	if (err != 0) {
		return err;
	}

	printf("Superblock info:\n");
	printf(" - magic: %d\n",     fs->superblock->magic);
	printf(" - size: %d\n",      fs->superblock->size);
	printf(" - blocksize: %d\n", fs->superblock->blocksize);
	printf("\n");
	printf(" - inodes_offset: %d\n", fs->superblock->inodes_offset);
	printf("TODO\n");

	return 0;
}

int cli_dispatch_command(int argc, char** argv) {
	if (argc <= 1) {
		print_help_and_exit(EXIT_SUCCESS);
	}

	if (!strcmp(argv[1], "mkfs")) {
		return process_mkfs_command(argc, argv);
	} else if (!strcmp(argv[1], "info")) {
		return process_info_command(argc, argv);
	} else if (!strcmp(argv[1], "help")) {
		print_help_and_exit(EXIT_SUCCESS);
	}

	printf("Unknown command.\n");
	print_help_and_exit(EXIT_FAILURE);

	return EXIT_FAILURE;
}

#endif
