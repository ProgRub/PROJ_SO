#include <stdio.h>          //Biblioteca geral de input e outputs da linguagem C
#include <stdlib.h>         //Biblioteca padrão do C
#include <sys/types.h>      //Contem definicoes de data types usados nos system calls. E necessario para os dois procimos imports
#include <sys/socket.h>     //Inclui definicoes necessarias para trabalhar com sockets
#include <sys/un.h>         //contem estruturas e constantes necessarias para o dominio do endereco
#include <time.h>            //biblioteca para trabalhar com  o tempo
#include <stdbool.h>        //Para trabalhar com boleanos
#include <pthread.h>
#include <semaphore.h>
#include <ctype.h>


#define UNIXSTR_PATH "/tmp/s.2035417"
#define TamLinha 1024       //tamanho maximo do buffer


struct pessoa{
    int estacao;
    int destino; 
    bool temBilhete;
    bool VIP;
    bool temQueMudarDeEstacao;
    bool estaComboio;
    bool desistiu;
};

struct metro{
    int id;
    bool direcao; //1 esta a ir para baixo, 0 esta a ir para cima
    int numeroPassageirosAtual;
    int trajeto [3];
    int pessoasEsperaSaida[3];
    int idEstacaoAtual;
    int idEstacaoSeguinte;
};

struct bilheteira{                                                                                                             
    int pessoasVIPFila;
    int pessoasNormalFila;
    int estacao; 
    float tempoMedioEspera;
    float tempoMedioCompra;
    int pessoasDesistentesVIP;
    int pessoasDesistentesNormal;
    int pessoasAtendidasVIP;
    int pessoasAtendidasNormal;
    float somaTempoEspera;
    int quantidadeTemposEspera;
    float somaTempoCompra;
    int quantidadeTemposCompra;
};

struct estacao{
    int id;
    int numeroPassageirosEspera;
    int numeroPssageirosSairamComboio;
    int numeroPssageirosSairamEstacao;
    int numeroPassageirosEntraramComboio;
    int numeroPassageirosEntraramEstacao;
    int numeroPassageirosDesistiram;
    int numeroPassageiros;
};