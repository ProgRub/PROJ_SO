#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define UNIXSTR_PATH "/tmp/s.2045218"
#define TamLinha 1024       //tamanho maximo do buffer
#define TRUE 1
#define FALSE 0

struct Configuration
{
    int tempoMedioChegada;
    int tempoTesteNormal;
    int tempoTesteRapido;
    int tempoEsperaCentro1;
    int tempoEsperaCentro2;
    int tamanhoFilaCentro1;
    int tamanhoFilaCentro2;
    int tamanhoHospital;
    float probabilidadePositivo;
    float probabilidadeTesteNormalFalsoPositivo;
    float probabilidadeTesteRapidoFalsoPositivo;
    int tempoSimulacao;
};

struct pessoa{

};

struct centroTeste{
};

struct hospital{
};

struct isolamento{
};