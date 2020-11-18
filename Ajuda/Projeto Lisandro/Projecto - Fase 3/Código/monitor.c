//Chamada das Livrarias.
#include "unix.h"

int sockfd;
int newsockfd;
pthread_t Thread_Recebe_Mensagens;

//------------------------------------------------------------------------------------------------------------------
//Variaveis utilizadas para execução da Simulacao:
bool Simulacao_Em_Curso = false;				// Indica se a simulação está em curso.
int Opcao_Utilizador = 0;						// Variavel para armazenar as opções selecionadas pelo utilizador.
//------------------------------------------------------------------------------------------------------------------
//Variaveis a Receber do Simulador.
int Capacidade_Maxima_Loja = 0;

//------------------------------------------------------------------------------------------------------------------
//Funcao Para Ler do Ficheiro Config:
void Leitura_Ficheiro_Confg()
{
	//Declaracao e Inicializacao de Variaveis Auxiliares da Funcao
	char Le_Linha[100];
	int Valor_Variavel_Lida;
	char Nome_Variavel_Lida[100];
	char Var1[]="Cap_Maxima_Loja"; 			// Capacidade Máxima da Loja.

	FILE *fp;
	fp=fopen("config.conf","r"); 			//Vai Abrir o Ficheiro Config.conf.
	
	if(fp!=NULL){
		//Ciclo de Leitura:
		while(fgets(Le_Linha,100,fp)!=NULL){
			sscanf(Le_Linha, "%s = %i", Nome_Variavel_Lida, &Valor_Variavel_Lida);
			
			if(strcmp(Nome_Variavel_Lida,Var1)==0){
				Capacidade_Maxima_Loja=Valor_Variavel_Lida;
			}
		}
		fclose(fp);
	}
		else{
		printf("Nao Foi Possivel Abrir o Ficheiro Configuracao.");
		}
}

//------------------------------------------------------------------------------------------------------------------
// Declaração das Variáveis Globais
int Total_Clientes_Idosos_Entraram_Fila_Loja; 
int Total_Clientes_Idosos_Atendidos_Loja; 
int Total_Clientes_Idosos_Desistiram_Fila_Loja; 
int Total_Clientes_Adultos_Entraram_Fila_Loja; 
int Total_Clientes_Adultos_Atendidos_Loja; 
int Total_Clientes_Adultos_Desistiram_Fila_Loja; 
int Total_Clientes_Actualmente_Na_Fila_Loja; 
int Total_Clientes_Actualmente_Na_Loja;
int Total_Clientes_Sairam_Loja;
int quantidade_prodA;
int quantidade_prodB;
int quantidade_prodC;
int total_vendas_A = 0;
int total_vendas_B = 0;
int total_vendas_C = 0;

//------------------------------------------------------------------------------------------------------------------
// Interface do Menu Principal (Monitor).
void Menu_Interface(){
system("clear");
printf("┌──────────────────────────────────────────────────────────────┐\n");
printf("│   Sistemas Operativos 2018 - 2019 ─ Simulacao de uma Loja    │\n");
printf("└──────────────────────────────────────────────────────────────┘\n");
printf("│                      Dados da Simulacao                      │\n");
printf("┌──────────────────────────────────────────────────────────────┐\n");
printf("│* Opcao 1 - Iniciar Simulacao                                 │\n");
printf("│* Opcao 2 - Sair                                              │\n");
printf("│* Opcao 3 - Ver Eventos                                       │\n");
printf("│* Opcao 4 - Ver Estatisticas                                  │\n");
printf("│                                                              │\n");
printf("│                                                              │\n");
printf("│                                                              │\n");
printf("│                                                              │\n");
printf("│                                                              │\n");
printf("│                                                              │\n");
printf("└──────────────────────────────────────────────────────────────┘\n");
}

