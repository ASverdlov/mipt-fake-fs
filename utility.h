#ifndef UTILITY_H
#define UTILITY_H

#define MIN(a,b) \
	({ __typeof__ (a) _a = (a); \
	   __typeof__ (b) _b = (b); \
	 _a < _b ? _a : _b; })


int get_file_size(char* path);

char* safe_basename(char* path);
char* safe_dirname(char* path);

#endif
