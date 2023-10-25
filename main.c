#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
#include "myio.h"

int main(int argc, char *argv[]) {
	struct FILER *FV = myopen("/home/sartaj/Assignments/assignment2/Testfile.txt", O_RDONLY);

	//printf("HELLO: %d\n", FV -> fd);

	
	char *buf = malloc(sizeof(char *));

	int n = myread(FV, buf, 10); 
	printf("first read done\n");
	printf("RETURN: %d\n", n);	
	n = myread(FV, buf, 40);
	printf("second read done\n");
	printf("RETURN: %d\n", n);	
	n = myread(FV, buf, 48);
	printf("third read done\n");
	printf("RETURN: %d\n", n);	
	n = myread(FV, buf, 52);
	printf("fourth read done\n");
	printf("RETURN: %d\n", n);	
	n = myread(FV, buf, 30);
	printf("fifth read done\n");
	printf("RETURN: %d\n", n);


	//struct FILER *FV2 = myopen("/home/sartaj/Assignments/assignment2/Testfile.txt", O_CREAT);

	//n = mywrite(FV2, "hello there", 12);

	//char *buffer = malloc(1024 * sizeof(char));

	return 0;
}