//------------------------------------------------------------------------------------------------------------------
//Funcao Para Preparar Ficheiro do Log Eventos (Invocado Quando o Utilizador Escolhe a Opcao de Simulacao no Menu Principal):
void Prepara_Ficheiro_Eventos()
{
	FILE *fp;
	fp=fopen("relatorio_eventos.c","w");

	fprintf(fp, "Eventos Loja: \n");
	fprintf(fp, "\n");

	fclose(fp);
}
//Funcao Para Escrever Eventos no Ficheiro Log (Precisamos da Funcao de "Preparacao" Anterior Pois Esta Funcao Vai Ser Invocada Sempre Que Houver um Evento. Dessa Forma Temos de Usar Append em Vez de Write, Senao Apagava Sempre o Conteudo do Ficheiro):
void Escreve_Ficheiro_Eventos(int Evento, int ID_Cliente, int Faixa_Etaria)
{
	char *Nome_Faixa_Etaria;
	char Tipo_Nome_Faixa_Etaria1[]="(IDOSO)";
	char Tipo_Nome_Faixa_Etaria2[]="(ADULTO)";
	
	if(Faixa_Etaria==1){
		Nome_Faixa_Etaria=Tipo_Nome_Faixa_Etaria1;
	}
	else if(Faixa_Etaria==2){
		Nome_Faixa_Etaria=Tipo_Nome_Faixa_Etaria2;
	}
	
	FILE *fp;
	fp=fopen("relatorio_eventos.c","a"); //"a" = Append (Ou Seja, a Linha é Adicionada ao Ficheiro Sem Eliminar Nada)
	
	switch(Evento)
	{
		case 1:
			fprintf(fp, "->Cliente Nº %i (%s) Entrou na Loja \n", ID_Cliente+1,Nome_Faixa_Etaria); 
			fprintf(fp, "-----------------------------------------------------\n");
			break;
			
		case 2:
			fprintf(fp, "->Cliente Nº %i Chegou a fila \n", ID_Cliente+1);  
			fprintf(fp, "-----------------------------------------------------\n");
			break;

		case 3:
			fprintf(fp, "->Cliente Nº %i Foi atendido com o produto A\n", ID_Cliente+1);  
			fprintf(fp, "-----------------------------------------------------\n");
			break;

		case 4:
			fprintf(fp, "->Cliente Nº %i Desistiu da Fila \n", ID_Cliente+1);  
			fprintf(fp, "-----------------------------------------------------\n");
			break;

		case 5:
			fprintf(fp, "->Cliente Nº %i Saiu da Loja \n", ID_Cliente+1);  
			fprintf(fp, "-----------------------------------------------------\n");
			break;

		case 6:
			fprintf(fp, "->Cliente Nº %i Foi atendido com o produto B\n", ID_Cliente+1);  
			fprintf(fp, "-----------------------------------------------------\n");
			break;
		
		case 7:
			fprintf(fp, "->Cliente Nº %i Foi atendido com o produto C\n", ID_Cliente+1);  
			fprintf(fp, "-----------------------------------------------------\n");
			break;
		
		case 8:
			fprintf(fp, "O Empregado Nº1 foi repor o produto A");  
			fprintf(fp, "-----------------------------------------------------\n");
			break;
		
		case 9:
			printf("│ O Empregado Nº1 foi repor o produto B\n"); 
			fprintf(fp, "-----------------------------------------------------\n");
			break;
			
		case 10:
			printf("│ O Empregado Nº1 foi repor o produto C\n");  
			fprintf(fp, "-----------------------------------------------------\n");
			break;
		
		case 50:
			fprintf(fp, "*Simulacao Terminada* \n");  
			fprintf(fp, "\n");
			fprintf(fp, "-----------------------------------------------------\n");
			break;

		default:
			fprintf(fp, "-Erro: Evento Nao Definido\n"); 
			fprintf(fp, "-----------------------------------------------------\n");
	}
	fclose(fp);
}

