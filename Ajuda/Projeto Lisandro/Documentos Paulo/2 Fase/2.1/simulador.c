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
int Tamanho_Maximo_Carroussel=0;
int Tamanho_Maximo_MontanhaRussaG=0;
int Tamanho_Maximo_MontanhaRussaP=0;
int Tamanho_Maximo_Feira=0;

int Tempo_Maximo_Espera_Filas=0;
int Percentagem_Ser_Cliente_Crianca=0; //quando é menor que 6 anos
int Percentagem_Ser_Cliente_Mobilidade_Reduzida=0;

//Funcao Que Recebe Array Com Parametros Lidos do Ficheiro Config e Atribui Valores as Variaveis
void Recebe_Array_Parametros_Config(int Array[11])
{
	Tempo_Medio_Chegadas=Array[0];
	Tempo_Simulacao=Array[1];
	Probabilidade_Desistencia=Array[2];
	Tamanho_Maximo_Bar=Array[3];
	Tamanho_Maximo_Carroussel=Array[4];
	Tamanho_Maximo_MontanhaRussaG=Array[5];
	Tamanho_Maximo_MontanhaRussaP=Array[6];
	Tamanho_Maximo_Feira=Array[7];
	Tempo_Maximo_Espera_Filas=Array[8];
	Percentagem_Ser_Cliente_Crianca=Array[9];
	Percentagem_Ser_Cliente_Mobilidade_Reduzida=Array[10];
}

int sockfd;

//Guarda Quantas Threads Foram Criadas (Para Fazer Ciclo For Por Causa do Join)
int Numero_Threads_Clientes_Criadas=0;
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------

//Funcao Que Escreve Mensagem Para Enviar Pela Socket 
void Escreve_Mensagem(int sockfd, int ID_Cliente, int Tipo_Cliente, int Estado, int Hora_Actual, int Tempo_Espera_Maximo_Filas, int Tempo_Estadia_Feira, int Hora_Entrada_Fila_Bar, int Hora_Desistencia_Fila_Bar, int Hora_Atendimento_Bar, int Hora_Entrada_Fila_Carroussel, int Hora_Desistencia_Fila_Carroussel, int Hora_Entrada_Carroussel, int Hora_Entrada_Fila_MontanhaRussaG, int Hora_Desistencia_Fila_MontanhaRussaG, int Hora_Andou_MontanhaRussaG, int Hora_Entrada_Fila_MontanhaRussaP, int Hora_Desistencia_Fila_MontanhaRussaP, int Hora_Andou_MontanhaRussaP, int Hora_Entrada_Filas_Bilheteiras, int Hora_Desistencia_Filas_Bilheteiras, int Hora_Entrada_Feira, int Hora_Saida_Feira)
{

	char Buffer[MAX_LINE];
	int Tamanho_Mensagem;

	sprintf(Buffer,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", ID_Cliente, Tipo_Cliente, Estado,Hora_Actual,Tempo_Espera_Maximo_Filas,Tempo_Estadia_Feira,Hora_Entrada_Fila_Bar, Hora_Desistencia_Fila_Bar, Hora_Atendimento_Bar, Hora_Entrada_Fila_Carroussel, Hora_Desistencia_Fila_Carroussel, Hora_Entrada_Carroussel, Hora_Entrada_Fila_MontanhaRussaG, Hora_Desistencia_Fila_MontanhaRussaG, Hora_Andou_MontanhaRussaG, Hora_Entrada_Fila_MontanhaRussaP, Hora_Desistencia_Fila_MontanhaRussaP, Hora_Andou_MontanhaRussaP, Hora_Entrada_Filas_Bilheteiras, Hora_Desistencia_Filas_Bilheteiras, Hora_Entrada_Feira,Hora_Saida_Feira); // Writes in simulator socket buffer a message to monitor

	Tamanho_Mensagem=strlen(Buffer)+1; // Add one character more because of \n

	if(write(sockfd,Buffer,Tamanho_Mensagem)!=Tamanho_Mensagem)
	{
		perror("Erro ao escrever no pixota cliente\n");
	}
	else
	{
			printf("Connected\n");
	}
}
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------

//Funcao Para Ler do Ficheiro Config:
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
	char Var5[]="Tamanho_Maximo_Carroussel";
	char Var6[]="Tamanho_Maximo_MontanhaRussaG";
	char Var7[]="Tamanho_Maximo_MontanhaRussaP";
	char Var8[]="Tamanho_Maximo_Feira";
	char Var9[]="Tempo_Maximo_Espera_Filas";
	char Var10[]="Percentagem_Ser_Cliente_Crianca";
	char Var11[]="Percentagem_Ser_Cliente_Mobilidade_Reduzida";
	
	
	static int Parametros_Lidos[11]; //Array que Vai Guardar Valores dos Parametros Lidos No Ficheiro de Configuracao
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
			else if(strcmp(Nome_Variavel_Lida,Var10)==0)
			{
				Parametros_Lidos[9]=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var11)==0)
			{
				Parametros_Lidos[10]=Valor_Variavel_Lida;
			}
		}
		fclose(fp);
		return Parametros_Lidos;
	}

	else
	{
		printf("Erro na Abertura do Ficheiro");
		//abort();
		return 0;
	}
}

