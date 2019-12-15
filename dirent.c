#include "dirent.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "fs.h"
#include "inode.h"

struct dirent_ondisk* init_dirent_ondisk() {
	struct dirent_ondisk* ondisk;
	
	ondisk = (struct dirent_ondisk*)malloc(sizeof(struct dirent_ondisk));
	if (ondisk <= 0) {
		printf("Failed to allocate dirent_ondisk");
		exit(1);
	}

	return ondisk;
}

// TODO: delete
struct dirent* init_dirent(struct fs_description* fs, struct dirent* parent) {
	struct dirent* d;

	d = (struct dirent*)malloc(sizeof(struct dirent));
	if (d <= 0) {
		printf("Failed to allocate dirent");
		exit(1);
	}

	d->ondisk = init_dirent_ondisk();
	d->parent = parent;
	d->fs = fs;

	return d;
}