//------------------------------------------------------------------------------------------------------------------
//Funcao que Mostra o Log de Eventos em Tempo Real
void Mostra_No_Monitor_Eventos_Tempo_Real(int Evento, int ID_Cliente, int Faixa_Etaria, int ID_empregado)
{
	char *Nome_Faixa_Etaria;
	char Tipo_Nome_Faixa_Etaria1[] = "(IDOSO)";
	char Tipo_Nome_Faixa_Etaria2[] = "(ADULTO)";
		
	if(Faixa_Etaria == 1){
		Nome_Faixa_Etaria = Tipo_Nome_Faixa_Etaria1;
	}
	
		else if(Faixa_Etaria == 2){
			Nome_Faixa_Etaria = Tipo_Nome_Faixa_Etaria2;
		}
	
	switch(Evento)
	{
		case 1:
			printf("│ O Cliente Nº %d %s Chegou a fila \n", ID_Cliente+1,Nome_Faixa_Etaria);
			break;

		case 2:
		 	printf("│ O Cliente Nº %d %s foi atendido pelo empregado %d com o produto A\n", ID_Cliente+1, Nome_Faixa_Etaria, ID_empregado);		
			break;

		case 3:
			printf("│ O Cliente Nº %d %s Desistiu\n", ID_Cliente+1, Nome_Faixa_Etaria);   
			break;

		case 4:  
			printf("│ O Cliente Nº %d %s Saiu da Loja \n", ID_Cliente+1, Nome_Faixa_Etaria);
			break;

		case 5:
			printf("│ O Cliente Nº %d %s Entrou na Loja \n", ID_Cliente+1,Nome_Faixa_Etaria);
			break;

		case 6:
			printf("│ O Cliente Nº %d %s Foi Atendido Pelo Empregado %d com o Produto B\n", ID_Cliente+1, Nome_Faixa_Etaria, ID_empregado);  
			break;

		case 7:
			printf("│ O Cliente Nº %d %s Foi Atendido Pelo Empregado %d com o Produto C\n", ID_Cliente+1, Nome_Faixa_Etaria, ID_empregado);  
			break;
		
		case 8:
			printf("│ O Empregado Nº %d Foi Repor o Produto A\n", ID_empregado);  
			break;
			
		case 9:
			printf("│ O Empregado Nº %d Foi Repor o Produto B\n", ID_empregado); 
			break;
			
		case 10:
			printf("│ O Empregado Nº %d Foi Repor o Produto C\n", ID_empregado);  
			break;
			
		case 50:
			printf("│ Simulacao Terminada.  \n");
			break;

		default:
			printf("│ Erro: Caso Nao Definido.  \n");
	}
}

//------------------------------------------------------------------------------------------------------------------
//Funcao Para Mostrar as Estatisticas da simulação no Monitor:
void Mostra_Estatisticas_Monitor( int Total_Clientes_Idosos_Entraram_Fila, int Total_Clientes_Idosos_Atendidos, int Total_Clientes_Idosos_Desistiram_Fila, int Total_Clientes_Adultos_Entraram_Fila, int Total_Clientes_Adultos_Atendidos, int Total_Clientes_Adultos_Desistiram_Fila, int Total_Clientes_Sairam_Loja)
{
	system("clear");
	printf("┌──────────────────────────────────────────────────────────────┐\n");
	printf("│   Sistemas Operativos 2018 - 2019 ─ Simulacao de uma Loja    │\n");
	printf("└──────────────────────────────────────────────────────────────┘\n");
	printf("│                  Estatisticas Simulacao                      │\n");

	// Variáveis Que calculam os Totais de cada parametro (Clientes).
	int Total_Clientes_Atendidos = Total_Clientes_Idosos_Atendidos + Total_Clientes_Adultos_Atendidos;
	int Total_Clientes_Entraram_Fila = Total_Clientes_Idosos_Entraram_Fila + Total_Clientes_Adultos_Entraram_Fila;
	int Total_Clientes_Desistiram_Fila = Total_Clientes_Idosos_Desistiram_Fila + Total_Clientes_Adultos_Desistiram_Fila;
	
		printf("┌──────────────────────────────────────────────────────────────┐\n");
		printf("│*  Total Clientes Atendidos Loja: %d                           │\n", Total_Clientes_Atendidos);
		printf("│*  Total Clientes Sairam Loja: %d                              │\n", Total_Clientes_Sairam_Loja );
		printf("│*  Total Clientes Entraram Fila Loja: %d                       │\n", Total_Clientes_Entraram_Fila);
		printf("│*  Total Clientes Desistiram Fila Loja: %d                     │\n", Total_Clientes_Desistiram_Fila);
		printf("│*  Total Vendas Produto A: %d                                  │\n", total_vendas_A);
		printf("│*  Total Vendas Produto B: %d                                  │\n", total_vendas_B);
		printf("│*  Total Vendas Produto C: %d                                  │\n", total_vendas_C);
		printf("└──────────────────────────────────────────────────────────────┘\n");
		printf("* Opcao 2 - Sair Simulacao		* Opcao 3 - Ver Eventos   \n");
}

