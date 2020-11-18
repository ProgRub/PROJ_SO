#
#
#	Makefile for test programs
#
#
CC = gcc
CFLAGS = -g

LIBS = -lsocket -lns -lnsl
LIBS = -lsocket -lnsl

all:  udp-client udp-server

udp-client: udp-client.o util.o util-dgram-client.o inet.h
	$(CC) -g -o udp-client udp-client.o util.o util-dgram-client.o $(LIBS)

udp-server: udp-server.o util.o util-dgram-server.o inet.h
	$(CC) -g -o udp-server udp-server.o util.o util-dgram-server.o $(LIBS)

clean:
	rm -f *.o *~ udp-client udp-server

