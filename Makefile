myio: myio.c
	gcc -Wall -pedantic -g -o myio main.c myio.c

.PHONE: clean
clean:
	rm -f myio
