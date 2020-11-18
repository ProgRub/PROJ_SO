#
#
#	Makefile for test programs
#
#
CC = gcc
CFLAGS = -g

LIBS = -lsocket -lns -lnsl
LIBS = -lsocket -lnsl

all:  tcp-client tcp-server

tcp-client: tcp-client.o util.o util-stream-client.o inet.h
	$(CC) -g -o tcp-client tcp-client.o util.o util-stream-client.o $(LIBS)

tcp-server: tcp-server.o util.o util-stream-server.o inet.h
	$(CC) -g -o tcp-server tcp-server.o util.o util-stream-server.o $(LIBS)

clean:
	rm -f *.o *~ tcp-client tcp-server

