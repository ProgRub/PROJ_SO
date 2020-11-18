
#include "unix.h"

//Variaveis Que Vao Guardar os Valores dos Parametros Lidos no Ficheiro Config
int Tempo_Medio_Chegadas=0;
int Tempo_Simulacao=0;
int Tamanho_Maximo_Bar=0;
int Tamanho_Maximo_Carroussel=0;
int Tamanho_Maximo_Feira=0;
int Tempo_Maximo_Espera_Filas=0;
int Percentagem_Ser_Cliente_Crianca=0;
int Percentagem_Ser_Cliente_Mobilidade_Reduzida=0;
int Numero_Clientes_A_Criar=0;
int Tempo_Medio_Fica_Zonas=0;
int Percentagem_Ficar_Bar=0;
int Tempo_Medio_Estadia_Cliente_Na_Feira=0;

int newsockfd;
int sockfd;

//---------------------------------------------------------------------------------------------------------------------------------

//Funcao Que Escreve Mensagem Para Enviar Pela Socket 
void Escreve_Mensagem(int newsockfd, int ID_Cliente, int Tipo_Cliente, int Estado, int Hora_Actual, int Idade, int Hora_Entrada_Fila_Bar, int Hora_Desistencia_Fila_Bar, int Hora_Atendimento_Bar, int Hora_Entrada_Fila_Carroussel, int Hora_Desistencia_Fila_Carroussel, int Hora_Entrada_Carroussel, int Hora_Entrada_Fila_MontanhaRussa, int Hora_Desistencia_Fila_MontanhaRussa, int Hora_Andou_MontanhaRussa, int Hora_Entrada_Filas_Bilheteiras, int Hora_Desistencia_Filas_Bilheteiras, int Hora_Entrada_Feira, int Hora_Saida_Feira)
{

	char Buffer[MAX_LINE];
	int Tamanho_Mensagem;

	sprintf(Buffer,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", ID_Cliente, Tipo_Cliente, Estado,Hora_Actual,Idade,Hora_Entrada_Fila_Bar, Hora_Desistencia_Fila_Bar, Hora_Atendimento_Bar, Hora_Entrada_Fila_Carroussel, Hora_Desistencia_Fila_Carroussel, Hora_Entrada_Carroussel, Hora_Entrada_Fila_MontanhaRussa, Hora_Desistencia_Fila_MontanhaRussa, Hora_Andou_MontanhaRussa, Hora_Entrada_Filas_Bilheteiras, Hora_Desistencia_Filas_Bilheteiras, Hora_Entrada_Feira, Hora_Saida_Feira); // Writes in simulator socket buffer a message to monitor

	Tamanho_Mensagem=strlen(Buffer)+1; // Add one character more because of \n

	if(write(newsockfd,Buffer,Tamanho_Mensagem)!=Tamanho_Mensagem)
		perror("Erro ao escrever no cliente\n");
}

//---------------------------------------------------------------------------------------------------------------------------------

//Funcao Para Ler do Ficheiro Config:
void Ler_Ficheiro_Config()
{
	//Declaracao e Inicializacao de Variaveis Auxiliares da Funcao
	char Le_Linha[100];
	int Valor_Variavel_Lida;
	char Nome_Variavel_Lida[100];

	char Var1[]="T_Med_Cheg"; //"Tempo_Medio_Chegadas";
	char Var2[]="T_Sim"; //"Tempo_Simulacao";
	char Var3[]="Tam_Max_Bar"; //"Tamanho_Maximo_Bar";
	char Var4[]="Tam_Max_Carroussel"; //"Tamanho_Maximo_Carroussel";
	char Var5[]="Tam_Max_Feira"; //"Tamanho_Maximo_Feira";
	char Var6[]="T_Max_Esp_Filas"; //"Tempo_Maximo_Espera_Filas";
	char Var7[]="P_Ser_Cli_Cri"; //"Percentagem_Ser_Cliente_Crianca";
	char Var8[]="P_Ser_Cli_MR"; //"Percentagem_Ser_Cliente_Mobilidade_Reduzida";
	char Var9[]="N_Cli_Criar"; //"Numero_Clientes_A_Criar";
	char Var10[]="T_Med_Fica_Zona";//"Tempo_Medio_Fica_Zonas";
	char Var11[]="P_Fica_Bar";//"Percentagem_Ficar_Bar";
	char Var12[]="T_Med_Fica_Feira";//"Tempo_Medio_Estadia_Cliente_No_Feira";

	int Erro=0; //Se Uma Das Restricoes For Violada, a Variavel Passa Para 1

	FILE *fp;
	fp=fopen("config.conf","r"); //Vai Abrir o Ficheiro Config.conf
	if(fp!=NULL)
	{
		//Ciclo de Leitura:
		while(fgets(Le_Linha,100,fp)!=NULL)
		{
			/* %s= Nome da Variavel | %i= Valor da Variavel */
			sscanf(Le_Linha, "%s = %i", Nome_Variavel_Lida, &Valor_Variavel_Lida);
			
			/* Strcmp Tem Valor 0  se [Elemento1]=[Elemento2] */
			if(strcmp(Nome_Variavel_Lida,Var1)==0)
			{
				Tempo_Medio_Chegadas=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var2)==0)
			{
				Tempo_Simulacao=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var3)==0)
			{
				Tamanho_Maximo_Bar=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var4)==0)
			{
				Tamanho_Maximo_Carroussel=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var5)==0)
			{
				Tamanho_Maximo_Feira=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var6)==0)
			{
				Tempo_Maximo_Espera_Filas=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var7)==0)
			{
				Percentagem_Ser_Cliente_Crianca=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var8)==0)
			{
				Percentagem_Ser_Cliente_Mobilidade_Reduzida=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var9)==0)
			{
				Numero_Clientes_A_Criar=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var10)==0)
			{
				Tempo_Medio_Fica_Zonas=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var11)==0)
			{
				Percentagem_Ficar_Bar=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var12)==0)
			{
				Tempo_Medio_Estadia_Cliente_No_Feira=Valor_Variavel_Lida;
			}
		}
		fclose(fp);
	}

	else
	{
		printf("Erro Abertura Ficheiro");
	}
}

