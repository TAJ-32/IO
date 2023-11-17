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
	
	/*
	int n = myread(FV, buf, 10);
	n = myread(FV, buf + 10, 210); 

	n = mywrite(FV3, buf, 70);
	n = mywrite(FV3, buf + 70, 150); 
	*/
	int a = myseek(FV3, 30, SEEK_SET);
	int n = myread(FV3, buf, 25);
	n = myread(FV3, buf + 25, 85);
	a = myseek(FV3, 0, SEEK_SET);
	int m = mywrite(FV3, buf, 20);
	m = mywrite(FV3, buf + 20, 90);
	int o = myclose(FV3);

	//printf("buf b4 write: %s\n", buf);

	/*
	mywrite(FV3, buf, 80);
	mywrite(FV3, buf + 80, 80);
	mywrite(FV3, buf + 160, 80);
	mywrite(FV3, buf + 240, 10);
	*/
	

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
	//int b = myflush(FV3);


	//char buf2[19] = "\n WASSSSSSUUPPPPPP";

	//mywrite(FV3, buf2 + 250, 19);

	//int c = myflush(FV3);

	//struct FILER *FV2 = myopen("/home/sartaj/Assignments/assignment2/Testfile.txt", O_CREAT);

	//n = mywrite(FV2, "hello there", 12);

	//char *buffer = malloc(1024 * sizeof(char));

	return 0;
}



