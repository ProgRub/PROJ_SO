simulador: simulador.o
		gcc -g -o simulador simulador.o -lpthread

monitor: monitor.o
		gcc -g -o monitor monitor.o -lpthread

simulador.o: simulador.c simulador.h
		gcc -c -g simulador.c

monitor.o: monitor.c monitor.h
		gcc -c -g monitor.c

clean:
		rm *.o simulador monitor