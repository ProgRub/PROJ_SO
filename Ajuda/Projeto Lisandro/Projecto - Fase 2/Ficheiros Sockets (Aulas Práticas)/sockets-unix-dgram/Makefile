#
#
#	Makefile for test programs
#
#
CC = gcc
CFLAGS = -g

LIBS = -lsocket -lns -lnsl
LIBS = -lsocket -lnsl

all:  unix-dgram-client unix-dgram-server

unix-dgram-client: unix-dgram-client.o util.o util-dgram-client.o
	$(CC) -g -o unix-dgram-client unix-dgram-client.o util.o util-dgram-client.o $(LIBS)
unix-dgram-server: unix-dgram-server.o util.o util-dgram-server.o
	$(CC) -g -o unix-dgram-server unix-dgram-server.o util.o util-dgram-server.o $(LIBS)

clean:
	rm -f *.o *~ unix-dgram-client unix-dgram-server