//-----------------------------------------------------------------------------------------------------------------------------


int main(int argc, char *argv[])
{
	//Le os Parametros do Ficheiro Config
	Ler_Ficheiro_Config();

	int clilen, servlen,i;

	//int sockfd, newsockfd, clilen, servlen,i;
	struct sockaddr_un cli_addr, serv_addr;

	// Cria Socket 
	if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0)) < 0) perror("Erro ao criar socket stream servidor\n");
	unlink(UNIXSTR_PATH);
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	
	strcpy(serv_addr.sun_path, UNIXSTR_PATH);

	servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0)	perror("Erro ao atribuir nome socket servidor\n");
	listen(sockfd, 1);
	
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) perror("Erro ao criar ligacao dedicada - accept\n");



	printf("Pressione Enter Para Comecar Simulacao \n");
	
	char Tecla=0;
	while (Tecla!='\n' && Tecla!='\r') {Tecla=getchar();}

	sleep(1);
	
	
	printf("Pressione Enter Para Enviar Dados \n");
	
	char Tecla1=0;
	while (Tecla1!='\n' && Tecla1!='\r') {Tecla1=getchar();}
	printf("Mensagem 1 \n");
	Escreve_Mensagem(newsockfd,45,54,1,54,484,545,84,4,54,8454,8484,4115,54,5,7,88,55,547);
 //4º Parametro (1), Tem de Ficar Sempre 1 ->Significa Que É Para Simular. Se Este Parametro For =50, Simulacao Acaba.
	sleep(4);
	printf("Mensagem 2 \n");
	Escreve_Mensagem(newsockfd,234,51234,1,5434,483454,544565,8467,477,54567,84854,8484,4115,554,54,72,445,53343465,54457);
	sleep(4);
	printf("Mensagem 3 \n");
	Escreve_Mensagem(newsockfd,4345,54434,1,52344,42348467,5457457,84,4,54,84854,85484,4114565,5234,54,756,83548,34555,534547);
	sleep(4);
	printf("Mensagem 4 \n");
	Escreve_Mensagem(newsockfd,455,54214,1,56654,483454,5465,854,458,5674,8454,8484,41185465,54,5,7,86368,55,547);
	sleep(4);
	printf("Mensagem 5 \n");
	Escreve_Mensagem(newsockfd,451,5432,1,584,4864,55645,484,564,574,84524,84864,41715,541,54,71,838,5544,547553);


	sleep(2);

	printf("Pressione Enter Para Fechar Simulacao \n");
	
	char Tecla2=0;
	while (Tecla!='\n' && Tecla2!='\r') {Tecla2=getchar();}

	sleep(2);

	//Mensagem de Fim de Simulacao
	Escreve_Mensagem(newsockfd,0,0,50,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
	printf("Enviou Mensagem Fim Simulacao!\n");


	sleep(2);

	//Fechar Socket 
	close(newsockfd);


	return 0;

}