void Escreve_Estatisticas(int Total_Clientes_Idosos_Entraram_Fila, int Total_Clientes_Idosos_Atendidos, int Total_Clientes_Idosos_Desistiram_Fila, int Total_Clientes_Adultos_Entraram_Fila, int Total_Clientes_Adultos_Atendidos, int Total_Clientes_Adultos_Desistiram_Fila, int Total_Clientes_Sairam_Loja)
{
	// Variáveis Que calculam os Totais de cada parametro (Clientes).
	int Total_Clientes_Atendidos = Total_Clientes_Idosos_Atendidos + Total_Clientes_Adultos_Atendidos;
	int Total_Clientes_Entraram_Fila = Total_Clientes_Idosos_Entraram_Fila + Total_Clientes_Adultos_Entraram_Fila;
	int Total_Clientes_Desistiram_Fila = Total_Clientes_Idosos_Desistiram_Fila + Total_Clientes_Adultos_Desistiram_Fila;

	FILE *fp;
	fp=fopen("estatisticas.c","w");

	fprintf(fp, "│                        Estatisticas                          │\n");
	fprintf(fp, "\n");
	fprintf(fp, "┌──────────────────────────────────────────────────────────────┐\n");
	fprintf(fp, "│                Clientes Atendidos na Loja                    │ \n");
	fprintf(fp, "└──────────────────────────────────────────────────────────────┘\n");
	fprintf(fp, "\n");
	fprintf(fp, "* Total Clientes Atendidos: %d \n", Total_Clientes_Atendidos);
	fprintf(fp, "* Total Clientes Idosos Atendidos: %d \n", Total_Clientes_Idosos_Atendidos);
	fprintf(fp, "* Total Clientes Adultos Atendidos: %d \n", Total_Clientes_Adultos_Atendidos);

	fprintf(fp, "\n");
	fprintf(fp, "┌──────────────────────────────────────────────────────────────┐\n");
	fprintf(fp, "│                Clientes Entraram na Fila da Loja             │ \n");
	fprintf(fp, "└──────────────────────────────────────────────────────────────┘\n");
	fprintf(fp, "\n");
	fprintf(fp, "* Total Clientes Entraram Fila: %d \n", Total_Clientes_Entraram_Fila);
	fprintf(fp, "* Total Clientes Idosos Entraram Fila: %d \n", Total_Clientes_Idosos_Entraram_Fila_Loja);
	fprintf(fp, "* Total Clientes Adultos Entraram Fila: %d \n", Total_Clientes_Adultos_Entraram_Fila);
	
	fprintf(fp, "\n");
	fprintf(fp, "┌──────────────────────────────────────────────────────────────┐\n");
	fprintf(fp, "│                Clientes Desistiram da Fila da Loja           │ \n");
	fprintf(fp, "└──────────────────────────────────────────────────────────────┘\n");
	fprintf(fp, "\n");
	fprintf(fp, "* Total Clientes Desistiram da Fila: %d \n", Total_Clientes_Desistiram_Fila);
	fprintf(fp, "* Total Clientes Idosos Desistiram da Fila: %d \n", Total_Clientes_Idosos_Desistiram_Fila);
	fprintf(fp, "* Total Clientes Adultos Desistiram da Fila: %d \n", Total_Clientes_Adultos_Desistiram_Fila_Loja);
	
	fprintf(fp, "\n");
	fprintf(fp, "┌──────────────────────────────────────────────────────────────┐\n");
	fprintf(fp, "│                     Vendas de Produtos                       │ \n");
	fprintf(fp, "└──────────────────────────────────────────────────────────────┘\n");
	fprintf(fp, "\n");
	fprintf(fp, "* Total de Vendas Produto A: %d \n", total_vendas_A);
	fprintf(fp, "* Total de Vendas Produto B: %d \n", total_vendas_B);
	fprintf(fp, "* Total de Vendas Produto C: %d \n", total_vendas_C);
	fprintf(fp, "\n");
	fprintf(fp, "\n");

	fprintf(fp, "Dados Impressos Terminados.\n");  
	fprintf(fp, "\n");

	fclose(fp);
}

