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
	//printf("Count: %ld, Bytes: %d\n", count, bytes_so_far);
	if ((FV -> bytes_read_tot) == (FV -> size)) {
		printf("DONE\n");
		return 0;
	}

	if ((FV -> bytes_read_tot) + count >= (FV -> size)) {
		//printf("Nearing the end: %d %ld\n", FV->bytes_read_tot, count);
		count = (FV -> size) - (FV -> bytes_read_tot);
		//printf("count: %ld\n", count);
	}

	if (FV->not_read_yet) {
		read(FV->fd, FV->hidden_buf, FV->buf_size);
		printf("FIRST READ SYSCALL\n");
		memcpy(buf, FV->hidden_buf, count); 
		FV -> bytes_read += count;
		FV -> bytes_read_tot += count;
		FV -> not_read_yet = false;
		//printf("memcpy systcall: %s\n", (char *) buf);

	}
	else if (((FV -> bytes_read) + count) >= (FV -> buf_size)) {

		printf("Reset Buffer Case\n");

		//maybe change that you don't add bytes_read or bytes_read_tot in the actual read syscall
		int leftovers = (FV -> buf_size) - (FV->bytes_read);


		//printf("Before memcpy 1: buf: %s \nh_buf: %s \nbytes_read: %d \nleftovers: %d\n", (char *) buf, FV->hidden_buf, FV->bytes_read, leftovers);
		//printf("Bytes read: %d %s\n", FV->bytes_read, FV->hidden_buf + FV->bytes_read);
		memcpy((char *) buf + FV->bytes_read_tot, FV->hidden_buf + FV -> bytes_read, leftovers); //copy the few bits that will get lost in between the myread and the syscall to buf
		//printf("First memcpy: %s\n", (char *) buf);
		//printf("After memcpy 1: buf: %s \nh_buf: %s \nbytes_read: %d \nleftovers: %d\n", (char *) buf, FV->hidden_buf, FV->bytes_read, leftovers);

		FV->bytes_read += leftovers;//read two more bytes
		
		printf("User buf at leftover: %s\n", (char *) buf);
		memset(FV->hidden_buf, '\0', FV -> buf_size);
		printf("Hidden buf after clear: %s\n", FV->hidden_buf);
		read(FV->fd, FV->hidden_buf, FV -> buf_size);
		printf("Hidden buf after new read: %s\n", FV->hidden_buf);
		
		int carryover = count - leftovers;

		//printf("Before memcpy 2: buf: %s \nh_buf: %s \nbytes_read: %d \nleftovers: %d\n", (char *) buf + leftovers, FV->hidden_buf, FV->bytes_read, leftovers);

		memcpy((char *) buf + FV->bytes_read_tot + leftovers, FV->hidden_buf, carryover);
		printf("User buf at carryover: %s\n", (char *) buf);

		//printf("After memcpy 2: buf: %s \nh_buf: %s \nbytes_read: %d \nleftovers: %d\n", (char *) buf, FV->hidden_buf, FV->bytes_read, leftovers);

		//printf("memcpy: %s\n", (char *) buf);

		FV -> bytes_read = carryover;
		FV -> bytes_read_tot += count;

	}
	else {
		printf("Default case\n");
		//printf("Before memcpy: buf: %s \nh_buf: %s \nbytes_read: %d \ncount: %ld\n", (char *) buf, FV->hidden_buf, FV->bytes_read, count);	
		printf("User buf before memcpy: %s\n", (char *) buf);
		memcpy((char *) buf + FV->bytes_read_tot, FV->hidden_buf + FV->bytes_read, count); //copy however many bytes the user specifies from the hidden buf into the user buf. We do count + 1 to account for	null char
		printf("User buf after memcpy: %s\n", (char *) buf);
								     
		//printf("WHY\n");
		FV -> bytes_read += count;
		FV -> bytes_read_tot += count;

		//printf("After memcpy: buf: %s \nh_buf: %s \nbytes_read: %d \ncount: %ld\n", (char *) buf, FV->hidden_buf, FV->bytes_read, count);

		//printf("memcpy: %s\n", (char *) buf);
		//printf("It's here\n");
	}

	FV -> bytes_read_last = count;

