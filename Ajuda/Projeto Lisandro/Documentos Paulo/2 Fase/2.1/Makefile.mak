all: simulador monitor
simulador: simulador.o
	gcc -g simulador.o -o simulador -lpthread
simulador.o: simulador.c unix.h
	gcc -g -c simulador.c
monitor: monitor.o
	gcc -g monitor.o -o monitor
monitor.o: monitor.c unix.h
	gcc -g -c monitor.c
clean: rm *.o