//------------------------------------------------------------------------------------------------------------------
//Funcao Que Recebe Mensagem da Socket e Passa Cada Campo da Mensagem Para Uma Variavel Propria
int * Recebe_Mensagem(int sockfd)
{
	int Mensagem_Recebida=0;
	int ID_Cliente=0;
	int Estado=0;
	int Faixa_Etaria=0;
	int produto=0;
	int desistencias=0; 
	int pac_min=0;
	int pac_max=0;
	int stock_prod=0;
	int ID_Empregado=0;
	int cliente_troca=0;


	char recvline[MAX_LINE+1];
	static int Mensagem[9]; //Conversao dos Valores da Mensagem Para Array a Ser Enviado Para Interpretacao

	Mensagem_Recebida=read(sockfd, recvline, MAX_LINE);

	if (Mensagem_Recebida < 0) perror("Erro ao ler mensagem no servidor\n");
	recvline[Mensagem_Recebida] = 0;
	
	sscanf(recvline,"%d %d %d %d %d %d %d %d %d %d", &ID_Cliente, &Estado, &Faixa_Etaria, &produto, &desistencias, &pac_min, &pac_max, &stock_prod, &ID_Empregado, &cliente_troca);

	Mensagem[0] = ID_Cliente;
	Mensagem[1] = Estado;
	Mensagem[2] = Faixa_Etaria;
	Mensagem[3] = produto;
	Mensagem[4] = desistencias;
	Mensagem[5] = pac_min;
	Mensagem[6] = pac_max;
	Mensagem[7] = stock_prod;
	Mensagem[8] = ID_Empregado;
	Mensagem[9] = cliente_troca;

	return Mensagem;
}

