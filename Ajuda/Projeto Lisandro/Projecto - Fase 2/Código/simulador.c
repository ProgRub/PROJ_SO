// Chamada das bibliotecas.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unix.h"
#include <unistd.h>
//---------------------------------------------------------------------------------------------------------------------------------
// Declaração das Variáveis Globais
int num_cliente = 0;
int tempo_medio = 0;
int tempo_simulacao = 0;
int capacidade_fila = 0;
int probalidade_desistencia = 0;
int tempo_serv_prod = 0;


int newsockfd;
int sockfd;

//---------------------------------------------------------------------------------------------------------------------------------
//Funcao Que Escreve Mensagem Para Enviar Pela Socket 
void Escreve_Mensagem(int newsockfd, int ID_Cliente, int Estado, int produto, int desistencias, int pac_min, int pac_max, int stock_prod, int empregados, int cliente_troca)
{

	char Buffer[MAX_LINE];
	int Tamanho_Mensagem;

	sprintf(Buffer,"%d %d %d %d %d %d %d %d %d ", ID_Cliente, Estado, produto, desistencias, pac_min, pac_max, stock_prod, empregados, cliente_troca);

	Tamanho_Mensagem=strlen(Buffer)+1; // Add one character more because of \n

	if(write(newsockfd,Buffer,Tamanho_Mensagem)!=Tamanho_Mensagem)
		perror("Erro ao escrever no cliente\n");
}
//---------------------------------------------------------------------------------------------------------------------------------
// Função Responsável por ler o Ficheiro de Configuração 'configuracao_simulacao.txt'.
void leitura()
{
	FILE* config;
	config = fopen("configuracao_simulacao.txt", "r");

	if(config != NULL){
	
		char linha[50];
		int valor;
		char lixo[50];

		while(fgets(linha, sizeof(linha), config) != NULL){
			//valor = strtok(linha, ":");
			sscanf(linha, "%s : %d",lixo, &valor);
			
			if(strcmp(lixo, "num_cliente") == 0){
				num_cliente = valor;
			}

			if(strcmp(lixo, "tempo_medio") == 0){
				tempo_medio = valor;
			}
			if(strcmp(lixo, "temp_sim") == 0) {
				
				tempo_simulacao = valor;
			}
			
			if(strcmp(lixo, "capac_fila") == 0) {
				
				capacidade_fila = valor;
			}
			
			if(strcmp(lixo, "prob_des") == 0) {
				
				probalidade_desistencia = valor;
			}
			
			if(strcmp(lixo, "tempo_serv_prod") == 0) {
				tempo_serv_prod = valor;
			}		
		}
	} else {
		printf("Erro ao Abrir o Ficheiro. Tente Novamente.\n");
	}
	fclose(config);	
}
//---------------------------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	leitura();
	/*printf("Tempo de Medio de Espera: %d\n",tempo_medio);
	printf("Tempo de Servico em Cada Produto: %d\n",tempo_serv_prod);
	printf("Probablidade de Desistencia: %d\n",probalidade_desistencia);	
	printf("Capacidade da Fila: %d\n",capacidade_fila);
	printf("Tempo de Simulação: %d\n",tempo_simulacao);*/
	
	// Leitura do ficheiro configuração.

	int clilen, servlen,i;
	struct sockaddr_un cli_addr, serv_addr;

	// Cria Socket 
	if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0)) < 0) 
		perror("Erro ao criar socket stream servidor\n");
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
	//-----------------------------------------------------------------------------
	printf("Pressione space Para Comecar Simulacao \n");
	
	char Tecla=0;
	while (Tecla!=' ') {Tecla=getchar();}

	sleep(1);
	
	printf("Pressione space Para Enviar Dados \n");
	
	char Tecla1=0;
	while (Tecla1!=' ') {Tecla1=getchar();}
	printf("Mensagem 1 \n");
	Escreve_Mensagem(newsockfd,45,1,2,484,2,10,10,5,0);
 //O segundo parametro tem de ser (1), Tem de Ficar Sempre 1 -> Se Estado For =50, Simulacao Acaba.
	sleep(4);
	printf("Mensagem 2 \n");
	Escreve_Mensagem(newsockfd,54,1,1,500,2,10,8,4,0);
	sleep(4);
	printf("Mensagem 3 \n");
	Escreve_Mensagem(newsockfd,2,1,3,510,2,10,6,6,0);
	sleep(4);
	printf("Mensagem 4 \n");
	Escreve_Mensagem(newsockfd,30,1,3,530,2,10,7,5,0);
	sleep(4);
	printf("Mensagem 5 \n");
	Escreve_Mensagem(newsockfd,65,1,2,550,2,10,8,6,1);


	sleep(2);
	
	printf("Pressione space Para Fechar Simulacao \n");
	
	char Tecla2=0;
	while (Tecla!=' ') {Tecla2=getchar();}
	
	sleep(2);

	//Mensagem de Fim de Simulacao
	Escreve_Mensagem(newsockfd,0,50,0,0,0,0,0,0,0);
	printf("Enviou Mensagem Fim Simulacao!\n");


	sleep(2);

	//Fechar o Socket 
	close(newsockfd);
	return 0;
	
	
}
