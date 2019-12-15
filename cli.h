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
	} else if (!strcmp(argv[1], "help")) {
		print_help_and_exit(EXIT_SUCCESS);
	}

	printf("Unknown command.\n");
	print_help_and_exit(EXIT_FAILURE);

	return EXIT_FAILURE;
}

#endif