//Funcao Que Define Tipo dos Clientes Baseado nas Percentagens Lidas do Ficheiro Config
int Define_Tipo_Cliente_Random(int Percentagem_Ser_Cliente_Crianca, int Percentagem_Ser_Cliente_Mobilidade_Reduzida)
{
	int Tipo_Cliente=0;

	int Random=rand() %100 + 1; //Cria Numero Random de 1 a 100
	/*
	Exemplo Funcionamento:
	Prob Cri=5%->1,2,3,4,5
	Prob MR=10%->6,7,8,9,10,11,12,13,14,15

	Random<=Prob Cri  Ou seja, Random<=5
	((Random>Prob Cri) && (Random<=(Prob Cri+Prob MR))) Ou Seja, Random>=6 e <=15

	*/

	//Ser Crianca
	if(Random<=Percentagem_Ser_Cliente_Crianca) 
	{
		Tipo_Cliente=2;
	}
	//Ser Mobilidade Reduzida
	else if((Random>Percentagem_Ser_Cliente_Crianca) && (Random<=(Percentagem_Ser_Cliente_Crianca+Percentagem_Ser_Cliente_Mobilidade_Reduzida)))
	{
		Tipo_Cliente=1;
	}
	else //Publico Geral
	{
		Tipo_Cliente=3;
	}

	return Tipo_Cliente;
}

//Funcao Que Define Tempo de Estadia do Cliente na Feira
int Define_Tempo_Estadia_Cliente_Na_Feira()
{
	int Tempo_Estadia=rand() %500 + 300; 

	return Tempo_Estadia;
}

void Escolhe_Evento_Cliente(int ID_Cliente)
{
//Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1),... ->Em Vez de cliente[ID_Cliente].ID+1 Pode Ficar Simplesmente ID_Cliente+1?

//Iniciar As Variaveis da Estrutura dos Clientes a 0


cliente[ID_Cliente].ID=ID_Cliente+1;
cliente[ID_Cliente].Tipo_Cliente=Define_Tipo_Cliente_Random(Percentagem_Ser_Cliente_Crianca, Percentagem_Ser_Cliente_Mobilidade_Reduzida);
cliente[ID_Cliente].Tempo_Espera_Maximo_Filas=Tempo_Maximo_Espera_Filas;
cliente[ID_Cliente].Tempo_Estadia_Feira=Define_Tempo_Estadia_Cliente_Na_Feira();
cliente[ID_Cliente].Hora_Entrada_Fila_Bar=0;
cliente[ID_Cliente].Hora_Desistencia_Fila_Bar=0;
cliente[ID_Cliente].Hora_Atendimento_Bar=0;
cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel=0;
cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel=0;
cliente[ID_Cliente].Hora_Entrada_Carroussel=0;
cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG=0;
cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG=0;
cliente[ID_Cliente].Hora_Andou_MontanhaRussaG=0;
cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP=0;
cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP=0;
cliente[ID_Cliente].Hora_Andou_MontanhaRussaP=0;
cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras=0;
cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras=0;
cliente[ID_Cliente].Hora_Entrada_Feira=0;
cliente[ID_Cliente].Lugar_Fila_Bilheteiras=0;
cliente[ID_Cliente].Lugar_Fila_Bar=0;
cliente[ID_Cliente].Lugar_Fila_Carroussel=0;
cliente[ID_Cliente].Lugar_Fila_MontanhaRussaG=0;
cliente[ID_Cliente].Lugar_Fila_MontanhaRussaP=0;
cliente[ID_Cliente].Hora_Saida_Feira=0;


/*
Estrutura:
Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 1, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira)
*/


//case 1: //Entra Fila Bilheteiras do Feira

cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 1, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);


