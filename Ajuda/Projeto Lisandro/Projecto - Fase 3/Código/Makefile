
all: monitor simulador

monitor: monitor.o
	gcc monitor.c -lpthread -o Monitor

simulador: simulador.o
	gcc simulador.c -lpthread -o Simulador
	./Simulador
clean:
	rm *.o Monitor
	rm *.o Simulador
