// Chamada das bibliotecas.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unix.h"
#include <unistd.h>

//-----------------------------------------------------------------------------
// Declaração das Variáveis Globais
int Tempo_Medio_Chegadas = 0;
int Capacidade_Maxima_Loja = 0;
int Capacidade_Maxima_Fila_Loja = 0;
int tempo_simulacao = 0;
int tempo_serv_prod = 0;
int Percentagem_Ser_Cliente_Idoso = 0;
int Numero_Clientes_A_Criar = 0;
int Tempo_Maximo_Espera_Filas = 0;
int Tempo_Medio_Na_Loja = 0;

int Percentagem_Ser_ProdA = 0;
int Percentagem_Ser_ProdB = 0;
int Percentagem_Ser_ProdC = 0;

int quantidade_prodA = 10;
int quantidade_prodB = 10;
int quantidade_prodC = 10;
int tipo_produto;

int newsockfd;
int sockfd;
int Fecha_Loja = 0; //Quando Todos os Cliente Forem Embora, Acaba a Thread da loja.
int Valor_Semaforo_fila;
int Pessoas_No_Semaforo_fila = 0;
int IDEmpregado = 1;

//-----------------------------------------------------------------------------
//Funcao Que Recebe o Valor Inserido (Tempo Medio de Qualquer Coisa) no Ficheiro Config e Devolve Valor Random Baseado no Valor Lido
int Calcula_Tempos_Medios_Random(int Valor_Lido_Config){
	
	srand(time(NULL));
	int Desvio;
	int Random;
	int Valor_Lido_Mais_Um; 					//Para Ser, Por Exemplo, de 0 a 10 e nao de 0 a 9

		if(Valor_Lido_Config > 1){				//Se For Impar Arredondar Para Tras. Isto é, 1.5->1
			Desvio = Valor_Lido_Config/2; 		
			Valor_Lido_Mais_Um = Valor_Lido_Config+1;
			Random = (rand() %Valor_Lido_Mais_Um)+Desvio;
		}
			else if(Valor_Lido_Config == 1){	//Caso Especial Se For 1. Temos de Deixar o Random a 1
				Random = 1;
			}
	return Random;
}

//-----------------------------------------------------------------------------
//Funcao Que Define Tipo dos Clientes Baseado nas Percentagens Lidas do Ficheiro Config.
int Define_Tipo_Cliente_Random(int Percentagem_Ser_Cliente_Idoso){
	
	srand(time(NULL));
	int Faixa_Etaria = 0;
	int Random=(rand() %100)+1; 				//Cria Numero Random de 1 a 100

		if(Random <= Percentagem_Ser_Cliente_Idoso){
			Faixa_Etaria = 1;					// Idoso.
		}
			else{
				Faixa_Etaria=2;					// Adulto.
			}
	return Faixa_Etaria;
}

//-----------------------------------------------------------------------------
//Funcao Que Define Tipo Tipo de Produto dos Clientes Com base nas Percentagens Ligas do Ficheiro Config.
int Define_Tipo_Produto(int Percentagem_Ser_ProdA, int Percentagem_Ser_ProdB, int Percentagem_Ser_ProdC){
	
	srand(time(NULL));
	int produto = 0;
	int Random = (rand() %100)+1; 					// Cria Numero Random de 1 a 100

		if(Random <= Percentagem_Ser_ProdA){
			produto = 1;							// Produto ser A.
		}
			else if(Random <= Percentagem_Ser_ProdA + Percentagem_Ser_ProdB){
				produto = 2;						// Produto ser B.
			}
				else{								// Produto ser C.
					produto = 3;
				}
	return produto;
}