sleep(20);

//case 2: //Sai Fila Bilheteiras do Feira e Entra no Feira

cliente[ID_Cliente].Hora_Entrada_Feira=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 2, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

sleep(20);

//case 3: //Desiste Fila Bilheteiras da Feira

cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 3, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

sleep(20);

//case 4: //Entra Fila Bar

cliente[ID_Cliente].Hora_Entrada_Fila_Bar=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 4, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

sleep(20);

//case 5: //Sai Fila Bar, é Atendido e Sai do Bar

cliente[ID_Cliente].Hora_Atendimento_Bar=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 5, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

sleep(20);

//case 6: //Fica no Bar//Sai Fila Bar, é Atendido e Fica no Bar

cliente[ID_Cliente].Hora_Atendimento_Bar=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 6, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

sleep(20);

//case 7: //Desiste Fila Bar

cliente[ID_Cliente].Hora_Desistencia_Fila_Bar=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 7, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

sleep(20);

//case 8: //Sai Bar

//Como Nao Incluimos Horas de Saidas Das Zonas, Nao Se Poe Nada Aqui

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 8, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

sleep(20);

//case 9: //Entra Fila Carroussel

cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 9, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

sleep(20);

//case 10: //Sai Fila Carroussel e Entra na Carroussel

cliente[ID_Cliente].Hora_Entrada_Carroussel=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 10, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

sleep(20);

//case 11: //Desiste Fila Carroussel

cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 11, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

sleep(20);

//case 12: //Entra Fila MontanhaRussaG

cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 12, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

sleep(20);

//case 13: //Sai Fila MontanhaRussaG e Anda no MontanhaRussaG

cliente[ID_Cliente].Hora_Andou_MontanhaRussaG=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 13, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

sleep(20);

//case 14: //Desiste Fila MontanhaRussaG

cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 14, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

sleep(20);

//case 15: //Entra Fila MontanhaRussaP

cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 15, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

sleep(20);

//case 16: //Sai Fila MontanhaRussaP e Anda no MontanhaRussaP

cliente[ID_Cliente].Hora_Andou_MontanhaRussaP=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 16, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

sleep(20);

//case 17: //Desiste Fila MontanhaRussaP

cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, (cliente[ID_Cliente].ID+1), cliente[ID_Cliente].Tipo_Cliente, 17, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

sleep(20);

//case 18: //Sai Feira

//Como Nao Incluimos Horas de Saidas Das Zonas, Nao Se Poe Nada Aqui. No Entanto Pode Se Incluir Estatistica "Tempo Medio de Estadia na Feira" e Acrescentar Tempos Medios no Monitor.c Case 15

cliente[ID_Cliente].Hora_Saida_Feira=Tempo_Simulacao_Actual;

Escreve_Mensagem(sockfd, cliente[ID_Cliente].ID+1, cliente[ID_Cliente].Tipo_Cliente, 18, Tempo_Simulacao_Actual, cliente[ID_Cliente].Tempo_Espera_Maximo_Filas, cliente[ID_Cliente].Tempo_Estadia_Feira, cliente[ID_Cliente].Hora_Entrada_Fila_Bar, cliente[ID_Cliente].Hora_Desistencia_Fila_Bar, cliente[ID_Cliente].Hora_Atendimento_Bar, cliente[ID_Cliente].Hora_Entrada_Fila_Carroussel, cliente[ID_Cliente].Hora_Desistencia_Fila_Carroussel, cliente[ID_Cliente].Hora_Entrada_Carroussel, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaG, cliente[ID_Cliente].Hora_Andou_MontanhaRussaG, cliente[ID_Cliente].Hora_Entrada_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Desistencia_Fila_MontanhaRussaP, cliente[ID_Cliente].Hora_Andou_MontanhaRussaP, cliente[ID_Cliente].Hora_Entrada_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Desistencia_Filas_Bilheteiras, cliente[ID_Cliente].Hora_Entrada_Feira,cliente[ID_Cliente].Hora_Saida_Feira);

}

