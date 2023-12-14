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
	if (FV->flags & O_WRONLY) { 
		return 1;
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
		if (FV->not_read_yet) {
			if (read(FV->fd, buf, count) < 0) {
				return 1;
			}
			FV->bytes_read_tot += count;
			FV->offset += count;
			FV->user_offset += count;
			//we don't set not_read_yet to true because they haven't made a read for little amount of bytes so we just use the actual syscall until they want to do that

			return count;
		}

		int rest_of_hidden = FV->buf_size - FV->buf_offset; //maybe bytes_read
		
		//copy the rest of h_buf into buf
		memcpy(buf, FV->hidden_buf + FV->buf_offset, rest_of_hidden);

		if (FV->bytes_writ > 0) {
			myflush(FV);
		}

		if (FV->bytes_read == 0) {
			rest_of_hidden = 0;
		}
		if (read(FV->fd, (char *) buf + rest_of_hidden, count-rest_of_hidden) < 0) {
			return 1;
	
		}
		FV->bytes_read_tot += count;
		FV->offset += count-rest_of_hidden;

		FV->user_offset += count;

		memset(FV->hidden_buf, '\0', FV->buf_size); 
		FV->bytes_read = 0;
		FV->buf_offset = 0;

		return count;
	}


	//if first read, we want to do the actual read syscall
	if (FV->not_read_yet) {
		if (read(FV->fd, FV->hidden_buf, FV->buf_size) < 0) {
			return 1;
		}
		memcpy(buf, FV->hidden_buf, count);
		FV->bytes_read += count;
		FV->buf_offset += count;
		FV->bytes_read_tot += count;
		FV->offset += FV->buf_size;
		FV->user_offset += count;
		FV->not_read_yet = false;

		return count;
	}
	//if this next myread will take us over the buf size, we want to do pointer arithmetic to keep hidden_buf accurate
	else if (((FV->buf_offset) + count) >= (FV->buf_size)) {

		int leftovers = (FV->buf_size) - (FV->buf_offset);

		//copy the few bits that will get lost in between the myread and the syscall to buf
		memcpy((char *) buf, FV->hidden_buf + FV->buf_offset, leftovers); 
		FV->bytes_read += leftovers;
		FV->buf_offset += leftovers;

		if (FV->bytes_writ > 0) {
			myflush(FV);
		}

		memset(FV->hidden_buf, '\0', FV->buf_size);
		if (read(FV->fd, FV->hidden_buf, FV->buf_size) < 0) {
			return 1;
		}
		
		int carryover = count - leftovers;

		memcpy((char *) buf + leftovers, FV->hidden_buf, carryover);

		FV->bytes_read = carryover;
		FV->buf_offset = carryover;
		FV->bytes_writ = 0; //do this because the carryover for the hidden_buf is now only going to be read bytes not written bytes, so any writing to the hidden buf that happened before that should be reset
		FV->bytes_read_tot += count;
		FV->user_offset += count;
		FV->offset += FV->buf_size;
	}
	else {
		//copy however many bytes the user specifies from the hidden buf into the user buf. We do count + 1 to account for null char
		memcpy((char *) buf, FV->hidden_buf + FV->buf_offset, count);
		FV->bytes_read += count;
		FV->buf_offset += count;
		FV->bytes_read_tot += count;
		FV->user_offset += count;
	}


	return count; 
}

