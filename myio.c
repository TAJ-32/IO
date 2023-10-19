#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
#include "myio.h"

ssize_t myread(struct FILER *FV, void *buf, size_t count) {

	/*need to create a struct that has as properties:
	 *	- fd
	 *	- buf2
	 *	- the size specified by the first myread call
	 *we will pass that struct through on every call of myread so that it knows where to start adding to the buffer from the file
	 */

	if (FV->not_read_yet || (FV -> bytes_read) >= (FV -> buf_size)) { //if the buf2 is empty or the user buf is full, do the syscall
		read(FV->fd, FV->hidden_buf + FV->bytes_read, FV->buf_size); //when we actually have to do a system call, load 4096 bytes of data into the hidden buffer.
							 //On subsequent calls, that will be memcpyed into the user buf that is passed to this function.
		printf("SYSTEM CALL\n");
		FV->not_read_yet = false;
		memcpy(buf, FV->hidden_buf + FV->bytes_read, count + 1); 
		FV -> bytes_read += count + 1;
		printf("memcpy: %s\n", (char *) buf);
	}
	else {
		memcpy(buf, FV->hidden_buf + FV->bytes_read, count + 1); //copy however many bytes the user specifies from the hidden buf into the user buf. We do count + 1 to account for
									 //null char
		FV -> bytes_read += count + 1;
		printf("memcpy: %s\n", (char *) buf);
	}

	return 0;
	
}

struct FILER *myopen(const char *pathname, int flags) {
	/*
	 * Step 1: Create new open file description (not descriptor.
	 * 						Descriptor refers to the description)
	 * Step 2: Record the offset (current position in the file) and the status flags specified.
	 * Step 3: 
	 * */

	int fd = open(pathname, flags);

	printf("FILE DESCRIPTOR: %d\n", fd);

	struct FILER *FV = malloc(sizeof(struct FILER));
	FV -> fd = fd;
	FV -> buf_size = 100; //arbitrary amount of memory
	FV -> hidden_buf = malloc(FV -> buf_size); //arbitrary amount
	FV -> bytes_read = 0;
	FV -> not_read_yet = true;


	return FV;
}

