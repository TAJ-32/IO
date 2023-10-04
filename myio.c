#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

int myopen(const char *pathname, int flags);

int main(int argc, char *argv[]) {
	myopen("/home/sartaj/LAB/Day_One/copy-file.c", O_RDONLY);
	return 0;
}

int myopen(const char *pathname, int flags) {
	/*
	 * Step 1: Create new open file description (not descriptor.
	 * 						Descriptor refers to the description)
	 * Step 2: Record the offset (current position in the file) and the status flags specified.
	 * Step 3: 
	 * */


	int file_desc = open(pathname, flags);

	printf("FILE DESCRIPTOR: %d\n", file_desc);


	return file_desc;



}

