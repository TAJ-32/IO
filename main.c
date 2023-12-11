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

	struct FILER *FV2 = myopen("/home/sartaj/Assignments/assignment2/Outfile.txt", O_RDWR);

	struct FILER *FV3 = myopen("/home/sartaj/Assignments/assignment2/Outfile2.txt", O_RDWR);

	char buf[1024];
	char buf2[1024];
	
	
	int x = read(4, buf2, 120);
//	printf("buf2: %s\n", buf2);
	int y = write(4, buf2, 120);
	x = read(4, buf2, 160);
	y = write(4, buf2, 160);
	x = read(4, buf, 30);
	y = write(4, buf, 30);
	x = read(4, buf, 60);
	y = write(4, buf, 60);
	x = read(4, buf, 130);
	//printf("buf: %s\n", buf);
	y = write(4, buf, 130);
	

	/*
	int x = myread(FV2, buf2, 120);
	int y = mywrite(FV2, buf2, 120);
	x = myread(FV2, buf2 + 120, 160);
	y = mywrite(FV2, buf2 + 120, 160);
	x = myread(FV2, buf2 + 280, 30);
	y = mywrite(FV2, buf2 + 280, 30);
	int d = myclose(FV2);
	*/

	int a = myread(FV3, buf, 120);
	int b = mywrite(FV3, buf, 120);
	a = myread(FV3, buf + 120, 160);
	b = mywrite(FV3, buf + 120, 160);
	a = myread(FV3, buf + 280, 30);

	b = mywrite(FV3, buf + 280, 30);

	a = myread(FV3, buf + 310, 60);

	b = mywrite(FV3, buf + 310, 60);

	a = myread(FV3, buf + 370, 130);
	printf("off: %d, act_off: %ld\n", FV3->offset, lseek(FV3->fd, 0, SEEK_CUR));
	b = mywrite(FV3, buf + 370, 130);

	int c = myclose(FV3);
//	printf("off: %d, act_off: %ld\n", FV3->offset, lseek(FV3->fd, 0, SEEK_CUR));
	
/*	
	char buf2[256];
	
	int d = myread(FV, buf2, 80);
	int e = mywrite(FV3, buf2, 80);

	int f = myclose(FV);
	int g = myclose(FV3);

	printf("a: %d, b:%d, c:%d, d:%d, e:%d, f:%d, g:%d\n", a, b, c, d, e, f, g);
	*/

	return 0;
}



