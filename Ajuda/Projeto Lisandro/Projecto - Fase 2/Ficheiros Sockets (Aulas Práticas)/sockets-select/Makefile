#
#
#	Makefile for Unix sockets test programs
#
#
CC = gcc
CFLAGS = -g

LIBS = -lsocket -lns -lnsl
LIBS = -lsocket -lnsl

all: multiserver client-stream client-dgram

multiserver : multiserver.o 
	$(CC) -g -o multiserver multiserver.o $(LIBS)

client-stream : client-stream.o
	$(CC) -g -o client-stream client-stream.o $(LIBS)

client-dgram : client-dgram.o
	$(CC) -g -o client-dgram client-dgram.o $(LIBS)

clean:
	rm -f *.o *~ multiserver client-stream client-dgram
 

