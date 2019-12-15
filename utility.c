#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int get_file_size(char* path) {
	struct stat st;
	stat(path, &st);
	return st.st_size;
}

