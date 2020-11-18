#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>

#define ADDRESS      "champ_socket"

#define UNIXSTR_PATH "/tmp/socket.unix.stream_"
#define UNIXDG_PATH  "/tmp/socket.unix.datagram"
#define UNIXDG_TMP   "/tmp/socket.unix.dgXXXXXXX"


#define MAX_LINE 512

//---------------------------------------------------------------------------------------------------------
//--------------------------------------------- Structs ------------------------------------------------
//---------------------------------------------------------------------------------------------------------

//Estrutura Clientes:
typedef struct
{
int ID;
int Tipo_Cliente;
int Estado_Seguinte;
int Tempo_Espera_Maximo_Filas;
int Tempo_Estadia_Feira;
int Hora_Entrada_Fila_Bar;
int Hora_Desistencia_Fila_Bar;
int Hora_Atendimento_Bar;
int Hora_Entrada_Fila_Carroussel;
int Hora_Desistencia_Fila_Carroussel;
int Hora_Entrada_Carroussel;
int Hora_Entrada_Fila_MontanhaRussaG; //montanha russa grande
int Hora_Desistencia_Fila_MontanhaRussaG;
int Hora_Andou_MontanhaRussaG;
int Hora_Entrada_Fila_MontanhaRussaP; //montanha russa pequena
int Hora_Desistencia_Fila_MontanhaRussaP;
int Hora_Andou_MontanhaRussaP;
int Hora_Entrada_Filas_Bilheteiras;
int Hora_Desistencia_Filas_Bilheteiras;
int Hora_Entrada_Feira;


int Hora_Saida_Feira;

int Lugar_Fila_Bilheteiras;
int Lugar_Fila_Bar;
int Lugar_Fila_Carroussel;
int Lugar_Fila_MontanhaRussaG;
int Lugar_Fila_MontanhaRussaP;

} Clientes;


//Estrutura Zonas (Bar, Feira, Carroussel, Montanhas russas)
typedef struct
{
int ID;
int Tipo_Zona;
int Tamanho_Maximo;

} Zona;

//Variavel Global Activada Quando Utilizador Escolhe Hipotese Simulacao. Comeca a Criar Threads de Clientes
int Variavel_Global_Comeca_Simulacao; //=0;

//Variavel Global Activada Quando Utilizador Escolhe Hipotese de Sair da Aplicacao
int Variavel_Global_Sai_Aplicacao; //=0;


//Funcoes Auxiliares Partilhadas:

char *Converte_Em_Numero_De_Tres_Digitos(int);

char * Converte_Tempo_Para_Horas(int);