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
	
	
	int x = read(4, buf2, 20);
//	printf("buf2: %s\n", buf2);
	int y = write(4, buf2, 20);
	x = read(4, buf2, 60);
	y = write(4, buf2, 60);
	//x = read(4, buf, 30);
	//y = write(4, buf, 30);
	
	
	
	int a = myread(FV3, buf, 20);
//	printf("buf: %s\n h_buf: %s\n\n", buf, FV3->hidden_buf);
	int b = mywrite(FV3, buf, 20);
	//printf("buf: %s h_buf: %s\n\n", buf, FV3->hidden_buf);
	a = myread(FV3, buf + 20, 60);
	b = mywrite(FV3, buf + 20, 60);
	//printf("buf: %s\n\n, h_buf: %s, offset: %d, u_offset: %d, bytes_writ: %d, bytes_read: %d\n", buf, FV3->hidden_buf, FV3->offset, FV3->user_offset, FV3->bytes_writ, FV3->bytes_read);
	//a = myread(FV3, buf + 280, 30);
	//b = mywrite(FV3, buf + 280, 30);
	int c = myclose(FV3);
	
	
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



