#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "unix.h"

Clientes cliente[10000];
pthread_t Thread_Clientes[10000];

//Iniciar a Variavel Que Guarda o Tempo de Simulacao Actual no Inicio do Ficheiro a Um Valor Que Convertido Pela Funcao das Horas Seja <=> a 08:00h
int Tempo_Simulacao_Actual=0; //600=Quanto Em Horas?


//Declaracao e Inicializacao a 0 das Variaveis que vao Guardar os Valores das Variaveis Lidas do Ficheiro de Configuracao:
int Tempo_Medio_Chegadas=0;
int Tempo_Simulacao=0;
int Probabilidade_Desistencia=0;

int Tamanho_Maximo_Bar=0;
int Tamanho_Maximo_MontanhaRussaGrande=0;
int Tamanho_Maximo_MontanhaRussaPequena=0;
int Tamanho_Maximo_Feira=0;

int Tempo_Maximo_Espera_Filas=0;
int Percentagem_Ser_Cliente_Crianca=0;
int Percentagem_Ser_Cliente_Idoso=0;

//Funcao Que Recebe Array Com Parametros Lidos do Ficheiro Config e Atribui Valores as Variaveis
void Recebe_Array_Parametros_Config(int Array[10])
{
	Tempo_Medio_Chegadas=Array[0];
	Tempo_Simulacao=Array[1];
	Probabilidade_Desistencia=Array[2];
	Tamanho_Maximo_Bar = Array[3];
	Tamanho_Maximo_MontanhaRussaGrande=Array[4];
	Tamanho_Maximo_MontanhaRussaPequena=Array[5];
	Tamanho_Maximo_Feira=Array[6];
	Tempo_Maximo_Espera_Filas=Array[7];
	Percentagem_Ser_Cliente_Crianca=Array[8];
	Percentagem_Ser_Cliente_Idoso=Array[9];
}

int sockfd;

//Guarda Quantas Threads Foram Criadas 
int Numero_Threads_Clientes_Criadas=0;
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------

//Write throught the socket
void Escreve_Mensagem(int sockfd, int ID_Cliente, int Tipo_Cliente, int Estado, int Hora_Actual, int Tempo_Espera_Maximo_Filas, int Tempo_Estadia_Feira, int Hora_Entrada_Fila_Bar, int Hora_Desistencia_Fila_Bar, int Hora_Atendimento_Bar, int Hora_Entrada_Fila_MontanhaRussaPequena, int Hora_Desistencia_Fila_MontanhaRussaPequena, int Hora_Entrada_MontanhaRussaPequena, int Hora_Entrada_Fila_MontanhaRussaGrande, int Hora_Desistencia_Fila_MontanhaRussaGrande, int Hora_Andou_MontanhaRussaGrande, int Hora_Entrada_Filas_Bilheteiras, int Hora_Desistencia_Filas_Bilheteiras, int Hora_Entrada_Feira, int Hora_Saida_Feira)
{

	char Buffer[MAX_LINE];
	int Tamanho_Mensagem;

	sprintf(Buffer,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", ID_Cliente, Tipo_Cliente, Estado,Hora_Actual,Tempo_Espera_Maximo_Filas,Tempo_Estadia_Feira,Hora_Entrada_Fila_Bar, Hora_Desistencia_Fila_Bar, Hora_Atendimento_Bar, Hora_Entrada_Fila_MontanhaRussaPequena, Hora_Desistencia_Fila_MontanhaRussaPequena, Hora_Entrada_MontanhaRussaPequena, Hora_Entrada_Fila_MontanhaRussaGrande, Hora_Desistencia_Fila_MontanhaRussaGrande, Hora_Andou_MontanhaRussaGrande, Hora_Entrada_Filas_Bilheteiras, Hora_Desistencia_Filas_Bilheteiras, Hora_Entrada_Feira,Hora_Saida_Feira); // Writes in simulator socket buffer a message to monitor

	Tamanho_Mensagem=strlen(Buffer)+1; // Add one character more because of \n

	if(write(sockfd,Buffer,Tamanho_Mensagem)!=Tamanho_Mensagem)
		perror("Erro ao escrever no cliente\n");
}
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------

//Funcao Para Ler do Ficheiro Config.conf:
int* Ler_Ficheiro_Config()
{
	//Declaracao e Inicializacao de Variaveis Auxiliares da Funcao
	char Le_Linha[100];
	int Valor_Variavel_Lida;
	char Nome_Variavel_Lida[100];

	char Var1[]="Tempo_Medio_Chegadas";
	char Var2[]="Tempo_Simulacao";
	char Var3[]="Probabilidade_Desistencia";
	char Var4[]="Tamanho_Maximo_Bar";
	char Var5[]="Tamanho_Maximo_MontanhaRussaPequena";
	char Var6[]="Tamanho_Maximo_MontanhaRussaGrande";
	char Var7[]="Tamanho_Maximo_Feira";
	char Var8[]="Tempo_Maximo_Espera_Filas";
	char Var9[]="Percentagem_Ser_Cliente_Crianca";
	char Var10[]="Percentagem_Ser_Cliente_Idoso";


	static int Parametros_Lidos[10]; //Array que Vai Guardar Valores dos Parametros Lidos No Ficheiro de Configuracao
	//------------------------------------------------------------------------------------------------------------------

	FILE *fp;
	fp=fopen("config.conf","r"); //Vai Abrir o Ficheiro Config.conf
	if(fp!=NULL)
	{
		//Ciclo de Leitura:
		while(fgets(Le_Linha,100,fp)!=NULL)
		{
			/* %s= Nome da Variavel | %i= Valor da Variavel */
			sscanf(Le_Linha, "%s=%i", Nome_Variavel_Lida, &Valor_Variavel_Lida);
			
			/* Strcmp Tem Valor 0  se [Elemento1]=[Elemento2] */
			if(strcmp(Nome_Variavel_Lida,Var1)==0)
			{
				Parametros_Lidos[0]=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var2)==0)
			{
				Parametros_Lidos[1]=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var3)==0)
			{
				Parametros_Lidos[2]=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var4)==0)
			{
				Parametros_Lidos[3]=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var5)==0)
			{
				Parametros_Lidos[4]=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var6)==0)
			{
				Parametros_Lidos[5]=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var7)==0)
			{
				Parametros_Lidos[6]=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var8)==0)
			{
				Parametros_Lidos[7]=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var9)==0)
			{
				Parametros_Lidos[8]=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var9)==0)
			{
				Parametros_Lidos[9]=Valor_Variavel_Lida;
			}
		}
		fclose(fp);
		return Parametros_Lidos;
	}

	else
	{
		printf("Erro Abertura Ficheiro");
		//abort();
		return 0;
	}

}





