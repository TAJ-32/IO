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
	
	//int a = myseek(FV, 30, SEEK_SET);
	
	//int n = myread(FV, buf, 10);
	//n = myread(FV, buf + 10, 210); 

	//n = mywrite(FV3, buf, 70);
	//n = mywrite(FV3, buf + 70, 140); 

	write(FV2 -> fd, buf, 250);

	
	int n = myread(FV, buf, 10); 
	//printf("buf: %s\n", buf);

	n = myread(FV, buf + 10, 40);
	//printf("buf + 10: %s\n", buf + 10);

	n = myread(FV, buf + 49, 48);
	//printf("buf + 49: %s\n", buf + 49);

	n = myread(FV, buf + 97, 52);
	//printf("buf + 97: %s\n", buf + 97);

	n = myread(FV, buf + 149, 30);
	//printf("buf + 149: %s\n", buf + 149);

	n = myread(FV, buf + 179, 20);
	//printf("buf + 179: %s\n", buf + 179);

	n = myread(FV, buf + 199, 50);
	//printf("buf + 199: %s\n", buf + 199);

	write(FV2 -> fd, buf, 250);



	//printf("buf b4 write: %s\n", buf);

	mywrite(FV3, buf, 80);
	mywrite(FV3, buf + 80, 80);
	mywrite(FV3, buf + 160, 80);
	mywrite(FV3, buf + 240, 10);
	
	

	/*
	int n = myread(FV, buf, 40);
	n = myread(FV, buf + 40, 120);
	n = myread(FV, buf + 160, 90);


	write(FV2->fd, buf, 250);

	
	mywrite(FV3, buf, 20);
	mywrite(FV3, buf + 20, 50);
	mywrite(FV3, buf + 70, 130);
	mywrite(FV3, buf + 200, 50);
*/

	int count_for_flush = (FV3 -> bytes_writ); //this will be the amount in FV3's hidden buf if it hasn't been flushed out yet

	int b = myflush(FV3, count_for_flush);


	char buf2[19] = "\n WASSSSSSUUPPPPPP";

	//mywrite(FV3, buf2 + 250, 19);

	//count_for_flush = (FV3 -> bytes_writ);

	//int c = myflush(FV3, count_for_flush);

	//struct FILER *FV2 = myopen("/home/sartaj/Assignments/assignment2/Testfile.txt", O_CREAT);

	//n = mywrite(FV2, "hello there", 12);

	//char *buffer = malloc(1024 * sizeof(char));

	return 0;
}