//-----------------------------------------------------------------------------
// Função que Defini a Probalidade de um cliente desistir.
int Define_Desistencia(int percentagem_desistencia){

	srand(time(NULL));
	int percentagem_des = 0;
	int Random=(rand() %100)+1; //Cria Numero Random de 1 a 100

	// Produto ser A
	if(Random<=percentagem_desistencia){
		percentagem_des=1;
	}
	return percentagem_des;
}
//-----------------------------------------------------------------------------
// Função "Random" que irá atribuir um empregado para o Cliente.
int Define_Empregado_Atendimento(int num_empregados){
		srand(time(NULL));
		int random1 = (rand()%num_empregados)+1;
	return random1;
}
//-----------------------------------------------------------------------------
//Funcao Que Escreve Mensagem Para Enviar Pela Socket 
void Escreve_Mensagem(int newsockfd, int ID_Cliente, int Estado, int Faixa_Etaria, int produto, int stock_prod, int empregados, int cliente_troca){

	char Buffer[MAX_LINE];
	int Tamanho_Mensagem;

	sprintf(Buffer,"%d %d %d %d %d %d %d ", ID_Cliente, Estado, Faixa_Etaria, produto, stock_prod, empregados, cliente_troca);
	Tamanho_Mensagem = strlen(Buffer)+1; // Add one character more because of \n

	if(write(newsockfd,Buffer,Tamanho_Mensagem) != Tamanho_Mensagem)
		perror("Erro ao escrever no cliente.\n");
}
//-----------------------------------------------------------------------------
//Funcao Para Ler do Ficheiro Config:
void Ler_Ficheiro_Config(){
	//Declaracao e Inicializacao de Variaveis Auxiliares da Funcao
	char Le_Linha[100];
	int Valor_Variavel_Lida;
	char Nome_Variavel_Lida[100];

	char Var1[]="Cap_Maxima_Loja"; 		//"Tempo_Medio_Chegadas";
	char Var2[]="Cap_Maxima_Fila";		// Capacidade Máxima que a Fila da Loja Pode Tomar.
	char Var3[]="Tempo_Sim"; 			// Tempo de Simulação.
	char Var4[]="tempo_serv_prod";		// Tempo Seriço por Produto.
	char Var5[]="Prob_Cliente_Idoso"; 	// Percentagem do Cliente ser Idoso.
	char Var6[]="Total_Clientes_Gerar"; // Total Clientes a Gerar.
	char Var7[]="Tempo_Maximo_Espera_Filas";// Tempo Máximo de Espera na Fila.
	char Var8[]="Tempo_Medio_Na_Loja";	// Tempo Médio dos Clientes Permancer na Loja.
	char Var9[]="Tempo_Medio_Chegadas";// Tempo Médio de Chegada dos Clientes.
	char Var10[]="Prob_Prod_A"; 		// Percentagem do Cliente ser ProdutoA.
	char Var11[]="Prob_Prod_B"; 		// Percentagem do Cliente ser ProdutoB.
	char Var12[]="Prob_Prod_C"; 		// Percentagem do Cliente ser ProdutoC.
	
	int Erro=0; //Se Uma Das Restricoes For Violada, a Variavel Passa Para 1

	FILE *fp;
	fp=fopen("config.conf","r"); //Vai Abrir o Ficheiro Config.conf
	
		if(fp!=NULL){
			//Ciclo de Leitura:
			while(fgets(Le_Linha,100,fp)!=NULL){
	
			sscanf(Le_Linha, "%s = %i", Nome_Variavel_Lida, &Valor_Variavel_Lida);

			if(strcmp(Nome_Variavel_Lida,Var1) == 0){
				Capacidade_Maxima_Loja = Valor_Variavel_Lida;
			}
				
				else if(strcmp(Nome_Variavel_Lida,Var2) == 0){
						Capacidade_Maxima_Fila_Loja = Valor_Variavel_Lida;
				}
				
				else if(strcmp(Nome_Variavel_Lida,Var3) == 0){
						tempo_simulacao = Valor_Variavel_Lida;
				}

				else if(strcmp(Nome_Variavel_Lida,Var4) == 0){
					tempo_serv_prod = Valor_Variavel_Lida;
				}

				else if(strcmp(Nome_Variavel_Lida,Var5) == 0){
					Percentagem_Ser_Cliente_Idoso = Valor_Variavel_Lida;
				}

				else if(strcmp(Nome_Variavel_Lida,Var6) == 0){
					Numero_Clientes_A_Criar = Valor_Variavel_Lida;
				}

				else if(strcmp(Nome_Variavel_Lida,Var7) == 0){
					Tempo_Maximo_Espera_Filas = Valor_Variavel_Lida;
				}

				else if(strcmp(Nome_Variavel_Lida,Var8) == 0){
					Tempo_Medio_Na_Loja = Valor_Variavel_Lida;
				}

				else if(strcmp(Nome_Variavel_Lida,Var9) == 0){
					Tempo_Medio_Chegadas = Valor_Variavel_Lida;
				}

				else if(strcmp(Nome_Variavel_Lida,Var10) == 0){
					Percentagem_Ser_ProdA = Valor_Variavel_Lida;
				}

				else if(strcmp(Nome_Variavel_Lida,Var11) == 0){
					Percentagem_Ser_ProdB = Valor_Variavel_Lida;
				}
				else if(strcmp(Nome_Variavel_Lida,Var12) == 0){
					Percentagem_Ser_ProdC = Valor_Variavel_Lida;
				}
			}
			fclose(fp);
		}

			else{
				printf("Falha ao Abrir Ficheiro Configuracao.");
			}
// *************************** Erros (Restrições) ***************************

	if(Tempo_Medio_Chegadas <= 0){
		printf("Erro: Tempo Medio de Chegadas Tem de Ser maior que 0. \n");
		Erro = 1;
	};
	
	if(tempo_simulacao <2 ){
		printf("Erro: Tempo de Simulacao Tem de Ser maior que 2. \n");
		Erro = 1;
	};
	
	if(Capacidade_Maxima_Loja <= 0){
		printf("Erro: Capacidade Maxima da Loja Tem de Ser maior que 0. \n");
		Erro = 1;
	};
	
	if(Tempo_Maximo_Espera_Filas < 0){
		printf("Erro: Tempo Maximo de Espera nas Filas Tem de Ser maior ou igual a 0. \n");
		Erro = 1;
	};
	
	if(Percentagem_Ser_Cliente_Idoso < 0 || Percentagem_Ser_Cliente_Idoso > 100){
		printf("Erro: Percentagem de Ser Cliente Idoso Tem de Ser >0%% e <=100%%. \n");
		Erro = 1;
	};
	
	if(Numero_Clientes_A_Criar <= 0 || Numero_Clientes_A_Criar > 9999){
		printf("Erro: Numero de Clientes a Criar Tem de Ser maior que 0 e menor que 10000. \n");
		Erro = 1;
	};
	
	if(Tempo_Medio_Na_Loja <= 0){
		printf("Erro: Tempo Medio na Loja Tem de Ser maior que 0. \n");
		Erro = 1;
	};	
		if(Capacidade_Maxima_Fila_Loja <= 0){
		printf("Erro: Capacidade Maxima da Fila na Loja Tem de Ser maior que 0. \n");
		Erro = 1;
	};	
	//Se Alguma das Restricoes Anteriores Acontecer, Programa Termina
	if(Erro == 1){
		exit(0);
	};
}