ssize_t mywrite(struct FILER *FV, void *buf, size_t count) {

	//if a RDONLY is specified
	if (FV->flags & O_RDONLY) { 
		return 1;
	}
	
	//int old_offset = FV -> user_offset;
	
	if (FV->flags & O_APPEND) {
		myseek(FV, FV->size, SEEK_SET);
	}

	if (count >= FV->buf_size) {
		int flushed = FV->bytes_writ;
		if (flushed != 0) {
			FV->close_flush = true;
			myflush(FV);
		}
		if (FV->bytes_writ != 0) {
			if (lseek(FV->fd, -1*FV->buf_size, SEEK_CUR) < (off_t) -1) {
				return 1;
			}
		}

		if (write(FV->fd, (char *) buf, count) < 0) {
			return 1;
		}
		FV->bytes_writ_tot += count - flushed;
		FV->offset += count;
		FV->user_offset += count;
		memset(FV->hidden_buf, '\0', FV->buf_size); 
		FV->bytes_writ = 0;
		FV->buf_offset = 0;

		return count;
	}


	if (FV->bytes_writ == 0 && FV->buf_offset == 0) {
		//clear the buffer
		memset(FV->hidden_buf, '\0', FV->buf_size); 
	}

	if (((FV->buf_offset) + count) >= (FV->buf_size)) {

		int leftovers = (FV->buf_size) - (FV->buf_offset);

		//fill hidden_buf to its brim
		memcpy(FV->hidden_buf + FV->buf_offset, (char *) buf, leftovers);
		FV->buf_offset += leftovers;
		//write what is in the hidden_buf to the file
		if (FV->bytes_writ != 0) {
			if (lseek(FV->fd, -1*FV->buf_size, SEEK_CUR) < (off_t) -1) {
				return 1;
			}
		}

		if (write(FV->fd, (char *) buf, FV->buf_size) < 0) {
			return 1;
		}

		memset(FV->hidden_buf, '\0', FV->buf_size); 

		//after writing the leftovers, we want to set it back to 0
		FV->bytes_writ = 0; 
		FV->buf_offset = 0;
		FV->bytes_writ_tot += leftovers;
		FV->user_offset += leftovers;
		FV->offset += FV->buf_size;

		int carryover = count - leftovers;

		memcpy(FV->hidden_buf, (char *) buf + leftovers, carryover);

		FV->bytes_writ = carryover;
		FV->buf_offset = carryover;

		FV->bytes_writ_tot += carryover;

		FV->user_offset += carryover;


	}
	else {
		memcpy(FV->hidden_buf + FV->buf_offset, (char *) buf, count);
		FV->bytes_writ += count;
		FV->buf_offset += count;
		FV->bytes_writ_tot += count;
		FV->user_offset += count;
	}

	//it needs to do the second write for the carrovered bytes after leftovers is done
	if (FV->bytes_writ_tot == FV->size && FV->not_writ_yet == false) { 
		if (FV->bytes_writ != 0) {
			if (lseek(FV->fd, -1*FV->buf_size, SEEK_CUR) < (off_t) -1) {
				return 1;
			}
		}

		if (write(FV->fd, (char *) buf, count) < 0) {
			return 1;
		}
		FV->user_offset += count;
		FV->offset += count;
	}

	//FV->user_offset = old_offset;

	return (count); 
}	

ssize_t myflush(struct FILER *FV) {

	if (FV->bytes_writ != 0 && FV->offset >= FV->buf_size) {
		if (FV->offset == (lseek(FV->fd, -1*FV->buf_size, SEEK_CUR) < (off_t) -1)) {
			return 1;
		}
	}

	if (FV->close_flush == true) {
		if (write(FV->fd, FV->hidden_buf, FV->buf_offset) < 0) {
			return 1;
		}
		FV->close_flush = false;
	}
	else {
		if (write(FV->fd, FV->hidden_buf, FV->buf_size) < 0) {
			return 1;
		}

	}

	memset(FV->hidden_buf, '\0', FV->buf_size);

	FV->bytes_writ = 0;
	FV->buf_offset = 0;
	FV->bytes_writ_tot += FV->buf_offset;
	//FV->user_offset += FV->buf_offset;
	FV->offset += FV->buf_offset;

	return 0;
}

off_t myseek(struct FILER *FV, off_t offset, int whence) {


	if (whence == SEEK_SET) {
		if (lseek(FV->fd, offset, SEEK_SET) < (off_t) -1) {
			return 1;
		}
		FV->offset = offset;
		FV->user_offset = offset;
		return offset;
	}
	else if (whence == SEEK_CUR) {
		if (lseek(FV->fd, FV->user_offset + offset, SEEK_SET) < (off_t) -1) {
			return 1;
		}	
		FV->offset = FV->user_offset + offset;
		FV->user_offset += offset;
		return (FV->user_offset + offset);
	}
	else {
		return 0;
	}


}


struct FILER *myopen(const char *pathname, int flags, mode_t mode) {
	/*
	 * Step 1: Create new open file description (not descriptor.
	 * 						Descriptor refers to the description)
	 * Step 2: Record the offset (current position in the file) and the status flags specified.
	 * Step 3: 
	 * */

	struct stat *st = malloc(sizeof(struct stat));
	stat(pathname, st);

	int fd = open(pathname, flags, mode);

	if (fd < 0) {
		return NULL;
	}

	struct FILER *FV = malloc(sizeof(struct FILER));
	FV->fd = fd;
	FV->buf_size = BUF_SIZE; 
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
	FV->offset = 0;
	FV->buf_offset = 0;
	FV->close_flush = false;

	return FV;
}

int myclose(struct FILER *FV) {

	if (FV->bytes_writ > 0){
		//if still things in the hidden_buf, we flush
		FV->close_flush = true;
		myflush(FV); 
	}

	int n = close(FV->fd);

	if (n < 0) {
		return 1;
	}

	free(FV->hidden_buf);
	free(FV);

	return n;
}
