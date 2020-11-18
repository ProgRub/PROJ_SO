#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "unix.h"

int sockfd;
int newsockfd;

//Funcao Que Recebe Mensagem da Socket e Passa Cada Campo da Mensagem Para Uma Variavel Propria
int * Recebe_Mensagem(int sockfd)
{
	int Mensagem_Recebida=0;
	int ID_Cliente=0;
	int Tipo_Cliente=0;
	int Estado=0; 
	int Hora_Actual=0;
	
	int Tempo_Espera_Maximo_Filas=0;
	int Tempo_Estadia_Feira=0; 
	
	int Hora_Entrada_Fila_Bar=0; 
	int Hora_Desistencia_Fila_Bar=0;
	int Hora_Atendimento_Bar=0;
	
	int Hora_Entrada_Fila_MontanhaRussaPequena=0;
	int Hora_Desistencia_Fila_MontanhaRussaPequena=0;
	int Hora_Entrada_MontanhaRussaPequena=0;
	int Hora_Andou_MontanhaRussaPequena=0;
	
	int Hora_Entrada_Fila_MontanhaRussaGrande=0;
	int Hora_Desistencia_Fila_MontanhaRussaGrande=0;
	int Hora_Andou_MontanhaRussaGrande=0;
	
	int Hora_Entrada_Filas_Bilheteiras=0;
	int Hora_Desistencia_Filas_Bilheteiras=0;
	
	int Hora_Entrada_Feira=0;
	int Hora_Saida_Feira=0;

	char recvline[MAX_LINE+1];
	static int Mensagem[19]; //Conversao dos Valores da Mensagem Para Array a Ser Enviado Para Interpretacao


	Mensagem_Recebida = read(sockfd, recvline, MAX_LINE);

	if (Mensagem_Recebida < 0) perror("Erro ao ler mensagem no servidor\n");
	recvline[Mensagem_Recebida] = 0;
	
	sscanf(recvline,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &ID_Cliente, &Tipo_Cliente, &Estado, &Hora_Actual, &Tempo_Espera_Maximo_Filas, &Tempo_Estadia_Feira, &Hora_Entrada_Fila_Bar, &Hora_Desistencia_Fila_Bar, &Hora_Atendimento_Bar, &Hora_Entrada_Fila_MontanhaRussaPequena, &Hora_Desistencia_Fila_MontanhaRussaPequena, &Hora_Entrada_MontanhaRussaPequena, &Hora_Entrada_Fila_MontanhaRussaGrande, &Hora_Desistencia_Fila_MontanhaRussaGrande, &Hora_Andou_MontanhaRussaGrande, &Hora_Entrada_Filas_Bilheteiras, &Hora_Desistencia_Filas_Bilheteiras, &Hora_Entrada_Feira,&Hora_Saida_Feira);

	Mensagem[0] = ID_Cliente;
	Mensagem[1] = Tipo_Cliente;
	Mensagem[2] = Estado;
	Mensagem[3] = Hora_Actual;
	Mensagem[4] = Tempo_Espera_Maximo_Filas;
	Mensagem[5] = Tempo_Estadia_Feira;
	Mensagem[6] = Hora_Entrada_Fila_Bar;
	Mensagem[7] = Hora_Desistencia_Fila_Bar;
	Mensagem[8] = Hora_Atendimento_Bar;
	Mensagem[9] = Hora_Entrada_Fila_MontanhaRussaPequena;
	Mensagem[10] = Hora_Desistencia_Fila_MontanhaRussaPequena;
	Mensagem[11] = Hora_Entrada_MontanhaRussaPequena;
	Mensagem[12] = Hora_Entrada_Fila_MontanhaRussaGrande;
	Mensagem[13] = Hora_Desistencia_Fila_MontanhaRussaGrande;
	Mensagem[14] = Hora_Andou_MontanhaRussaGrande;
	Mensagem[15] = Hora_Entrada_Filas_Bilheteiras;
	Mensagem[16] = Hora_Desistencia_Filas_Bilheteiras;
	Mensagem[17] = Hora_Entrada_Feira;
	Mensagem[18] = Hora_Saida_Feira;

	
	return Mensagem;
}
