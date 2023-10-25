#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>

#ifndef __MY_IO_H
#define __MY_IO_H

struct FILER {
	int fd;
	char *hidden_buf;
	int buf_size;
	int bytes_read;
	int bytes_read_tot;
	bool not_read_yet;
	bool not_writ_yet;
	int bytes_writ;
};

struct FILER *myopen(const char *pathname, int flags);

ssize_t myread(struct FILER *FV, void *buf, size_t count);

ssize_t mywrite(struct FILER *FV, void *buf, size_t count);
#endif