//------------------------------------------------------------------------------------------------------------------
//Funcao de Leitura e Interpretacao da Mensagem Recebida Pela Socket
void Interpreta_Mensagem()
{
	int* Buffer=Recebe_Mensagem(sockfd); 		//(newsockfd);
	int ID_Cliente=Buffer[0]; 					//Numero Processo
	int Estado=Buffer[1]; 						//Ver Tabela de Estados
	int Faixa_Etaria=Buffer[2];
	int produto=Buffer[3]; 						//Produto a escolher (Produto A,B ou C)
	int stock_prod=Buffer[4];
	int ID_Empregado=Buffer[5];
	int cliente_troca=Buffer[6];

	switch(Estado)
	{
		case 1: 													// Entra Fila da Loja
				Total_Clientes_Actualmente_Na_Fila_Loja++;

				if(Faixa_Etaria==1){	// Idosos
					Total_Clientes_Idosos_Entraram_Fila_Loja++;
				}
					else if(Faixa_Etaria==2){	// Adultos
						Total_Clientes_Adultos_Entraram_Fila_Loja++;
					}
			break;

		case 2: 													// Sai Fila da Loja / É atendido com o produto A
				Total_Clientes_Actualmente_Na_Fila_Loja--;
				quantidade_prodA--;
				total_vendas_A++;
		
				if(Faixa_Etaria == 1){ 	// Idosos
					Total_Clientes_Idosos_Atendidos_Loja++;
				}
					else if(Faixa_Etaria == 2){	// Adultos
						Total_Clientes_Adultos_Atendidos_Loja++;
					}
			break;
			
		case 3:														// Desiste Fila Loja 
				Total_Clientes_Actualmente_Na_Fila_Loja--;
				if(Faixa_Etaria == 1){	// Idosos
					Total_Clientes_Idosos_Desistiram_Fila_Loja++;
					Total_Clientes_Sairam_Loja++;
				}
					else if(Faixa_Etaria == 2){	// Adultos
						Total_Clientes_Adultos_Desistiram_Fila_Loja++;
						Total_Clientes_Sairam_Loja++;
					}
			break;

		case 4: 													// Sai Loja
				Total_Clientes_Actualmente_Na_Loja--;
				Total_Clientes_Sairam_Loja++;
			break;
		
		case 5: 													// Entra Loja
				Total_Clientes_Actualmente_Na_Loja++;
			break;		
			
		case 6: 													// Sai Fila da Loja / É atendido com o produto B
				Total_Clientes_Actualmente_Na_Fila_Loja--;
				quantidade_prodB--;
				total_vendas_B++;
		
				if(Faixa_Etaria == 1){	// Idosos
					Total_Clientes_Idosos_Atendidos_Loja++;
				}
					else if(Faixa_Etaria == 2){	// Adultos
						Total_Clientes_Adultos_Atendidos_Loja++;
					}
			break;
		
		case 7: 													// Sai Fila da Loja / É atendido com o produto C
				Total_Clientes_Actualmente_Na_Fila_Loja--;
				quantidade_prodC--;
				total_vendas_C++;
		
				if(Faixa_Etaria == 1){ // Idosos
					Total_Clientes_Idosos_Atendidos_Loja++;
				}
					else if(Faixa_Etaria == 2){	// Adultos
						Total_Clientes_Adultos_Atendidos_Loja++;
					}
			break;
			
		case 8: 													// Sai Fila da Loja / É atendido com o produto C
				quantidade_prodA = 10;
			break;
		
		case 9: 													// Sai Fila da Loja / É atendido com o produto C
				quantidade_prodB = 10;
			break;
		case 10: 													// Sai Fila da Loja / É atendido com o produto C
				quantidade_prodC = 10;
			break;
			
	
		case 50: 													// Fim Simulação
				Simulacao_Em_Curso = false;
				// Escreve dados das estatisticas quando a simulação termina.
				Escreve_Estatisticas(Total_Clientes_Idosos_Entraram_Fila_Loja, Total_Clientes_Idosos_Atendidos_Loja, Total_Clientes_Idosos_Desistiram_Fila_Loja, Total_Clientes_Adultos_Entraram_Fila_Loja, Total_Clientes_Adultos_Atendidos_Loja, Total_Clientes_Adultos_Desistiram_Fila_Loja, Total_Clientes_Sairam_Loja);
			break;

		default: printf("Erro: Estado do Processo Errado. \n");
			break;
			
	}			//Fim Switch
	
	if(Opcao_Utilizador == 4){	// Ver estatisticas em Tempo real da simulação.
		Mostra_Estatisticas_Monitor(Total_Clientes_Idosos_Entraram_Fila_Loja, Total_Clientes_Idosos_Atendidos_Loja, Total_Clientes_Idosos_Desistiram_Fila_Loja, Total_Clientes_Adultos_Entraram_Fila_Loja, Total_Clientes_Adultos_Atendidos_Loja, Total_Clientes_Adultos_Desistiram_Fila_Loja, Total_Clientes_Sairam_Loja);
	}
		else if(Opcao_Utilizador==3){	// Ver os eventos da simulação em tempo real.
			Mostra_No_Monitor_Eventos_Tempo_Real(Estado, ID_Cliente, Faixa_Etaria, ID_Empregado);
		}
}

//------------------------------------------------------------------------------------------------------------------
//Funcao da tarefa Que Recebe Mensagens.
void Activa_Ciclo_Recebe_Mensagens(){
	
	while(Simulacao_Em_Curso!=false){
		Interpreta_Mensagem();
	}
}

