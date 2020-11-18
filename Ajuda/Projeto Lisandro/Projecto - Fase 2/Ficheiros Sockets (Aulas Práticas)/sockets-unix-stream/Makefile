#
#
#	Makefile for test programs
#
#
CC = gcc
CFLAGS = -g

LIBS = -lsocket -lns -lnsl
LIBS = -lsocket -lnsl

all: unix-stream-client unix-stream-server

unix-stream-client: unix-stream-client.o util.o util-stream-client.o
	$(CC) -g -o unix-stream-client unix-stream-client.o util.o util-stream-client.o $(LIBS)

unix-stream-server: unix-stream-server.o util.o util-stream-server.o
	$(CC) -g -o unix-stream-server unix-stream-server.o util.o util-stream-server.o $(LIBS)

clean:
	rm -f *.o *~ unix-stream-client unix-stream-server