//-----------------------------------------------------------------------------
//Estrutura de Dados dos Clientes
typedef struct{
	int Estado_Seguinte;  
	int ID_Cliente; 
	int Faixa_Etaria; 
	int Tempo_Espera_Maximo_Filas;
	int Tempo_Estadia_Loja; 
	int Lugar_Fila_Loja;
	int Tempo_Fica_Zona;
	int produto;
	int Momento_Entrada_Fila_Loja;
	int Momento_Vai_Desistir_Fila_Loja;
	int empregado;
}

//-----------------------------------------------------------------------------
Struct_Clientes;

	int Clientes_Criados = 0;
	int Tempo_Simulacao_Actual = 0; //Guarda o Instante Actual da Simulacao. Comeca a 0. Vai Incrementando Ate Acabar a Simulacao 

Struct_Clientes Cliente[10000]; //[Numero_Clientes_A_Criar]; ->Numero de Structs de Clientes Criadas

//Declaracao Trincos:
pthread_mutex_t Trinco_Comunicacao_Socket; //Trinco Que Impede Atribuir Instrucoes a Varios Clientes ao Mesmo Tempo

//Declaracao Threads:
pthread_t Thread_Ciclo_Tempo_Simulacao;
pthread_t Thread_Cliente[10000]; //[Numero_Clientes_A_Criar]; ->Numero de Tarefas Criadas


