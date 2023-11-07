#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
#include "myio.h"

int main(int argc, char *argv[]) {
	struct FILER *FV = myopen("/home/sartaj/Assignments/assignment2/Testfile.txt", O_RDWR);

	//printf("HELLO: %d\n", FV -> fd);

	struct FILER *FV2 = myopen("/home/sartaj/Assignments/assignment2/Outfile.txt", O_WRONLY);

	struct FILER *FV3 = myopen("/home/sartaj/Assignments/assignment2/Outfile2.txt", O_CREAT | O_RDWR);

	char buf[256];

	//buf = buf + FV->offset //this will start the buffer to where lseek told it to start at once we make lseek
	
	int a = myseek(FV, 30, SEEK_SET);

	/*
	int n = myread(FV, buf, 10); 

	n = myread(FV, buf, 40);

	n = myread(FV, buf, 48);

	n = myread(FV, buf, 52);

	n = myread(FV, buf, 30);

	n = myread(FV, buf, 20);

	n = myread(FV, buf, 50);
	*/

	int n = myread(FV, buf, 40);
	n = myread(FV, buf, 120);
	n = myread(FV, buf, 90);


	write(FV2->fd, buf, 250);

	
	mywrite(FV3, buf, 20);
	mywrite(FV3, buf, 50);
	mywrite(FV3, buf, 130);
	mywrite(FV3, buf, 50);


	int count_for_flush = (FV3 -> bytes_writ); //this will be the amount in FV3's hidden buf if it hasn't been flushed out yet

	int b = myflush(FV3, count_for_flush);


	char buf2[19] = "\n WASSSSSSUUPPPPPP";

	mywrite(FV3, buf2, 19);

	count_for_flush = (FV3 -> bytes_writ);

	int c = myflush(FV3, count_for_flush);

	//struct FILER *FV2 = myopen("/home/sartaj/Assignments/assignment2/Testfile.txt", O_CREAT);

	//n = mywrite(FV2, "hello there", 12);

	//char *buffer = malloc(1024 * sizeof(char));

	return 0;
}



