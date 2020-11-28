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
#define TAMANHO_LINHA 1024       //tamanho maximo do buffer
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
    int numeroMedicos;
    float probabilidadePositivo;
    float probabilidadeTesteNormalFalsoPositivo;
    float probabilidadeTesteRapidoFalsoPositivo;
    float probabilidadeNaoIdosoPrecisaHospital;
    int tempoCurar;
    int tempoSimulacao;
};

struct pessoa{
    int centroTeste;
    int testouPositivo;
    int falsoPositivo;
    int idoso; //indica se a pessoa e idosa, e assim tem prioridade
    int precisaHospital; //indica se a pessoa precisa de ir para o hospital, idosos precisam automaticamente
    int tempoMaximoEspera;
    int estado; //0 - a espera, 1 - isolamento, 2 - hospital
};


//METODOS DEFINIDOS
//simulador.c
void simulacao(char * filename);
void carregarConfiguracao(char nomeFicheiro[]);
void iniciarSemaforosETrincos();
int criaSocket();
void criaPessoa();
void enviarMensagem( char *mensagemAEnviar);

//monitor.c
void escreveEmFicheiro(char *mensagem);
void criaServidor();
void trataMensagem(char mensagem[]);
void leituraSocket(int sockfd);