//		Declaracao Semaforos:f
sem_t Lotacao_Loja;
sem_t Fila_Loja;

int Clientes_AD_Fila_Loja=0;
int Clientes_ID_Fila_Loja=0;
int num_empregados = 1;

//-----------------------------------------------------------------------------
void Cria_Empregados(){
	
	int Total_Clientes = Clientes_ID_Fila_Loja + Clientes_AD_Fila_Loja;
	int Divisao_Clientes;
		
		Divisao_Clientes = Total_Clientes / 2;
		num_empregados = Divisao_Clientes + 1;
		//	if(Total_Clientes == 0){
		//		num_empregados = 1;
	//}
}
// Prioridades dos Clientes (Idoso - Adulto)
void Aumenta_Tipo_Cliente_Fila_Loja(int Faixa_Etaria){
	
	if(Faixa_Etaria == 1){				// Idoso.
		Clientes_ID_Fila_Loja++;
	}
		else if(Faixa_Etaria == 2){		// Adultos.
			Clientes_AD_Fila_Loja++;
		}
}

//-----------------------------------------------------------------------------
//Diminui o Numero de Clientes na Fila Dependendo do Seu Tipo
void Diminui_Tipo_Cliente_Fila_Loja(int Faixa_Etaria){
	
	if(Faixa_Etaria == 1){				// Idoso
		Clientes_ID_Fila_Loja--;
	}
		else if(Faixa_Etaria == 2){		// Adulto
			Clientes_AD_Fila_Loja--;
		}
}
//-----------------------------------------------------------------------------
//Quando o Cliente Chega à Fila da Loja É Verificado o Tipo do Cliente Que Chegou (Idoso ou Adulto).
void Atribui_Lugar_Fila_Loja_A_Cliente(int ID_Cliente){
	
	int i;
	
	if(Cliente[ID_Cliente].Faixa_Etaria == 1){		//Se o Cliente Que Chegou for Idoso.
		//Clientes_ID_Fila_Loja++;
		Cliente[ID_Cliente].Lugar_Fila_Loja = Clientes_ID_Fila_Loja; 			// Vai para último lugar da fila dos Idosos.

		for(i = 0; i < Clientes_Criados; i++){
			if(Cliente[i].Faixa_Etaria != 1 && Cliente[i].Lugar_Fila_Loja > 0){ // Vai para a fila normal da loja.
				Cliente[i].Lugar_Fila_Loja++;
			}
		}
	}
		else if(Cliente[ID_Cliente].Faixa_Etaria == 2){	// Se o Cliente que chegou for Adulto.	
			//Clientes_AD_Fila_Loja++;
			Cliente[ID_Cliente].Lugar_Fila_Loja = Clientes_ID_Fila_Loja + Clientes_AD_Fila_Loja; //Vai Para o Ultimo Lugar da Fila Composta Pela Soma de Todas as Outras "Sub Filas MR, PG e Cri"
		}
}
//-----------------------------------------------------------------------------
//Funcao Que Faz o Cliente Recebido (1º da Fila) Sair da Fila e Entrar na Zona. E Todos Avancam 1 Posicao
void Avanca_Um_Lugar_Fila_Loja(){
	
	int i;
	
	for(i = 0; i < Clientes_Criados; i++){
		if(Cliente[i].Lugar_Fila_Loja > 0){					// Se está na fila da loja.
				Cliente[i].Lugar_Fila_Loja--;
		}
	}
}
//-----------------------------------------------------------------------------
//Funcao Que Faz o Cliente Recebido Desistir da Fila e Todos Atras Dele Avancam 1 Posicao
void Desiste_Fila_E_Avancam_Um_Lugar_Fila_Loja(int ID_Cliente){
	
	int i;
	Cliente[ID_Cliente].Lugar_Fila_Loja = 0; 				// Cliente Que Desiste Sai da Fila (Poiscao=0)

	for(i = 0; i < Clientes_Criados; i++){ 					// Todos os Clientes Atras Dele Avancam 1 Posicao
		
		if(Cliente[i].Lugar_Fila_Loja > 0){ 				// Se Esta na Fila da Loja.
			Cliente[i].Lugar_Fila_Loja--;
		}
	}
}
//-----------------------------------------------------------------------------
void Gestor_Eventos_Clientes(int ID_Cliente_Recebido){

	Cliente[ID_Cliente_Recebido].ID_Cliente = ID_Cliente_Recebido;
	Cliente[ID_Cliente_Recebido].produto = Define_Tipo_Produto(Percentagem_Ser_ProdA,Percentagem_Ser_ProdB,Percentagem_Ser_ProdC);
	Cliente[ID_Cliente_Recebido].Faixa_Etaria = Define_Tipo_Cliente_Random(Percentagem_Ser_Cliente_Idoso);
	Cliente[ID_Cliente_Recebido].Tempo_Espera_Maximo_Filas = Tempo_Maximo_Espera_Filas; //Variavel Lida do Config. //Tirar em Principio da Mensagem
	Cliente[ID_Cliente_Recebido].Tempo_Estadia_Loja = Calcula_Tempos_Medios_Random(Tempo_Medio_Na_Loja);//Tirar em Principio da Mensagem
	Cliente[ID_Cliente_Recebido].Lugar_Fila_Loja = 0;
	Cliente[ID_Cliente_Recebido].Momento_Vai_Desistir_Fila_Loja = 0;					//novo
	
	//Tempo Que Vai Ficar na Zona:
	Cliente[ID_Cliente_Recebido].Tempo_Fica_Zona = Calcula_Tempos_Medios_Random(Tempo_Medio_Na_Loja);
	Cliente[ID_Cliente_Recebido].empregado = Define_Empregado_Atendimento(num_empregados);

	int Tempo_Actual;
	//Variavel Que Guarda Se Houve Desistencia ou Nao
	int Desistencia_Fila_Loja = 0;
	Cria_Empregados();
	
//##############################################################################################################
	// Caso 5:					CLIENTE ENTRA NA LOJA.
	// Semáforo Para Controlar o total de clientes que estão dentro da loja.
	sem_wait(&Lotacao_Loja);
	pthread_mutex_lock(&Trinco_Comunicacao_Socket);
	Escreve_Mensagem(newsockfd, Cliente[ID_Cliente_Recebido].ID_Cliente, 5, Cliente[ID_Cliente_Recebido].Faixa_Etaria, Cliente[ID_Cliente_Recebido].produto, 10, Cliente[ID_Cliente_Recebido].empregado, 0);
	pthread_mutex_unlock(&Trinco_Comunicacao_Socket);
	
//##############################################################################################################
	//case 1:					CLIENTE ENTRA NA FILA DA LOJA.
	sem_wait(&Fila_Loja);
	sleep(1);
	pthread_mutex_lock(&Trinco_Comunicacao_Socket);
	//--------------------------------------------------------------------------
	Cliente[ID_Cliente_Recebido].Momento_Entrada_Fila_Loja = Tempo_Simulacao_Actual;
	Tempo_Actual=Tempo_Simulacao_Actual;
	Tempo_Actual=Cliente[ID_Cliente_Recebido].Momento_Entrada_Fila_Loja; //Nao va a Hora Mudar Entre Estas 2 Instrucoes e Haver Diferencas.
	//--------------------------------------------------------------------------
	//Define o momento de Desistencia:
	Cliente[ID_Cliente_Recebido].Momento_Vai_Desistir_Fila_Loja=Cliente[ID_Cliente_Recebido].Momento_Entrada_Fila_Loja+Cliente[ID_Cliente_Recebido].Tempo_Espera_Maximo_Filas;

	//Aumenta o Numero de Clientes n Fila Dependendo do Seu Tipo
	Aumenta_Tipo_Cliente_Fila_Loja(Cliente[ID_Cliente_Recebido].Faixa_Etaria);
	sleep(1);
	//pthread_mutex_lock(&Trinco_Comunicacao_Socket);
	Escreve_Mensagem(newsockfd, Cliente[ID_Cliente_Recebido].ID_Cliente, 1, Cliente[ID_Cliente_Recebido].Faixa_Etaria, Cliente[ID_Cliente_Recebido].produto, 10, Cliente[ID_Cliente_Recebido].empregado, 0);
	//pthread_mutex_unlock(&Trinco_Comunicacao_Socket);
	sleep(1);
	Atribui_Lugar_Fila_Loja_A_Cliente(Cliente[ID_Cliente_Recebido].ID_Cliente);
	pthread_mutex_unlock(&Trinco_Comunicacao_Socket);

	while(Cliente[ID_Cliente_Recebido].Lugar_Fila_Loja != 1 && Desistencia_Fila_Loja != 1){//Enquanto o Cliente da Tarefa Nao For 1º da Fila, Espera	
		if(Tempo_Simulacao_Actual >= Cliente[ID_Cliente_Recebido].Momento_Vai_Desistir_Fila_Loja)
		{
			
//##############################################################################################################
	//case 3:					CLIENTE DESISTE FILA DA LOJA
	sem_post(&Fila_Loja);
	pthread_mutex_lock(&Trinco_Comunicacao_Socket);
	//sem_post(&Fila_Loja);
	//Diminui o Numero de Clientes na Fila Dependendo do Seu Tipo
	Diminui_Tipo_Cliente_Fila_Loja(Cliente[ID_Cliente_Recebido].Faixa_Etaria);
	Escreve_Mensagem(newsockfd, Cliente[ID_Cliente_Recebido].ID_Cliente, 3, Cliente[ID_Cliente_Recebido].Faixa_Etaria, Cliente[ID_Cliente_Recebido].produto, 10, Cliente[ID_Cliente_Recebido].empregado, 0);
	sleep(1);
	Desiste_Fila_E_Avancam_Um_Lugar_Fila_Loja(Cliente[ID_Cliente_Recebido].ID_Cliente);
	pthread_mutex_unlock(&Trinco_Comunicacao_Socket);

	//Se Cliente Desistir, Activa Sinal Para Indicar Que Nao Vai Haver Entrada na Zona em Questao nem Saida
	Desistencia_Fila_Loja = 1;	
		}	
	}			// Fim do ciclo While.
	if(Desistencia_Fila_Loja == 0){
		
//##############################################################################################################
		//case 2: 					CLIENTE É ATENDIDO
		sem_post(&Fila_Loja);
		pthread_mutex_lock(&Trinco_Comunicacao_Socket);
		//sem_post(&Fila_Loja);
		//Diminui o Numero de Clientes na Fila Dependendo do Seu Tipo
		Diminui_Tipo_Cliente_Fila_Loja(Cliente[ID_Cliente_Recebido].Faixa_Etaria);
		//sem_post(&Lotacao_Loja);
		//*********************************************************************************************
		if(Cliente[ID_Cliente_Recebido].produto == 1){				// Produto A.
			quantidade_prodA--;
			if (quantidade_prodA <3){
				Escreve_Mensagem(newsockfd,Cliente[ID_Cliente_Recebido].ID_Cliente, 8, Cliente[ID_Cliente_Recebido].Faixa_Etaria, 1, 10, Cliente[ID_Cliente_Recebido].empregado, 0);
				sleep(1);
				quantidade_prodA = 10;
			}
			//sleep(2);
			Escreve_Mensagem(newsockfd, Cliente[ID_Cliente_Recebido].ID_Cliente, 2, Cliente[ID_Cliente_Recebido].Faixa_Etaria, Cliente[ID_Cliente_Recebido].produto, 10, Cliente[ID_Cliente_Recebido].empregado, 0);
		}
		//*********************************************************************************************
			else if(Cliente[ID_Cliente_Recebido].produto == 2){		// Produto B.
				quantidade_prodB--;
					if (quantidade_prodB <3){
						Escreve_Mensagem(newsockfd,Cliente[ID_Cliente_Recebido].ID_Cliente, 9, Cliente[ID_Cliente_Recebido].Faixa_Etaria,  1, 10, Cliente[ID_Cliente_Recebido].empregado, 0);
						sleep(1);
						quantidade_prodB = 10;
					}
					//sleep(2);
				Escreve_Mensagem(newsockfd, Cliente[ID_Cliente_Recebido].ID_Cliente, 6, Cliente[ID_Cliente_Recebido].Faixa_Etaria, Cliente[ID_Cliente_Recebido].produto, 10,Cliente[ID_Cliente_Recebido].empregado, 0);
			}
		//*********************************************************************************************
				else if(Cliente[ID_Cliente_Recebido].produto == 3){		// Produto C.
					quantidade_prodC--;
					if (quantidade_prodC < 3){
						Escreve_Mensagem(newsockfd,Cliente[ID_Cliente_Recebido].ID_Cliente, 10, Cliente[ID_Cliente_Recebido].Faixa_Etaria,  1, 10, Cliente[ID_Cliente_Recebido].empregado, 0);
						sleep(1);
						quantidade_prodC = 10;
					}
				//sleep(2);
				Escreve_Mensagem(newsockfd, Cliente[ID_Cliente_Recebido].ID_Cliente, 7, Cliente[ID_Cliente_Recebido].Faixa_Etaria, Cliente[ID_Cliente_Recebido].produto, 10,Cliente[ID_Cliente_Recebido].empregado, 0);
				}
		
			sleep(1);
			Avanca_Um_Lugar_Fila_Loja();			//int ID_Cliente);
			pthread_mutex_unlock(&Trinco_Comunicacao_Socket);
			srand(time(NULL));
		}								// Fim do If se Houve Desistencia.
//##############################################################################################################
		//case 4: 					CLIENTE SAI DA LOJA
		pthread_mutex_lock(&Trinco_Comunicacao_Socket);
		Escreve_Mensagem(newsockfd, Cliente[ID_Cliente_Recebido].ID_Cliente, 4, Cliente[ID_Cliente_Recebido].Faixa_Etaria, Cliente[ID_Cliente_Recebido].produto, 10, Cliente[ID_Cliente_Recebido].empregado, 0);
		sleep(1);
		pthread_mutex_unlock(&Trinco_Comunicacao_Socket);	
		sleep(1);
		sem_post(&Lotacao_Loja);
}
//------------------------------------------------------------------------------------------------------
//Vai Criar Clientes Ate Alcancar o Tempo de Simulacao OU Criar Todos os Clientes Definidos
void Criacao_Clientes()
{
   	srand(time(NULL));
	long Clientes_Criados2 = 0;

	while(Tempo_Simulacao_Actual<tempo_simulacao && Clientes_Criados <= Numero_Clientes_A_Criar){
		
		if(pthread_create(&(Thread_Cliente[Clientes_Criados]), NULL ,(void *)&Gestor_Eventos_Clientes,(void *)Clientes_Criados2) != 0){
			printf("Erro na criacao da tarefa de criacao dos clientes\n");
			exit(1);
		}	
		
		sleep(Calcula_Tempos_Medios_Random(Tempo_Medio_Chegadas));
		Clientes_Criados++;			//Variavel Global
		Clientes_Criados2++;		//Variavel Local ->Senao, Para Eliminar Esta Variavel, Talvez Por a Variavel Global Como Long!	
	}
}

