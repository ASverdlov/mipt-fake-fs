#ifndef CLI_H
#define CLI_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libgen.h"

#include "dir.h"
#include "fs.h"

void print_help_and_exit(int status);

int mkfs_command(int argc, char** argv);
int mkdir_command(int argc, char** argv);
int info_command(int argc, char** argv);

int cli_dispatch_command(int argc, char** argv);

#endif
