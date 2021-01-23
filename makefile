all: simulador monitor

cleanAndBuild: clean simulador monitor

simulador: simulador.c config.h
		gcc -c -g simulador.c && gcc -g -o simulador simulador.o -lpthread

monitor: monitor.c config.h
		gcc -c -g monitor.c && gcc -g -o monitor monitor.o -lpthread

clean:
		rm *.o simulador monitor