//------------------------------------------------------------------------------------------------------
//Relogio Funciona Ate Todos os Clientes Sairem
void Funcao_Incrementa_Tempo_Actual(){
	
	while(Fecha_Loja != 1){
		Tempo_Simulacao_Actual = Tempo_Simulacao_Actual+1;
		sleep(1);
	}
}
//------------------------------------------------------------------------------------------------------
//Quando Recebe Mensagem do Monitor Com Sinal Para Comecar a Simulacao
void Comeca_Simulacao(){
	
	if(pthread_create(&(Thread_Ciclo_Tempo_Simulacao), NULL ,(void *)&Funcao_Incrementa_Tempo_Actual, NULL) != 0){
		printf("Falha na Criacao da tarefa do ciclo tempo.\n");
		exit(1);
	}	
}
//------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	
	// Leitura do ficheiro configuração.
	Ler_Ficheiro_Config();
	pthread_mutex_init(&Trinco_Comunicacao_Socket, NULL);
	
	//Semaforos Para o Gestor da Loja:
	sem_init(&Lotacao_Loja,0,Capacidade_Maxima_Loja);
	sem_init(&Fila_Loja,0,Capacidade_Maxima_Fila_Loja);
	
	
	int clilen, servlen,i;
	struct sockaddr_un cli_addr, serv_addr;

	// Cria Socket 
	if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0)) < 0)
		perror("Falha ao criar socket stream servidor.\n");
	
	unlink(UNIXSTR_PATH);
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, UNIXSTR_PATH);
	servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
	
	if (bind(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0)	
		perror("Falha ao atribuir nome socket servidor.\n");
	
	listen(sockfd, 1);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	
	if (newsockfd < 0) 
		perror("Falha ao criar ligacao dedicada.\n");
	
	printf("Pressione SPACE Para Comecar Simulacao. \n");
	
	char Tecla=0;
	
		while (Tecla!=' ') {
			Tecla=getchar()
		;}

	//Comeca Simulacao
	Comeca_Simulacao();
	Criacao_Clientes();
	sleep(2);


	int qwe;
	
		for(qwe = 0; qwe < Clientes_Criados; qwe++){
			pthread_join(Thread_Cliente[qwe],NULL);
		}

	printf("Acabou Join Threads dos Clientes\n");
	sleep(2);
	//Quando Termina Todas as Threads Criadas, a Loja Comeca a Fechar
	Fecha_Loja = 1;
	pthread_join(Thread_Ciclo_Tempo_Simulacao,NULL);
	printf("Acabou Join Thread do Ciclo do Tempo de Simulacao.\n");
	sleep(2);

	//Mensagem de Fim de Simulacao
	pthread_mutex_lock(&Trinco_Comunicacao_Socket);
	Escreve_Mensagem(newsockfd,0,50,0,0,0,0,0);
	printf("Enviou Mensagem Fim Simulacao!\n");
	pthread_mutex_unlock(&Trinco_Comunicacao_Socket);
	sleep(2);

	//Fechar o Socket 
	close(newsockfd);
	return 0;
	
	
}

