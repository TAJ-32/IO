#ifndef __MYIO_H
#define __MYIO_H

#define BUF_SIZE 100

#include <stdlib.h>
#include <stdbool.h>

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
	int flags;
	int size;
	int user_offset;
	int offset;
	int buf_offset;
	bool close_flush;
};

struct FILER *myopen(const char *pathname, int flags, mode_t mode);

int myclose(struct FILER *FV);

ssize_t myread(struct FILER *FV, void *buf, size_t count);

ssize_t mywrite(struct FILER *FV, void *buf, size_t count);

off_t myseek(struct FILER *FV, off_t offset, int whence);

ssize_t myflush(struct FILER *FV);
#endif


