#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

#define UNIXSTR_PATH "/tmp/s.2045218"
#define TAMANHO_LINHA 1024 //tamanho maximo do buffer
#define TRUE 1
#define FALSE 0
#define NAO_TESTOU 0
#define POSITIVO 1
#define NEGATIVO 2
#define INCONCLUSIVO 3
#define ESPERA 0
#define ISOLAMENTO 1
#define HOSPITAL 2
#define TAMANHO_CONFIGURACAO 19
#define CENTRO_PRIORITARIO 1

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
    int numeroPessoas;
    float probabilidadeSerIdoso;
    float probabilidadeMedicoPositivo;
    float probabilidadePopulacaoPositivo;
    float probabilidadeTesteNormalInconclusivo;
    float probabilidadeTesteRapidoInconclusivo;
    float probabilidadeNaoIdosoPrecisaHospital;
    float probabilidadeIdosoMorrer;
    float probabilidadeNaoIdosoMorrer;
    int tempoCurar;
    int tempoSimulacao;
};

struct pessoa
{
    int id;
    int medico; // 0-> nao é 1-> é medico
    int centroTeste;
    int estadoTeste;     //0 -> nao fez teste 1 -> teste positivo 2 -> negativo 3 ->teste incunclusivo
    int idoso;           //indica se a pessoa e idosa, e assim tem prioridade 0 -> nao é 1-> idoso
    int precisaHospital; //indica se a pessoa precisa de ir para o hospital, idosos precisam automaticamente
    int tempoMaximoEspera;
    int estado; //0 - a espera, 1 - isolamento, 2 - hospital
};

struct centro 
{
    int id;
    int numeroPessoas;
};

//METODOS DEFINIDOS
//simulador.c
int probabilidade(float prob);
void simulacao(char *filename);
void carregarConfiguracao(char nomeFicheiro[]);
void iniciarSemaforosETrincos();
int criaSocket();
struct pessoa criaPessoa();
struct pessoa criaMedico();
void enviarMensagem(char *mensagemAEnviar);
void Medico(void * ptr);
void Pessoa(void * ptr);

//monitor.c
void escreveEmFicheiroEMonitor(char *mensagem);
void criaServidor();
void trataMensagem(char mensagem[]);
void leituraSocket(int sockfd);
void imprimirInformacao();
void limparFicheiro();