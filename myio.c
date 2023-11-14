#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
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
	
	if (FV -> flag_val == 1 || FV -> flag_val == 65) { //if a WRONLY is specified
		perror("No read access");
		exit(-1);
	}

	if ((FV -> bytes_read_tot) == (FV -> size)) {
		return 0;
	}

	if ((FV -> bytes_read_tot) + count >= (FV -> size)) {
		count = (FV -> size) - (FV -> bytes_read_tot);
	}


	if (count >= FV -> buf_size) {
		int rest_of_hidden = FV -> buf_size - FV -> bytes_read;
		memcpy(buf, FV->hidden_buf + FV -> bytes_read, rest_of_hidden); //copy the rest of h_buf into buf
		if (FV -> bytes_read == 0) {
			rest_of_hidden = 0;
		}
		read(FV -> fd, (char *) buf + rest_of_hidden, count-rest_of_hidden);
		FV -> bytes_read_tot += count;
		FV -> user_offset += count;
		memset(FV->hidden_buf, '\0', FV -> buf_size); //we are no longer trying to deal with the hidden buf because we just system call
							      //read into the user buffer so we can clear all the progress we made in h_buf
		FV -> bytes_read = 0;

		return count;
	}


	if (FV->not_read_yet) {
		read(FV->fd, FV->hidden_buf, FV->buf_size);
		memcpy(buf, FV->hidden_buf, count); 
		FV -> bytes_read += count;
		FV -> bytes_read_tot += count;
		FV -> user_offset += count;
		FV -> not_read_yet = false;
	}
	else if (((FV -> bytes_read) + count) >= (FV -> buf_size)) {

		int leftovers = (FV -> buf_size) - (FV->bytes_read);

		memcpy((char *) buf, FV->hidden_buf + FV -> bytes_read, leftovers); //copy the few bits that will get lost in between the myread and the syscall to buf

		FV->bytes_read += leftovers;//read two more bytes
		
		memset(FV->hidden_buf, '\0', FV -> buf_size);
		read(FV->fd, FV->hidden_buf, FV -> buf_size);
		
		int carryover = count - leftovers;

		memcpy((char *) buf + leftovers, FV->hidden_buf, carryover);

		FV -> bytes_read = carryover;
		FV -> bytes_read_tot += count;
		FV -> user_offset += count;
	}
	else {
		memcpy((char *) buf, FV->hidden_buf + FV->bytes_read, count); //copy however many bytes the user specifies from the hidden buf into the user buf. We do count + 1 to account for null char
								     
		FV -> bytes_read += count;
		FV -> bytes_read_tot += count;
		FV -> user_offset += count;
	}


//use my bytes_read_tot as a file offset to see if I am about to get to end of file.

	return (count); //return the number of bytes read on this specific call
}

ssize_t mywrite(struct FILER *FV, void *buf, size_t count) { //count is how many bytes we are writing
	if (FV -> flag_val == 0 || FV -> flag_val == 64) { //if a RONLY is specified
		perror("No write access");
		exit(-1);
	}

	if (count >= FV -> buf_size) {
		int flushed = FV -> bytes_writ;
		myflush(FV);
		write(FV -> fd, (char *) buf, count);
		FV -> bytes_writ_tot += count - flushed;
		FV -> user_offset += count - flushed;
		memset(FV->hidden_buf, '\0', FV -> buf_size); //we are no longer trying to deal with the hidden buf because we just system call
							      //read into the user buffer so we can clear all the progress we made in h_buf
		FV -> bytes_writ = 0;

		return count;
	}


	if (FV -> bytes_writ == 0) {
		memset(FV -> hidden_buf, '\0', FV -> buf_size); //clear the buffer
	}

	if (((FV -> bytes_writ) + count) >= (FV -> buf_size)) {

		int leftovers = (FV -> buf_size) - (FV -> bytes_writ);

		memcpy(FV -> hidden_buf + FV -> bytes_writ, (char *) buf, leftovers);//fill hidden_buf to its brim

		write(FV -> fd, FV->hidden_buf, FV -> buf_size); //write what is in the hidden_buf to the file

		memset(FV -> hidden_buf, '\0', FV -> buf_size); //clear the buffer

		FV -> bytes_writ = 0; //after writing the leftovers, we want to set it back to 0
		FV -> bytes_writ_tot += leftovers;
		FV -> user_offset += leftovers;

		int carryover = count - leftovers;

		memcpy(FV -> hidden_buf, (char *) buf + leftovers, carryover);

		FV -> bytes_writ = carryover;

		FV -> bytes_writ_tot += carryover;

		FV -> user_offset += carryover;


	}
	else {
		memcpy(FV -> hidden_buf + FV -> bytes_writ, (char *) buf, count);
		FV -> bytes_writ += count;
		FV -> bytes_writ_tot += count;
		FV -> user_offset += count;
	}

	if (FV -> bytes_writ_tot == FV -> size && FV -> not_writ_yet == false) { //it needs to do the second write for the carrovered bytes after leftovers is done
		write(FV -> fd, FV -> hidden_buf, FV -> buf_size);
	}

	//use my bytes_read_tot as a file offset to see if I am about to get to end of file.

	return (count); //return the number of bytes read on this specific call
}	

ssize_t myflush(struct FILER *FV) {

	int n = write(FV -> fd, FV -> hidden_buf, FV -> bytes_writ);

	memset(FV->hidden_buf, '\0', FV -> buf_size);

	FV -> bytes_writ = 0;
	FV -> bytes_writ_tot += FV -> bytes_writ;
	FV -> user_offset += FV -> bytes_writ;

	return 0;
}

off_t myseek(struct FILER *FV, off_t offset, int whence) {


	if (whence == SEEK_SET) {
		lseek(FV -> fd, offset, SEEK_SET);
		return offset;
	}
	else if (whence == SEEK_CUR) {
		lseek(FV -> fd, FV -> user_offset + offset, SEEK_SET);
		return (FV -> user_offset + offset);
	}
	else {
		return 0;
	}


}


struct FILER *myopen(const char *pathname, int flags) {
	/*
	 * Step 1: Create new open file description (not descriptor.
	 * 						Descriptor refers to the description)
	 * Step 2: Record the offset (current position in the file) and the status flags specified.
	 * Step 3: 
	 * */

	struct stat *st = malloc(sizeof(struct stat));
	stat(pathname, st);

	//char* flag_string = (char *) flags;

	printf("FLAG: %d\n", flags);

	int fd = open(pathname, flags);

	printf("FILE DESCRIPTOR: %d\n", fd);

	struct FILER *FV = malloc(sizeof(struct FILER));
	FV -> fd = fd;
	FV -> buf_size = 100; //arbitrary amount of memory
	FV -> hidden_buf = malloc(FV -> buf_size); //arbitrary amount
	FV -> bytes_read = 0;
	FV -> bytes_read_tot = 0;
	FV -> bytes_read_last = 0;
	FV -> bytes_writ = 0;
	FV -> bytes_writ_tot = 0;// file offset
	FV -> not_read_yet = true;
	FV -> not_writ_yet = true;
	FV -> flag_val = flags;
	FV -> size = st -> st_size;
	FV -> user_offset = 0;

	return FV;
}

int myclose(struct FILER *FV) {
	int n = close(FV->fd);

	free(FV->hidden_buf);
	free(FV);

	return n;
}

