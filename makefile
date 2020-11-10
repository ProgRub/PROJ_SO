all: simulador monitor

simulador: simulador.c simulador.h
		gcc -c -g simulador.c && gcc -g -o simulador simulador.o -lpthread

monitor: monitor.c monitor.h
		cc -c -g monitor.c && gcc -g -o monitor monitor.o -lpthread

# simulador.o: simulador.c simulador.h
# 		gcc -c -g simulador.c

# monitor.o: monitor.c monitor.h
# 		gcc -c -g monitor.c

# ambos.o: simulador.c simulador.h monitor.c monitor.h
# 		gcc -c -g simulador.c && gcc -c -g monitor.c

clean:
		rm *.o simulador monitor