//------------------------------------------------------------------------------------------------------------------
//Menu Principal:
void Menu_Principal(){
	
	if(Opcao_Utilizador!=3){
		printf("Opcao Selecionada: ");
	}
		//verifica a escolha do menus
		scanf("%d",&Opcao_Utilizador);

	switch(Opcao_Utilizador){
			
		case 1: 				//Se Nao Houver Nenhuma Simulacao Activa, começa uma nova.
				if(Simulacao_Em_Curso == false){ 
		
					Prepara_Ficheiro_Eventos();
					system("clear");
					printf("┌──────────────────────────────────────────────────────────────┐\n");
					printf("│   Sistemas Operativos 2018 - 2019 ─ Simulacao de uma Loja    │\n");
					printf("└──────────────────────────────────────────────────────────────┘\n");
					printf("│                      Simulacao Iniciada                      │\n");
					printf("┌──────────────────────────────────────────────────────────────┐\n");
					printf("│* Opcao 2 - Sair                                              │\n");
					printf("│* Opcao 3 - Ver Eventos                                       │\n");
					printf("│* Opcao 4 - Ver Estatisticas                                  │\n");
					printf("└──────────────────────────────────────────────────────────────┘\n");
					Simulacao_Em_Curso = true;
		

					if(pthread_create(&(Thread_Recebe_Mensagens), NULL ,(void *)&Activa_Ciclo_Recebe_Mensagens, NULL) != 0){
						printf("Erro na criacao da tarefa\n");
						exit(1);
					}
				}
					else{ //Se Houver uma Simulacao Activa Devolve Erro.
						printf("Erro: Simulacao Activa Neste Momento \n");
					}
			break;	

		case 2: 				//Escreve Estatisticas (do Momento em Que a Simulacao Foi Interrompida) no Ficheiro
				if(Simulacao_Em_Curso == true){ 
					Escreve_Estatisticas(Total_Clientes_Idosos_Entraram_Fila_Loja, Total_Clientes_Idosos_Atendidos_Loja, Total_Clientes_Idosos_Desistiram_Fila_Loja, Total_Clientes_Adultos_Entraram_Fila_Loja, Total_Clientes_Adultos_Atendidos_Loja, Total_Clientes_Adultos_Desistiram_Fila_Loja, Total_Clientes_Sairam_Loja);
				}
				
				close(newsockfd);
				close(sockfd);
				exit(0);
			break;
			
		case 3: 				//Ver Eventos em tempo Real
				if(Simulacao_Em_Curso == true){
					system("clear");
					printf("┌──────────────────────────────────────────────────────────────┐\n");
					printf("│   Sistemas Operativos 2018 - 2019 ─ Simulacao de uma Loja    │\n");
					printf("└──────────────────────────────────────────────────────────────┘\n");
					printf("│                      Ver Eventos                             │\n");
					printf("┌──────────────────────────────────────────────────────────────┐\n");	
					printf("* Opcao 2 - Sair Simulacao		* Opcao 4 - Ver Estatisticas   \n");
					printf("                                                                \n");
				}
					else{
						printf("Erro: Nenhuma Simulacao Activa Neste Momento.\n");
					}
			break;
		
		case 4: 				//Funcao Para Mostrar as Estatisticas da Feira no Monitor:
				if(Simulacao_Em_Curso == true){ 
					Mostra_Estatisticas_Monitor(Total_Clientes_Idosos_Entraram_Fila_Loja, Total_Clientes_Idosos_Atendidos_Loja, Total_Clientes_Idosos_Desistiram_Fila_Loja, Total_Clientes_Adultos_Entraram_Fila_Loja, Total_Clientes_Adultos_Atendidos_Loja, Total_Clientes_Adultos_Desistiram_Fila_Loja, Total_Clientes_Sairam_Loja);
				}
				else{
					printf("Erro: Nenhuma Simulacao Activa Neste Momento. Selecione Outra Opcao. \n");
				}
			break;

		default: printf("Erro: Opcao Inserida Invalida. \n");
			break;
	}
}
//------------------------------------------------------------------------------------------------------------------------
int main(){

	Leitura_Ficheiro_Confg();
	int  servlen;
	struct sockaddr_un serv_addr;

		if ((sockfd= socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
			perror("Falha ao criar socket Cliente.n");
		}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;

	strcpy(serv_addr.sun_path, UNIXSTR_PATH);
	servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

		if (connect(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0){
			perror("Falha ao Estabelecer Conexao.\n");
		}

	system("clear");
	Menu_Interface();
	while(Opcao_Utilizador!=2){
		Menu_Principal();
	}
	return 0;
}
