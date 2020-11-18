
#include "unix.h"

int sockfd;
int newsockfd;

pthread_t Thread_Recebe_Mensagens;

//------------------------------------------------------------------------------------------------------------------

//Variaveis da Simulacao:
bool Simulacao_Em_Curso=false; //Variavel Vai Indicar que Esta Uma Simulacao em Curso (Se For True Podemos Activar a Ocupacao das Zonas em Tempo Real)
int Escolha_Menu=0; //Variavel Que Guarda a Escolha do Utilizador. Quando É Igual a 7, Aplicacao Fecha. Tem de Ser Inicializado a 0 Por Causa da Funcao "Escreve_Cabecalho(int Escolha_Menu)"

//---------------------------------------------------------------------------------------------------------------------------------

// Cria Cabeçalho do Interface do Monitor
void Escreve_Cabecalho(int Escolha_Menu)
{
system("clear");
printf("                        ┌──────────────────────────────────────────────────────────────┐\n");
printf("                        │   Sistemas Operativos 2017/2018 ─ Simulacao Montanha Russa   │\n");
printf("                        └──────────────────────────────────────────────────────────────┘\n");
printf("┌─────────────────────────────────────────┌────────────────────────────────────────────────────────────────────┐\n");
printf("│                                         │  ■ 1-Inicio Simulacao          ■ 2-Estatisticas Tempo Real         │\n");

switch(Escolha_Menu)
{
	case 0: printf("│             Menu Principal              │  ■ 3-Eventos Tempo Real        ■ 4-Estado Zonas Tempo Real         │\n"); break;
	case 1: printf("│            Inicio Simulacao             │  ■ 3-Eventos Tempo Real        ■ 4-Estado Zonas Tempo Real         │\n"); break;
	case 2: printf("│         Estatisticas Tempo Real         │  ■ 3-Eventos Tempo Real        ■ 4-Estado Zonas Tempo Real         │\n"); break;		
	case 3: printf("│            Eventos Tempo Real           │  ■ 3-Eventos Tempo Real        ■ 4-Estado Zonas Tempo Real         │\n"); break;
	case 4: printf("│         Estado Zonas Tempo Real         │  ■ 3-Eventos Tempo Real        ■ 4-Estado Zonas Tempo Real         │\n"); break;
	case 5: printf("│                  Sobre                  │  ■ 3-Eventos Tempo Real        ■ 4-Estado Zonas Tempo Real         │\n"); break;
	case 6: printf("│                  Ajuda                  │  ■ 3-Eventos Tempo Real        ■ 4-Estado Zonas Tempo Real         │\n"); break;
	case 7: printf("│                   Sair                  │  ■ 3-Eventos Tempo Real        ■ 4-Estado Zonas Tempo Real         │\n"); break;
	default: printf("│          Erro: Opcao Errada            │  ■ 3-Eventos Tempo Real        ■ 4-Estado Zonas Tempo Real         │\n"); break;
}

printf("│                                         │  ■ 5-Sobre                     ■ 6-Ajuda                ■ 7-Sair   │\n");
printf("└─────────────────────────────────────────┘────────────────────────────────────────────────────────────────────┘\n");
}



