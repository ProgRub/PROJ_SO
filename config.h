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

#define TESTE_NORMAL 0
#define TESTE_RAPIDO 1

//CORES

#define RESET  "\x1B[0m"
#define VERMELHO  "\x1B[31m"
#define VERDE  "\x1B[32m"
#define AMARELO  "\x1B[33m"
#define AZUL  "\x1B[34m"
#define MAGENTA  "\x1B[35m"
#define CIANO  "\x1B[36m"
#define BRANCO  "\x1B[37m"

//ESTADO TESTE
#define NAO_TESTOU 0
#define POSITIVO 1
#define NEGATIVO 2
#define INCONCLUSIVO 3

//ESTADO PESSOA
#define ESPERA 0
#define ISOLAMENTO 1
#define HOSPITAL 2
#define DESISTIU 3

//AUXILIAR
#define TAMANHO_CONFIGURACAO 20
#define TAMANHO_ARRAY_TAREFAS 100000
#define MINUTO 42//em milisegundos
#define HORA 42*60 //em milisegundos
#define DIA 42*60*24 //em milisegundos

struct Configuration
{
    int tempoMedioChegada;
    int tempoTesteNormal;
    int tempoTesteRapido;
    int tempoCooldownPontosTestagem;
    int tempoMaximoEsperaMedio;
    int tamanhoFilaCentro1;
    int tamanhoFilaCentro2;
    int numeroPontosTestagemCentro1;
    int numeroPontosTestagemCentro2;
    int tamanhoHospital;
    int numeroMedicos;
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
    int tipoTeste;
    int estadoTeste;     //0 -> nao fez teste 1 -> teste positivo 2 -> negativo 3 ->teste inconclusivo
    int idoso;           //indica se a pessoa e idosa, e assim tem prioridade 0 -> nao é 1-> idoso
    int precisaHospital; //indica se a pessoa precisa de ir para o hospital, idosos precisam automaticamente
    int tempoMaximoEspera;
    int numeroPessoasAFrenteParaDesistir;
    int tempoChegadaFilaEspera;
    int desistiu;
    int estado; //0 - a espera, 1 - isolamento, 2 - hospital
    sem_t semaforoPessoa;
    int numeroDiasDesdePositivo;
};

struct centroTeste1
{
    int numeroPessoasEspera;
    sem_t filaEspera;
};

struct centroTeste2
{
    int numeroPessoasNormalEspera;
    int numeroPessoasPrioritariasEspera;
    sem_t filaEsperaPrioritaria;
    sem_t filaEsperaNormal;
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
void FilaEspera(struct pessoa * pessoa);

//monitor.c
void escreveEmFicheiroEMonitor(char *mensagem);
void criaServidor();
void trataMensagem(char mensagem[]);
void leituraSocket(int sockfd);
void imprimirInformacao();
void limparFicheiro();