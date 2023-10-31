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

	if (FV->not_read_yet) {
		read(FV->fd, FV->hidden_buf, FV->buf_size);
		memcpy(buf, FV->hidden_buf, count); 
		FV -> bytes_read += count;
		FV -> bytes_read_tot += count;
		FV -> not_read_yet = false;
	}
	else if (((FV -> bytes_read) + count) >= (FV -> buf_size)) {

		int leftovers = (FV -> buf_size) - (FV->bytes_read);

		memcpy((char *) buf + FV->bytes_read_tot, FV->hidden_buf + FV -> bytes_read, leftovers); //copy the few bits that will get lost in between the myread and the syscall to buf

		FV->bytes_read += leftovers;//read two more bytes
		
		memset(FV->hidden_buf, '\0', FV -> buf_size);
		read(FV->fd, FV->hidden_buf, FV -> buf_size);
		
		int carryover = count - leftovers;

		memcpy((char *) buf + FV->bytes_read_tot + leftovers, FV->hidden_buf, carryover);

		FV -> bytes_read = carryover;
		FV -> bytes_read_tot += count;
	}
	else {
		memcpy((char *) buf + FV->bytes_read_tot, FV->hidden_buf + FV->bytes_read, count); //copy however many bytes the user specifies from the hidden buf into the user buf. We do count + 1 to account for null char
								     
		FV -> bytes_read += count;
		FV -> bytes_read_tot += count;
	}

	FV -> bytes_read_last = count;

//use my bytes_read_tot as a file offset to see if I am about to get to end of file.

	return (count); //return the number of bytes read on this specific call
}

ssize_t mywrite(struct FILER *FV, void *buf, size_t count) { //count is how many bytes we are writing
	if (FV -> flag_val == 0 || FV -> flag_val == 64) { //if a WRONLY is specified
		perror("No write access");
		exit(-1);
	}
	/*
	if ((FV -> bytes_writ_tot) == (FV -> size)) {
		printf("DONE\n");
		return 0;
	}*/
	
	/*//I think this is maybe not my job because the user has to make sure they aren't writing more than they allocated 
	if ((FV -> bytes_writ_tot) + count >= (FV -> size)) {
		count = (FV -> size) - (FV -> bytes_read_tot);
	}*/

	printf("bytes_writ: %d\n", FV -> bytes_writ);
	printf("bytes_writ_tot: %d\n", FV -> bytes_writ_tot);

	if (((FV -> bytes_writ) + count) >= (FV -> buf_size)) {

		printf("hidden_buf thus far: %s\n", FV->hidden_buf);

		int leftovers = (FV -> buf_size) - (FV -> bytes_writ);

		memcpy(FV -> hidden_buf + FV -> bytes_writ, (char *) buf + FV->bytes_writ_tot, leftovers);//fill hidden_buf to its brim
		printf("after leftover copy: %s\n", FV->hidden_buf);


		write(FV -> fd, FV->hidden_buf, FV -> buf_size); //write what is in the hidden_buf to the file
		FV -> not_writ_yet = false;
		memset(FV -> hidden_buf, '\0', FV -> buf_size); //clear the buffer

		FV -> bytes_writ = 0; //after writing the leftovers, we want to set it back to 0
		FV -> bytes_writ_tot += leftovers;

		int carryover = count - leftovers;

		if (carryover >= FV -> buf_size) { //in the case that the user asked to write a substantial amount more than the bufsize when we were already close to the buffer being filled
			while (carryover >= FV -> buf_size) {
				printf("in loop\n");
				leftovers = (FV -> buf_size);
				memcpy(FV -> hidden_buf, (char *) buf + FV->bytes_writ_tot, leftovers);//fill hidden_buf to its brim
				printf("loop leftover: %s\n", FV -> hidden_buf);
				write(FV->fd, FV->hidden_buf, FV->buf_size);

				FV -> bytes_writ = 0;
				FV -> bytes_writ_tot += leftovers;

				carryover = carryover - leftovers;
				
				memcpy(FV -> hidden_buf, (char *) buf + FV->bytes_writ_tot, carryover);
				printf("loop carryover: %s\n", FV -> hidden_buf);

				FV -> bytes_writ += carryover;
				FV -> bytes_writ_tot += carryover;
			}
		}
		else {
			memcpy(FV -> hidden_buf, (char *) buf + FV->bytes_writ_tot, carryover);
			printf("after carryover copy: %s\n", FV->hidden_buf);

			FV -> bytes_writ = carryover;

			FV -> bytes_writ_tot += carryover;
		}

	}
	else {
		printf("Hidden buf: %s\n", FV->hidden_buf);

		memcpy(FV -> hidden_buf + FV -> bytes_writ, (char *) buf + FV -> bytes_writ_tot, count);

		FV -> bytes_writ += count;
		FV -> bytes_writ_tot += count;
	}

	if (FV -> bytes_writ_tot == FV -> size && FV -> not_writ_yet == false) { //it needs to do the second write for the carrovered bytes after leftovers is done
		write(FV -> fd, FV -> hidden_buf, FV -> buf_size);
	}

	//use my bytes_read_tot as a file offset to see if I am about to get to end of file.

	return (count); //return the number of bytes read on this specific call
	
/*	
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
*/
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

	return FV;
}