//use my bytes_read_tot as a file offset to see if I am about to get to end of file.

	return (count); //return the number of bytes read on this specific call


	
}

ssize_t mywrite(struct FILER *FV, void *buf, size_t count) { //count is how many bytes we are writing
							     //
		if (FV -> flag_val == 1 || FV -> flag_val == 65) { //if a WRONLY is specified
		perror("No read access");
		exit(-1);
	}

	

	//printf("Count: %ld, Bytes: %d\n", count, bytes_so_far);
	if ((FV -> bytes_read_tot) == (FV -> size)) {
		printf("DONE\n");
		return 0;
	}

	if ((FV -> bytes_read_tot) + count >= (FV -> size)) {
		//printf("Nearing the end: %d %ld\n", FV->bytes_read_tot, count);
		count = (FV -> size) - (FV -> bytes_read_tot);
		//printf("count: %ld\n", count);
	}

	if (FV->not_writ_yet) {
		write(FV->fd, buf, 100);
		//printf("FIRST READ SYSCALL\n");
		memcpy(buf, FV->hidden_buf, count + 1); 
		FV -> bytes_read += count;
		FV -> bytes_read_tot += count;
		FV -> not_read_yet = false;
		//printf("memcpy: %s\n", (char *) buf);

	}
	else if (((FV -> bytes_read) + count) >= (FV -> buf_size)) {

		printf("FINAL STRETCH\n");

		//maybe change that you don't add bytes_read or bytes_read_tot in the actual read syscall
		int leftovers = (FV -> buf_size) - (FV->bytes_read);


		printf("Before memcpy 1: buf: %s \nh_buf: %s \nbytes_read: %d \nleftovers: %d\n", (char *) buf, FV->hidden_buf, FV->bytes_read, leftovers);
		//printf("Bytes read: %d %s\n", FV->bytes_read, FV->hidden_buf + FV->bytes_read);
		memcpy(buf, FV->hidden_buf + FV -> bytes_read, leftovers); //copy the few bits that will get lost in between the myread and the syscall to buf
		//printf("First memcpy: %s\n", (char *) buf);
		printf("After memcpy 1: buf: %s \nh_buf: %s \nbytes_read: %d \nleftovers: %d\n", (char *) buf, FV->hidden_buf, FV->bytes_read, leftovers);

		FV->bytes_read += leftovers;//read two more bytes


		memset(FV->hidden_buf, '\0', 100);
		read(FV->fd, FV->hidden_buf, 100);
		
		int carryover = count - leftovers;

		printf("Before memcpy 2: buf: %s \nh_buf: %s \nbytes_read: %d \nleftovers: %d\n", (char *) buf + leftovers, FV->hidden_buf, FV->bytes_read, leftovers);

		memcpy(((char *) buf) + leftovers, FV->hidden_buf, carryover);

		printf("After memcpy 2: buf: %s \nh_buf: %s \nbytes_read: %d \nleftovers: %d\n", (char *) buf, FV->hidden_buf, FV->bytes_read, leftovers);

		//printf("memcpy: %s\n", (char *) buf);

		FV -> bytes_read = carryover;
		FV -> bytes_read_tot += count;

	}
	else {
		printf("Before memcpy: buf: %s \nh_buf: %s \nbytes_read: %d \ncount: %ld\n", (char *) buf, FV->hidden_buf, FV->bytes_read, count);
		memcpy(buf, FV->hidden_buf + FV->bytes_read, count); //copy however many bytes the user specifies from the hidden buf into the user buf. We do count + 1 to account for	null char
								     
		//printf("WHY\n");
		FV -> bytes_read += count;
		FV -> bytes_read_tot += count;

		printf("After memcpy: buf: %s \nh_buf: %s \nbytes_read: %d \ncount: %ld\n", (char *) buf, FV->hidden_buf, FV->bytes_read, count);

		//printf("memcpy: %s\n", (char *) buf);
		//printf("It's here\n");
	}

	FV -> bytes_read_last = count;

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
	FV -> not_read_yet = true;
	FV -> not_writ_yet = true;
	FV -> flag_val = flags;
	FV -> size = st -> st_size;

	return FV;
}

