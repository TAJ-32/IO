#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <string.h>
#include "myio.h"

int main(int argc, char *argv[]) {
	struct FILER *FV = myopen("/home/sartaj/Assignments/assignment1/ToDo.txt", O_RDONLY);

	//printf("HELLO: %d\n", FV -> fd);

	
	char *buf = malloc(sizeof(char *));

	int n = myread(FV, buf, 10); 
	printf("first read done\n");
	n = myread(FV, buf, 40);
	printf("second read done\n");
	n = myread(FV, buf, 48);
	printf("third read done\n");
	n = myread(FV, buf, 52);
	printf("fourth read done\n");
	n = myread(FV, buf, 30);
	printf("fifth read done\n");

	printf("%d\n", n);

	//char *buffer = malloc(1024 * sizeof(char));

	return 0;
}



