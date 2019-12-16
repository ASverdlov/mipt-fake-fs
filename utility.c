#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"
#include "libgen.h"
#include "stdlib.h"
#include "string.h"

int get_file_size(char* path) {
	struct stat st;
	stat(path, &st);
	return st.st_size;
}

char* safe_basename(char* path) {
	char* bname = basename(path);
	char* tmp = (char*)malloc(strlen(bname) + 1);
	strcpy(tmp, bname);
	return tmp;
}

char* safe_dirname(char* path) {
	char* dname = dirname(path);
	char* tmp = (char*)malloc(strlen(dname) + 1);
	strcpy(tmp, dname);
	return tmp;
}
