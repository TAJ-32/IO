#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "myio.h" 

ssize_t myread(struct FILER *FV, void *buf, size_t count) { //count is how many bytes we are reading

	/*need to create a struct that has as properties:
	 *	- fd
	 *	- buf2
	 *	- the size specified by the first myread call
	 *we will pass that struct through on every call of myread so that it knows where to start adding to the buffer from the file
	 */
	
	//if a WRONLY is specified.
	if (FV->flags == 1 || FV->flags == 65) { 
		perror("No Read Access");
		exit(-1);
	}
	
	//if the whole file has been read, just be done
	if ((FV->bytes_read_tot) == (FV->size)) {
		return 0;
	}

	//don't read more than the file size
	if ((FV->bytes_read_tot) + count >= (FV->size)) {
		count = (FV->size) - (FV->bytes_read_tot);
	}

	//if they want to read more than the specified buf_size, just do the actual read syscall and 
	//do a little pointer arithmetic to keep hidden_buf accurate
	if (count >= FV->buf_size) {
		int rest_of_hidden = FV->buf_size - FV->bytes_read;
		//copy the rest of h_buf into buf
		memcpy(buf, FV->hidden_buf + FV->bytes_read, rest_of_hidden); 
		if (FV->bytes_read == 0) {
			rest_of_hidden = 0;
		}
		if (read(FV->fd, (char *) buf + rest_of_hidden, count-rest_of_hidden) < 0) {
			perror("Error reading file");
			exit(-1);
		}
		FV->bytes_read_tot += count;
		FV->user_offset += count;

		memset(FV->hidden_buf, '\0', FV->buf_size); 
		FV->bytes_read = 0;

		return count;
	}

	//if first read, we want to do the actual read syscall
	if (FV->not_read_yet) {
		if (read(FV->fd, FV->hidden_buf, FV->buf_size) < 0) {
			perror("Error reading file");
			exit(-1);	
		}
		memcpy(buf, FV->hidden_buf, count);
		FV->bytes_read += count;
		FV->bytes_read_tot += count;
		FV->user_offset += count;
		FV->not_read_yet = false;
	}
	//if this next myread will take us over the buf size, we want to do pointer arithmetic to keep hidden_buf accurate
	else if (((FV->bytes_read) + count) >= (FV->buf_size)) {

		int leftovers = (FV->buf_size) - (FV->bytes_read);

		//copy the few bits that will get lost in between the myread and the syscall to buf
		memcpy((char *) buf, FV->hidden_buf + FV->bytes_read, leftovers); 

		FV->bytes_read += leftovers;

		memset(FV->hidden_buf, '\0', FV->buf_size);
		read(FV->fd, FV->hidden_buf, FV->buf_size);
		
		int carryover = count - leftovers;

		memcpy((char *) buf + leftovers, FV->hidden_buf, carryover);

		FV->bytes_read = carryover;
		FV->bytes_read_tot += count;
		FV->user_offset += count;
	}
	else {
		//copy however many bytes the user specifies from the hidden buf into the user buf. We do count + 1 to account for null char
		memcpy((char *) buf, FV->hidden_buf + FV->bytes_read, count); 								     
		FV->bytes_read += count;
		FV->bytes_read_tot += count;
		FV->user_offset += count;
	}


	return (count); 
}

