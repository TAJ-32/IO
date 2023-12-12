#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
#include "myio.h"

int main(int argc, char *argv[]) {
	struct FILER *FV = myopen("/home/sartaj/Assignments/assignment2/Testfile.txt", O_RDWR, 0777);

	//printf("HELLO: %d\n", FV -> fd);

	struct FILER *FV2 = myopen("/home/sartaj/Assignments/assignment2/Outfile.txt", O_RDWR | O_CREAT, 0777);

	struct FILER *FV3 = myopen("/home/sartaj/Assignments/assignment2/Outfile2.txt", O_RDWR, 0777);

	char buf[1024];
	char buf2[1024];

	char buf3[25] = "Hello my name is Sartaj!\0";

	int a = mywrite(FV2, buf3, 25);

/*	
	int x = read(4, buf2, 120);
//	printf("buf2: %s\n", buf2);
	int y = write(4, buf2, 120);
	x = read(4, buf2, 160);
	y = write(4, buf2, 160);
	x = read(4, buf2, 30);
	y = write(4, buf2, 30);
	x = read(4, buf2, 60);
	y = write(4, buf2, 60);
	x = read(4, buf2, 130);
	y = write(4, buf2, 130);
	x = read(4, buf2, 10);
	y = read(4, buf2, 10);

	int a = myread(FV3, buf, 120);
	int b = mywrite(FV3, buf, 120);
	a = myread(FV3, buf + 120, 160);
	b = mywrite(FV3, buf + 120, 160);
	a = myread(FV3, buf + 280, 30);
	b = mywrite(FV3, buf + 280, 30);
	a = myread(FV3, buf + 310, 60);
	b = mywrite(FV3, buf + 310, 60);
	a = myread(FV3, buf + 370, 130);
	b = mywrite(FV3, buf + 370, 130);
	a = myread(FV3, buf + 500, 10);
	b = myread(FV3, buf + 500, 10);
*/
	int c = myclose(FV2);
	
	c = myclose(FV);
	c = myclose(FV3);
	
	//printf("a: %d, c: %d\n", a, c);
	return 0;
}