//Cria Threads de Clientes
int Cria_Threads_Clientes()
{
	int ID_Cliente;

	for (ID_Cliente = 0; Tempo_Simulacao_Actual<=Tempo_Simulacao; ID_Cliente++)
	{
		//if(pthread_create(&(Thread_Clientes[ID_Cliente]), NULL ,(void)&Escolhe_Evento_Cliente, ID_Cliente) != 0)
		if(pthread_create(&(Thread_Clientes[ID_Cliente]), NULL ,(void*)&Escolhe_Evento_Cliente, NULL) != 0)
		{
			printf("Erro na criacao da tarefa\n");
			exit(1);
		}
		sleep(rand() % Tempo_Medio_Chegadas);		

//colocamos <= Em Vez de < Para Por Um If Que Se o Tempo Simulacao Actual Iguala o Tempo de Simulacao, Manda Mensagem de Fim de Simulacao

		if(Tempo_Simulacao_Actual==Tempo_Simulacao)
		{
			//Envia Mensagem de Terminar Simulacao
			//Escreve_Mensagem(sockfd,0,0,50,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0); // Manda a mensagem de fim da simulação para o monitor
			Escreve_Mensagem(sockfd,0,0,50,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
		}

		Tempo_Simulacao_Actual=Tempo_Simulacao_Actual+1; //Incremento de Uma Unidade de Tempo da Simulacao
	}

	//Devolve Numero de Threads de Clientes Criadas
	return ID_Cliente;

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
	//Garante Que Quando Simulador é Iniciado, A Variavel Global de Simulacao Esta a 0 e Nao Com Outros Valores
	Variavel_Global_Comeca_Simulacao=0;

	//Ler_Ficheiro_Config(); //Funcao no Funcoes_Auxiliares_Simulador
	Recebe_Array_Parametros_Config(Ler_Ficheiro_Config());

	int sockfd, newsockfd, clilen, servlen,i;
	struct sockaddr_un cli_addr, serv_addr;

/* Cria socket stream */
	if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0)) < 0) perror("Erro ao criar socket stream servidor\n");
	/*Elimina preventivamente o ficheiro ou socket prexistente*/
	unlink(UNIXSTR_PATH);
	/*O nome serve para que os clientes possam identificar o servidor*/
	/*Preencher a estrutura com o nome do socket e dimensão ocupada após limpeza preventiva*/
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	
	strcpy(serv_addr.sun_path, UNIXSTR_PATH);
	
	servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0)	perror("Erro ao atribuir nome socket servidor\n");
	/* Servidor pronto a aceitar 1 clientes para o socket stream */
	listen(sockfd, 1);
	
	/*Dimensão inicial (alterável) da estrutura informa tipo de socket (UNIX/INET) à função */
	clilen = sizeof(cli_addr);
	/*Sincroniza com cliente ao aceitar pedido de ligação: criação dinâmica de canal privado*/
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) perror("Erro ao criar ligacao dedicada - accept\n");


	//Ciclo While Sempre Activo A Partir do Momento Que o Simulador é Iniciado.
	//Enquanto Variavel Global For 1 (Simular) Cria Clientes. Caso Contrario Espera Pela Escolha do Menu 1 (Simular) Pelo Utilizador
	while(Variavel_Global_Sai_Aplicacao!=1)
	{
		if(Variavel_Global_Comeca_Simulacao==1)
		{
			printf("Comecou a Criar Clientes \n");
			Numero_Threads_Clientes_Criadas=Cria_Threads_Clientes();

			Variavel_Global_Comeca_Simulacao=0; //Incluir Isto Senao Vai Estar Sempre a Invocar a Funcao Para Criar Clientes.
		}

	}

	//Ciclo For Que Vai Esperar Que Todas as Threads Acabem a Execucao
	for(i=0; i < Numero_Threads_Clientes_Criadas; i++)
	{
		pthread_join(Thread_Clientes[i] , NULL);
	}

	//Fechar Socket 
	close(newsockfd);
	close(sockfd);

	return 0;

//Missing:

//Incluir Sistema Para Monitor Enviar Sinais Para o Simulador a Dizer que Comeca a Simulacao. Sem Recorrer a Variaveis Globais


}



