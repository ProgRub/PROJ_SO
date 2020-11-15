all: simulador monitor

simulador: simulador.o
	gcc -o simulador simulador.o -lpthread -lrt
monitor: monitor.o
	gcc -o monitor monitor.o -lpthread -lrt

clean:
	rm -f *.o *~ simulador monitor