ssize_t mywrite(struct FILER *FV, void *buf, size_t count) {

	//if a RDONLY is specified
	if (FV->flags == 0 || FV->flags == 64) { 
		perror("No write access");
		exit(-1);
	}
	
	//int old_offset = FV -> user_offset;
	
	myseek(FV, FV->user_offset, SEEK_SET);
	
	if (FV->flags >= 1024) {
		myseek(FV, FV->size, SEEK_SET);
	}



	if (count >= FV->buf_size) {
		int flushed = FV->bytes_writ;
		myflush(FV);
		if (write(FV->fd, (char *) buf, count) < 0) {
			perror("Error writing to file");
			exit(-1);
		}
		FV->bytes_writ_tot += count - flushed;
		FV->user_offset += count - flushed;
		memset(FV->hidden_buf, '\0', FV->buf_size); 
		FV->bytes_writ = 0;

		return count;
	}


	if (FV->bytes_writ == 0) {
		//clear the buffer
		memset(FV->hidden_buf, '\0', FV->buf_size); 
	}

	if (((FV->bytes_writ) + count) >= (FV->buf_size)) {

		int leftovers = (FV->buf_size) - (FV->bytes_writ);

		//fill hidden_buf to its brim
		memcpy(FV->hidden_buf + FV->bytes_writ, (char *) buf, leftovers);

		 //write what is in the hidden_buf to the file
		write(FV->fd, FV->hidden_buf, FV->buf_size);

		memset(FV->hidden_buf, '\0', FV->buf_size); 

		//after writing the leftovers, we want to set it back to 0
		FV->bytes_writ = 0; 
		FV->bytes_writ_tot += leftovers;
		FV->user_offset += leftovers;

		int carryover = count - leftovers;

		memcpy(FV->hidden_buf, (char *) buf + leftovers, carryover);

		FV->bytes_writ = carryover;

		FV->bytes_writ_tot += carryover;

		//FV->user_offset += carryover;


	}
	else {
		memcpy(FV->hidden_buf + FV->bytes_writ, (char *) buf, count);

		FV->bytes_writ += count;
		FV->bytes_writ_tot += count;
		//FV->user_offset += count;
	}

	//it needs to do the second write for the carrovered bytes after leftovers is done
	if (FV->bytes_writ_tot == FV->size && FV->not_writ_yet == false) { 
		write(FV->fd, FV->hidden_buf, FV->buf_size);
		FV->user_offset += count;
	}

	//FV->user_offset = old_offset;

	return (count); 
}	

ssize_t myflush(struct FILER *FV) {

	if (write(FV->fd, FV->hidden_buf, FV->bytes_writ) < 0) {
		perror("Error writing to file");
		exit(-1);
	}

	memset(FV->hidden_buf, '\0', FV->buf_size);

	FV->bytes_writ = 0;
	FV->bytes_writ_tot += FV->bytes_writ;
	FV->user_offset += FV->bytes_writ;

	return 0;
}

off_t myseek(struct FILER *FV, off_t offset, int whence) {


	if (whence == SEEK_SET) {
		if (lseek(FV->fd, offset, SEEK_SET) < (off_t) -1) {
			perror("lseek error");
			exit((off_t) -1);
		}
		FV->user_offset = offset;
		return offset;
	}
	else if (whence == SEEK_CUR) {
		if (lseek(FV->fd, FV->user_offset + offset, SEEK_SET) < (off_t) -1) {
			perror("lseek error");
			exit((off_t) -1);
		}	
		FV->user_offset += offset;
		return (FV->user_offset + offset);
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

	int fd = open(pathname, flags);

	if (fd < 0) {
		perror("File could not be opened");
		exit(-1);
	}

	int buf_size = 100;

	struct FILER *FV = malloc(sizeof(struct FILER));
	FV->fd = fd;
	FV->buf_size = buf_size; 
	FV->hidden_buf = malloc(FV->buf_size); 
	FV->bytes_read = 0;
	FV->bytes_read_tot = 0;
	FV->bytes_read_last = 0;
	FV->bytes_writ = 0;
	FV->bytes_writ_tot = 0;
	FV->not_read_yet = true;
	FV->not_writ_yet = true;
	FV->flags = flags;
	FV->size = st->st_size;
	FV->user_offset = 0;

	return FV;
}

int myclose(struct FILER *FV) {

	if (FV->bytes_writ > 0){
		//if still things in the hidden_buf, we flush
		myflush(FV); 
	}

	int n = close(FV->fd);

	if (n < 0) {
		perror("Error closing file");
		exit(-1);
	}

	free(FV->hidden_buf);
	free(FV);

	return n;
}

