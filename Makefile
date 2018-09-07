CC = gcc
CFLAGS = -O2 -Wall -I .

LIB = -lpthread

all: tiny cgi

tiny: tiny.c csapp.o
	$(CC) $(CFLAGS) -o tiny tiny.c csapp.o $(LIB)

caspp.o: csapp.c
	$(CC) $(CFLAGS) -c caspp.c

cgi:
	(cd cgi-bin; make)

clean:
	rm -f *.o tiny *~
	(cd cgi-bin; make clean)