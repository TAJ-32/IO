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

	struct FILER *FV3 = myopen("/home/sartaj/Assignments/assignment2/Outfile2.txt", O_RDWR | O_APPEND);

	char buf[256];
	
	int a = myread(FV2, buf, 20);
	printf("buf: %s\n", buf);
	printf("offset: %d\n", FV2->user_offset);
	int b = mywrite(FV2, buf, 20);

	//a = myread(FV2, buf + 20, 30);
	//printf("buf + 20: %s\n", buf + 20);
	//int d = myseek(FV2, 100, SEEK_SET);
	//b = mywrite(FV2, buf + 20, 30);

	int c = myclose(FV2);
	



	return 0;
}