//Funcao Que Coloca Linhas em Branco Para Manter Efeito Estetico
void Coloca_Linhas_Em_Branco(int Quantidade_Linhas)
{
	int i=0;

	for(i=0;i<=Quantidade_Linhas;i++)
	{
		if(i==Quantidade_Linhas)
		{
			printf("└──────────────────────────────────────────────────────────────────────────────────────────────────────────────┘\n");
		}
		else
		{
			printf("│                                                                                                              │\n");
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------------------

//Funcao Que Recebe Mensagem da Socket e Passa Cada Campo da Mensagem Para Uma Variavel Propria
int * Recebe_Mensagem(int sockfd)
{
	int Mensagem_Recebida=0;
	int ID_Cliente=0;
	int Tipo_Cliente=0;
	int Estado=0; 
	int Hora_Actual=0;
	int Hora_Entrada_Fila_Bar=0; 
	int Hora_Desistencia_Fila_Bar=0;
	int Hora_Atendimento_Bar=0;
	int Hora_Entrada_Fila_Carroussel=0;
	int Hora_Desistencia_Fila_Carroussel=0;
	int Hora_Entrada_Carroussel=0;
	int Hora_Entrada_Fila_MontanhaRussa=0;
	int Hora_Desistencia_Fila_MontanhaRussa=0;
	int Hora_Andou_MontanhaRussa=0;
	int Hora_Entrada_Filas_Bilheteiras=0;
	int Hora_Desistencia_Filas_Bilheteiras=0;
	int Hora_Entrada_Feira=0;
	int Hora_Saida_Feira=0;
	
	int Idade=0;

	char recvline[MAX_LINE+1];
	static int Mensagem[18]; //Conversao dos Valores da Mensagem Para Array a Ser Enviado Para Interpretacao

	Mensagem_Recebida=read(sockfd, recvline, MAX_LINE);

	if (Mensagem_Recebida < 0) perror("Erro ao ler mensagem no servidor\n");
	recvline[Mensagem_Recebida] = 0;
	
	sscanf(recvline,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &ID_Cliente, &Tipo_Cliente, &Estado, &Hora_Actual, &Idade, &Hora_Entrada_Fila_Bar, &Hora_Desistencia_Fila_Bar, &Hora_Atendimento_Bar, &Hora_Entrada_Fila_Carroussel, &Hora_Desistencia_Fila_Carroussel, &Hora_Entrada_Carroussel, &Hora_Entrada_Fila_MontanhaRussa, &Hora_Desistencia_Fila_MontanhaRussa, &Hora_Andou_MontanhaRussa, &Hora_Entrada_Filas_Bilheteiras, &Hora_Desistencia_Filas_Bilheteiras, &Hora_Entrada_Feira,&Hora_Saida_Feira);

	Mensagem[0] = ID_Cliente;
	Mensagem[1] = Tipo_Cliente;
	Mensagem[2] = Estado;
	Mensagem[3] = Hora_Actual;
	Mensagem[4] = Idade;
	Mensagem[5] = Hora_Entrada_Fila_Bar;
	Mensagem[6] = Hora_Desistencia_Fila_Bar;
	Mensagem[7] = Hora_Atendimento_Bar;
	Mensagem[8] = Hora_Entrada_Fila_Carroussel;
	Mensagem[9] = Hora_Desistencia_Fila_Carroussel;
	Mensagem[10] = Hora_Entrada_Carroussel;
	Mensagem[11] = Hora_Entrada_Fila_MontanhaRussa;
	Mensagem[12] = Hora_Desistencia_Fila_MontanhaRussa;
	Mensagem[13] = Hora_Andou_MontanhaRussa;
	Mensagem[14] = Hora_Entrada_Filas_Bilheteiras;
	Mensagem[15] = Hora_Desistencia_Filas_Bilheteiras;
	Mensagem[16] = Hora_Entrada_Feira;
	Mensagem[17] = Hora_Saida_Feira;

	return Mensagem;
}


//---------------------------------------------------------------------------------------------------------------------------------


//Funcao de Leitura e Interpretacao da Mensagem Recebida Pela Socket
void Interpreta_Mensagem()
{
	int* Buffer=Recebe_Mensagem(sockfd); //(newsockfd);
	int ID_Cliente=Buffer[0]; //Numero Processo
	int Tipo_Cliente=Buffer[1]; //Tipo Cliente= {Mobilidade Reduzida, Crianças, Publico Geral}
	int Estado=Buffer[2]; //Ver Tabela de Estados
	int Hora_Actual=Buffer[3]; //Hora de um Acontecimento (Depende do Estado em que Esta e Esteve)
	int Idade=Buffer[4]; //Idade do Cliente
	int Hora_Entrada_Fila_Bar=Buffer[5];
	int Hora_Desistencia_Fila_Bar=Buffer[6];
	int Hora_Atendimento_Bar=Buffer[7];
	int Hora_Entrada_Fila_Carroussel=Buffer[8];
	int Hora_Desistencia_Fila_Carroussel=Buffer[9];
	int Hora_Entrada_Carroussel=Buffer[10];
	int Hora_Entrada_Fila_MontanhaRussa=Buffer[11];
	int Hora_Desistencia_Fila_MontanhaRussa=Buffer[12];
	int Hora_Andou_MontanhaRussa=Buffer[13];
	int Hora_Entrada_Filas_Bilheteiras=Buffer[14];
	int Hora_Desistencia_Filas_Bilheteiras=Buffer[15];
	int Hora_Entrada_Feira=Buffer[16];
	int Hora_Saida_Feirs=Buffer[17];

	switch(Estado) //Evento
	{
		case 1: //Simulação
		
		//Fazer os printf();
		printf("ID_Cliente=%d",ID_Cliente);
		printf("\n");
		printf("Tipo_Cliente=%d",Tipo_Cliente);
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

//---------------------------------------------------------------------------------------------------------------------------------

//Teste Funcao da Thread Que Recebe Mensagens.

void Activa_Ciclo_Recebe_Mensagens()
{
	while(Simulacao_Em_Curso!=false)//==true)
	{
		Interpreta_Mensagem();
	}
}

//---------------------------------------------------------------------------------------------------------------------------------


//Menu Principal:
void Menu_Principal()
{
	//Escreve_Cabecalho(Escolha_Menu);
if(Escolha_Menu!=3)
{
	printf("Opcao="); //printf("$");
}
	scanf("%d",&Escolha_Menu);

	switch(Escolha_Menu)
	{
		case 1: //Iniciar Simulacao
		if(Simulacao_Em_Curso==false) //Se Nao Houver Nenhuma Simulacao Activa, Vai Comecar Uma Simulacao Nova
		{
			Escreve_Cabecalho(Escolha_Menu);
			Simulacao_Em_Curso=true;
			Coloca_Linhas_Em_Branco(24);

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
		case 2: //Estatisticas Tempo Real
			printf("Em Construção");
		break;
//---------------------------------------------------------------------------------------
		case 3: //Eventos em Tempo Real
			printf("Em Construção");
		break;
//---------------------------------------------------------------------------------------
		case 4: //Estado Zonas em Tempo Real
			printf("Em Construção");
		break;
//---------------------------------------------------------------------------------------
		case 5: //Sobre
			printf("Em Construção");
		break;
//---------------------------------------------------------------------------------------
		case 6: //Ajuda
			printf("Em Construção");
		break;		
//---------------------------------------------------------------------------------------
		case 7: //Fechar
			//Fechar Socket 
			close(newsockfd);
			close(sockfd);

			exit(0);
		break;		
//---------------------------------------------------------------------------------------
		default: printf("Erro: Opcao Inserida Nao Definida. \n");
		break;
	}
}


//---------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	//Faz Ligacao à Socket Quando Simulador é Iniciado. Simulador Comeca Sempre 1º
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
	Escreve_Cabecalho(0);
	Coloca_Linhas_Em_Branco(24);

	while(Escolha_Menu!=7)
	{
		Menu_Principal();
	}

	return 0;
}


