//Chamada das bibliotecas.
#include "unix.h"

int sockfd;
int newsockfd;

pthread_t Thread_Recebe_Mensagens;

//Variaveis utilizadas na Simulacao:
bool Simulacao_Em_Curso=false; //Variavel Vai Indicar que Esta Uma Simulacao em Curso
int Escolha_utilizador=0; //Variavel Que Guarda a Escolha do Utilizador. Quando É Igual a 7, Aplicacao Fecha. Tem de Ser Inicializado a 0 Por Causa da Funcao "Escreve_Cabecalho(int Escolha_Menu)"

// Gravação no ficheiro "resultados.txt" dos dados da simulação.
void escreve(){
	FILE* resultados;
	
	resultados = fopen("resultados.txt", "a");
	
	if(resultados == NULL){
		printf("ERRO NA CRIAÇÃO DO FICHEIRO");
	}
	else{
		fprintf(resultados, "-----INÍCIO DA SIMULAÇÃO-----\n");
		fprintf(resultados, "tempo_medio:20\n");
		fprintf(resultados, "tem_serv_prod:25\n");
		fprintf(resultados, "prob_des: 0.05\n");
		fprintf(resultados, "capacidade_fila:20\n");
		fprintf(resultados, "-----FIM DA SIMULAÇÃO-----\n");
	}
	fclose(resultados);
}

// Interface do Menu Principal (Monitor).
void Menu_Interface(){
system("clear");
printf("              ┌──────────────────────────────────────────────────────────────┐\n");
printf("              │   Sistemas Operativos 2018 - 2019 ─ Simulacao de uma Loja    │\n");
printf("              └──────────────────────────────────────────────────────────────┘\n");
printf("              │                      Dados da Simulacao                      │\n");
printf("              ┌──────────────────────────────────────────────────────────────┐\n");
printf("              │opcao 1 - iniciar simulacao                                   │\n");
printf("              │opcao 2 - sair                                                │\n");
printf("              │                                                              │\n");
printf("              │                                                              │\n");
printf("              │                                                              │\n");
printf("              │                                                              │\n");
printf("              │                                                              │\n");
printf("              │                                                              │\n");
printf("              │                                                              │\n");
printf("              │                                                              │\n");
printf("              └──────────────────────────────────────────────────────────────┘\n");
}

//---------------------------------------------------------------------------------------------------------------------------
//Funcao Que Recebe Mensagem da Socket e Passa Cada Campo da Mensagem Para Uma Variavel Propria
int * Recebe_Mensagem(int sockfd)
{
	int Mensagem_Recebida=0;
	int ID_Cliente=0;
	int Estado=0; 
	int produto=0;
	int desistencias=0; 
	int pac_min=0;
	int pac_max=0;
	int stock_prod=0;
	int empregados=0;
	int cliente_troca=0;


	char recvline[MAX_LINE+1];
	static int Mensagem[9]; //Conversao dos Valores da Mensagem Para Array a Ser Enviado Para Interpretacao

	Mensagem_Recebida=read(sockfd, recvline, MAX_LINE);

	if (Mensagem_Recebida < 0) perror("Erro ao ler mensagem no servidor\n");
	recvline[Mensagem_Recebida] = 0;
	
	sscanf(recvline,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &ID_Cliente, &Estado, &produto, &desistencias, &pac_min, &pac_max, &stock_prod, &empregados, &cliente_troca);

	Mensagem[0] = ID_Cliente;
	Mensagem[1] = Estado;
	Mensagem[2] = produto;
	Mensagem[3] = desistencias;
	Mensagem[4] = pac_min;
	Mensagem[5] = pac_max;
	Mensagem[6] = stock_prod;
	Mensagem[7] = empregados;
	Mensagem[8] = cliente_troca;


	return Mensagem;
}
//-------------------------------------------------------------------------------------------------------------------------

//Funcao de Leitura e Interpretacao da Mensagem Recebida Pela Socket
void Interpreta_Mensagem()
{
	int* Buffer=Recebe_Mensagem(sockfd); //(newsockfd);
	int ID_Cliente=Buffer[0]; //Numero Processo
	int Estado=Buffer[1]; //Ver Tabela de Estados
	int produto=Buffer[2]; //Produto a escolher (Produto A,B ou C)
	int desistencias=Buffer[3]; 
	int pac_min=Buffer[4];
	int pac_max=Buffer[5];
	int stock_prod=Buffer[6];
	int empregados=Buffer[7];
	int cliente_troca=Buffer[8];

	switch(Estado) //Evento
	{
		case 1: //Simulação
		
		//Fazer os printf();
		printf("ID_Cliente=%d",ID_Cliente);
		printf("\n");
		printf("Estado=%d",Estado);
		printf("\n");
		
	//---------------------------------------------------------------------------------------
		case 50: //Fim Simulação

		Simulacao_Em_Curso=false;

		break;
	//---------------------------------------------------------------------------------------
		default: printf("Erro: Estado do Processo Errado. \n");
		break;


	}//Fim Switch
}
//---------------------------------------------------------------

//Funcao da tarefa Que Recebe Mensagens.

void Activa_Ciclo_Recebe_Mensagens()
{
	while(Simulacao_Em_Curso!=false)//==true)
	{
		Interpreta_Mensagem();
	}
}
//-------------------------------------------------------------------

//Menu Principal:
void Menu_Principal()
{
	//Escreve_Cabecalho(Escolha_utilizador);
	printf("Opcao="); //printf("$");

	//verifica a escolha do menu
	scanf("%d",&Escolha_utilizador);

	switch(Escolha_utilizador)
	{
		case 1: //Iniciar Simulacao
		if(Simulacao_Em_Curso==false) //Se Nao Houver Nenhuma Simulacao Activa, Vai Comecar Uma Simulacao Nova
		{
			//Escreve_Cabecalho(Escolha_Menu);
			printf("│            Inicio da Simulacao             │\n"); 
			Simulacao_Em_Curso=true;
			//Coloca_Linhas_Em_Branco(24);

			if(pthread_create(&(Thread_Recebe_Mensagens), NULL ,(void *)&Activa_Ciclo_Recebe_Mensagens, NULL) != 0)
			{
				printf("Erro na criacao da tarefa\n");
				exit(1);
			}

		}
		else //Se Houver uma Simulacao Activa Devolve Erro, Pois So Podemos Executar Esta Opcao Se Nao Estiver Nenhuma Simulacao Activa
		{
			printf("Erro: Simulacao Activa Neste Momento \n");
		}

		break;	
//---------------------------------------------------------------------------------------
		case 2: 
			close(newsockfd);
			close(sockfd);

			exit(0);
		break;		
//---------------------------------------------------------------------------------------
		default: printf("Erro: Opcao Inserida Nao Definida. \n");
		break;
	}
}
//------------------------------------------------------------------------------------------------------------------------
int main(){
	//Chamada do ficheiro para gravação '.txt'. 
	/*escreve();
	printf("O ficheiro resultados.txt foi criado!\n");
	return 0;
	*/
	//Faz Ligacao à Socket Quando Simulador é Iniciado. O Simulador tem de Começar Sempre primeiro
	int  servlen;
	struct sockaddr_un serv_addr;

	if ((sockfd= socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		perror("Erro ao criar socket cliente\n");
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;

	strcpy(serv_addr.sun_path, UNIXSTR_PATH);
	servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

	if (connect(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0)
	{
		perror("Erro ao fazer connect\n");
	}


	system("clear");
	Menu_Interface();
	//Coloca_Linhas_Em_Branco(24);

	while(Escolha_utilizador!=2)
	{
		Menu_Principal();
	}

	return 0;
}
