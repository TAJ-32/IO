#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>

#ifndef __MY_IO_H
#define __MY_IO_H

//#define O_CREAT  1000000
//#define O_WRONLY 1
//#define O_RDONLY 10
//#define O_RDWR   100

struct FILER {
	int fd;
	char *hidden_buf;
	int buf_size;
	int bytes_read;
	int bytes_read_tot;
	int bytes_read_last;
	bool not_read_yet;
	bool not_writ_yet;
	int bytes_writ;
	int bytes_writ_tot;
	int flag_val;
	char flag_str[7]; //will need it as a string so I can loop through it and see which digits are 1. It will never be a string longer than 7
	int size;
	int user_offset;
};

struct FILER *myopen(const char *pathname, int flags);

ssize_t myread(struct FILER *FV, void *buf, size_t count);

ssize_t mywrite(struct FILER *FV, void *buf, size_t count);

off_t myseek(struct FILER *FV, off_t offset, int whence);

ssize_t myflush(struct FILER *FV);
#endif


