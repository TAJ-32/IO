#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
#include "myio.h" 

ssize_t myread(struct FILER *FV, void *buf, size_t count) { //count is how many bytes we are reading

	/*need to create a struct that has as properties:
	 *	- fd
	 *	- buf2
	 *	- the size specified by the first myread call
	 *we will pass that struct through on every call of myread so that it knows where to start adding to the buffer from the file
	 */

	int bytes_so_far = FV -> bytes_read;

	printf("Count: %ld, Bytes: %d\n", count, bytes_so_far);


	if (FV->not_read_yet) {
		read(FV->fd, FV->hidden_buf, 100);
		printf("FIRST READ SYSCALL\n");
		memcpy(buf, FV->hidden_buf, count); 
		FV -> bytes_read += count;
		FV -> bytes_read_tot += count;
		FV -> not_read_yet = false;
		printf("memcpy: %s\n", (char *) buf);

	}
	else if (((FV -> bytes_read) + count) >= (FV -> buf_size)) {

	//	FV->buf_size += 100;

	//	printf("Current buffer: %s\n", FV->hidden_buf);

	//	printf("Current pointer: %d\n", *FV->hidden_buf);

	//	FV->hidden_buf = realloc(FV->hidden_buf, FV->buf_size);
	//
	//	printf("Realloced pointer: %d\n", *FV->hidden_buf);



		//maybe change that you don't add bytes_read or bytes_read_tot in the actual read syscall
		int leftovers = (FV -> buf_size) - (FV->bytes_read);

	//	printf("LEFTOVERS: %d\n", leftovers);

	//	printf("Point in hidden_buf: %s\n", FV->hidden_buf + FV->bytes_read);

	//	printf("Bytes read: %d\n", FV->bytes_read);
		memcpy(buf, FV->hidden_buf + FV->bytes_read, leftovers); //copy the few bits that will get lost in between the myread and the syscall to buf
		//printf("Writing out the leftover buffer:\n");
		//write(1, buf, 2);
		//
		FV->bytes_read += leftovers;//read two more bytes
					    //
		if (FV->bytes_read >= FV->buf_size) {
			FV->bytes_read = 0;
		}
		

		read(FV->fd, FV->hidden_buf, 100);

		int carryover = count - leftovers;

		printf("READ SYSCALL\n");
		memcpy(((char *) buf) + leftovers, FV->hidden_buf, carryover);
		printf("memcpy: %s\n", (char *) buf);

		FV -> bytes_read += carryover;
		FV -> bytes_read_tot += count;

	}
	else {
		memcpy(buf, FV->hidden_buf + FV->bytes_read, count); //copy however many bytes the user specifies from the hidden buf into the user buf. We do count + 1 to account for
									 //null char
		FV -> bytes_read += count;
		FV -> bytes_read_tot += count;

		printf("memcpy: %s\n", (char *) buf);
	}

	return ((FV -> bytes_read)); //return the number of bytes read on this specific call



	/*
	if (FV->not_read_yet || ((FV -> bytes_read) + count) >= (FV -> buf_size)) { //if the buf2 is empty or the user buf is full, do the syscall
		
		//FV->buf_size += 100;

		//FV->hidden_buf = realloc(FV->hidden_buf, FV->buf_size);

		read(FV->fd, FV->hidden_buf + FV->bytes_read, 100); //when we actually have to do a system call, load 4096 bytes of data into the hidden buffer.
							 //On subsequent calls, that will be memcpyed into the user buf that is passed to this function.
		printf("READ SYSTEM CALL\n");
		FV->not_read_yet = false;
		memcpy(buf, FV->hidden_buf + FV->bytes_read, count); 
		FV -> bytes_read += count;
		printf("memcpy: %s\n", (char *) buf);
	}
	else {
		memcpy(buf, FV->hidden_buf + FV->bytes_read, count); //copy however many bytes the user specifies from the hidden buf into the user buf. We do count + 1 to account for
									 //null char
		FV -> bytes_read += count;
		printf("memcpy: %s\n", (char *) buf);
	}

	return ((FV -> bytes_read) - (bytes_so_far)); //return the number of bytes read on this specific call
	*/
	
}

ssize_t mywrite(struct FILER *FV, void *buf, size_t count) { //count is how many bytes we are writing
							     //
	int bytes_so_far = FV -> bytes_writ;

	if (FV->not_writ_yet || (FV -> bytes_writ) >= (FV -> buf_size)) {
		write(FV->fd, FV->hidden_buf + FV->bytes_writ, FV->buf_size);

		printf("WRITE SYSCALL\n");
		FV->not_writ_yet = false;
		memcpy(buf, FV->hidden_buf + FV->bytes_writ, count + 1); //the plus one for the endline character
		printf("memcpy: %s\n", (char *) buf);
	}
	else {
		memcpy(buf, FV->hidden_buf + FV->bytes_writ, count + 1);

		FV -> bytes_writ += count + 1;
		printf("memcpy: %s\n", (char *) buf);
	}

	return ((FV -> bytes_writ) - (bytes_so_far));

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
	FV -> bytes_read_tot = 0;
	FV -> bytes_writ = 0;
	FV -> not_read_yet = true;
	FV -> not_writ_yet = true;


	return FV;
}

