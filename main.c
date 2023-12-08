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

	struct FILER *FV3 = myopen("/home/sartaj/Assignments/assignment2/Outfile2.txt", O_RDWR | O_CREAT);

	char buf[256];
	
	
//	int a = read(4, buf, 20);
	//printf("buf: %s\n", buf);
//	int b = write(4, buf, 20);
//	a = read(4, buf, 20);
//	b = write(4, buf, 20);
	//a = read(4, buf, 20);
	//b = write(4, buf, 21);
	
	
	int a = myread(FV2, buf, 20);
	//printf("buf: %s\n", buf);
	//printf("hbuf: %s\n", FV2->hidden_buf);
	int b = mywrite(FV2, buf, 20);
	//a = myread(FV2, buf + 20, 20);
	//b = mywrite(FV2, buf + 20, 20);
	int c = myclose(FV2);

	
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



