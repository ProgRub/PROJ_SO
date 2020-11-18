
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

int Fecha_Feira=0; //Quando Todos os Cliente Forem Embora, Acaba a Thread do Gestor do Carroussel

int Valor_Semaforo_MontanhaRussa;

int Aux_MontanhaRussa=2;

int Pessoas_No_Semaforo_MontanhaRussa=0;




//Funcao Que Recebe o Valor Inserido (Tempo Medio de Qualquer Coisa) no Ficheiro Config e Devolve Valor Random Baseado no Valor Lido
int Calcula_Tempos_Medios_Random(int Valor_Lido_Config)
{
	srand(time(NULL));

	int Desvio;
	int Random;
	int Valor_Lido_Mais_Um; //Para Ser, Por Exemplo, de 0 a 10 e nao de 0 a 9

	if(Valor_Lido_Config>1)
	{
		Desvio=Valor_Lido_Config/2; //Se For Impar Arredondar Para Tras. Isto é, 1.5->1

		Valor_Lido_Mais_Um=Valor_Lido_Config+1;
		
		Random=(rand() %Valor_Lido_Mais_Um)+Desvio;
	}
	else if(Valor_Lido_Config==1) //Caso Especial Se For 1. Temos de Deixar o Random a 1
	{
		Random=1;
	}

/*
Exemplo 1: Para Valor 10)
0 a 10 -> +5(=Desvio) -> 5 a 15 <=> Valor_Lido_Menos_Desvio a Valor_Lido_Mais_Desvio

Exemplo 2: Para Valor 1)
0 a 1 -> +0(=Desvio) -> 0 a 1 <=> Existe Hipotese de Nada Acontecer. Ou Seja, Se Valor_Lido_Config For 1, o Random Tem de Ser 1.
*/


return Random;
}



//Funcao Que Define Idade do Cliente
int Define_Idade_Cliente(int Tipo_Cliente)
{
	srand(time(NULL));

	//Para Entrar Na Feira Tem de Ser >=2 e <=80
	
	//Crianca: Dos 2 aos 17
	//PG: Dos 18 aos 80
	//MR: Qualquer Idade

	int Random;

	if(Tipo_Cliente==2) //Se For Crianca (>=2 e <18)
	{
		Random=(rand() %16) + 2; //Cria Numero Random de 2 a 17
	}
	else if(Tipo_Cliente==3) //Se For Publico Geral (>=18)
	{
		Random=(rand() %63) + 18; //Cria Numero Random de 18 a 80
	}
	else if(Tipo_Cliente==1) //Se For Mobilidade Reduzida (Pode Ser Qualquer Idade)
	{
		Random=(rand() %73) + 8; //Cria Numero Random de 8 a 80
	}

	return Random;
}



int Fica_Ou_Nao_Bar(int Percentagem_Ficar_Bar)
{
	srand(time(NULL));

	int Random=(rand() %100)+1; //Cria Random de 1 a 100
	int Fica;

	if(Random<=Percentagem_Ficar_Bar)
	{
		Fica=1;
	}
	else
	{
		Fica=0;
	}

return Fica;

}


//Funcao Que Define Tipo dos Clientes Baseado nas Percentagens Lidas do Ficheiro Config
int Define_Tipo_Cliente_Random(int Percentagem_Ser_Cliente_Crianca, int Percentagem_Ser_Cliente_Mobilidade_Reduzida)
{
srand(time(NULL));

	int Tipo_Cliente=0;

	int Random=(rand() %100)+1; //Cria Numero Random de 1 a 100
	
	/* Exemplo Funcionamento:
	Prob Cri=5%->1,2,3,4,5
	Prob MR=10%->6,7,8,9,10,11,12,13,14,15

	Random<=Prob Cri  Ou seja, Random<=5
	((Random>Prob Cri) && (Random<=(Prob Cri+Prob MR))) Ou Seja, Random>=6 e <=15 */

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


//Funcao Que Escreve Mensagem Para Enviar Pela Socket 
void Escreve_Mensagem(int newsockfd, int ID_Cliente, int Tipo_Cliente, int Estado, int Hora_Actual, int Idade, int Hora_Entrada_Fila_Bar, int Hora_Desistencia_Fila_Bar, int Hora_Atendimento_Bar, int Hora_Entrada_Fila_Carroussel, int Hora_Desistencia_Fila_Carroussel, int Hora_Entrada_Carroussel, int Hora_Entrada_Fila_MontanhaRussa, int Hora_Desistencia_Fila_MontanhaRussa, int Hora_Andou_MontanhaRussa, int Hora_Entrada_Filas_Bilheteiras, int Hora_Desistencia_Filas_Bilheteiras, int Hora_Entrada_Feira, int Hora_Saida_Feira)
{

	char Buffer[MAX_LINE];
	int Tamanho_Mensagem;

	sprintf(Buffer,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", ID_Cliente, Tipo_Cliente, Estado,Hora_Actual,Idade,Hora_Entrada_Fila_Bar, Hora_Desistencia_Fila_Bar, Hora_Atendimento_Bar, Hora_Entrada_Fila_Carroussel, Hora_Desistencia_Fila_Carroussel, Hora_Entrada_Carroussel, Hora_Entrada_Fila_MontanhaRussa, Hora_Desistencia_Fila_MontanhaRussa, Hora_Andou_MontanhaRussa, Hora_Entrada_Filas_Bilheteiras, Hora_Desistencia_Filas_Bilheteiras, Hora_Entrada_Feira,Hora_Saida_Feira); // Writes in simulator socket buffer a message to monitor

	Tamanho_Mensagem=strlen(Buffer)+1; // Add one character more because of \n

	if(write(newsockfd,Buffer,Tamanho_Mensagem)!=Tamanho_Mensagem)
		perror("Erro ao escrever no cliente\n");
}


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
	char Var12[]="T_Med_Fica_Feira";//"Tempo_Medio_Estadia_Cliente_Na_Feira";

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
				Tempo_Medio_Estadia_Cliente_Na_Feira=Valor_Variavel_Lida;
			}
		}
		fclose(fp);
	}

	else
	{
		printf("Erro Abertura Ficheiro");
	}

	//------------------------------------------------------------------------------------------//
	//--------------------------  Restricoes: --------------------------------------------------//
	//------------------------------------------------------------------------------------------//

	if(Tempo_Medio_Chegadas<=0)
	{
		printf("Erro: Tempo_Medio_Chegadas Tem de Ser >0 \n");
		Erro=1;
	};
	if(Tempo_Simulacao<2)
	{
		printf("Erro: Tempo_Simulacao Tem de Ser >2 \n");
		Erro=1;
	};
	if(Tamanho_Maximo_Bar<=0)
	{
		printf("Erro: Tamanho_Maximo_Bar Tem de Ser >0 \n");
		Erro=1;
	};
	if(Tamanho_Maximo_Carroussel<=0)
	{
		printf("Erro: Tamanho_Maximo_Carroussel Tem de Ser >0 \n");
		Erro=1;
	};
	if(Tamanho_Maximo_Feira<=0)
	{
		printf("Erro: Tamanho_Maximo_Feira Tem de Ser >0 \n");
		Erro=1;
	};
	if(Tempo_Maximo_Espera_Filas<0)
	{
		printf("Erro: Tempo_Maximo_Espera_Filas Tem de Ser >=0 \n");
		Erro=1;
	};
	if(Percentagem_Ser_Cliente_Crianca<0 || Percentagem_Ser_Cliente_Crianca>100)
	{
		printf("Erro: Percentagem_Ser_Cliente_Crianca Tem de Ser >0%% e <=100%% \n");
		Erro=1;
	};
	if(Percentagem_Ser_Cliente_Mobilidade_Reduzida<0 || Percentagem_Ser_Cliente_Mobilidade_Reduzida>100)
	{
		printf("Erro: Percentagem_Ser_Cliente_Mobilidade_Reduzida Tem de Ser >0%% e <=100%% \n");
		Erro=1;
	};
	if(Numero_Clientes_A_Criar<=0 || Numero_Clientes_A_Criar>9999) //10000->Limite Maximo Inserido das Struct e das Threads Clientes
	{
		printf("Erro: Numero_Clientes_A_Criar Tem de Ser >0 e <10000 \n");
		Erro=1;
	};
	if(Tempo_Medio_Fica_Zonas<=0)
	{
		printf("Erro: Tempo_Medio_Fica_Zonas Tem de Ser >0 \n");
		Erro=1;
	};
	if(Percentagem_Ficar_Bar<0 || Percentagem_Ficar_Bar>100)
	{
		printf("Erro: Percentagem_Ficar_Bar Tem de Ser >0%% e <=100%% \n");
		Erro=1;
	};
	if(Tempo_Medio_Estadia_Cliente_Na_Feira<=0)
	{
		printf("Erro: Tempo_Medio_Estadia_Cliente_Na_Feira Tem de Ser >0 \n");
		Erro=1;
	};



	//Ultima Restricao: Se a Soma das Percentagens do Tipo de Clientes For >100
	if(Percentagem_Ser_Cliente_Crianca+Percentagem_Ser_Cliente_Mobilidade_Reduzida>100)
	{
		printf("Erro: Soma Percentagens Tipo Clientes Nao Pode Ser >100%% \n");
		Erro=1;
	};
	//Se Alguma das Restricoes Anteriores Acontecer, Programa Termina
	if(Erro==1)
	{
		exit(0);
	};
}


//Estrutura de Dados dos Clientes
typedef struct{
	int Estado_Seguinte;  

	int ID_Cliente; 
	int Tipo_Cliente; 
	int Tempo_Espera_Maximo_Filas;
	int Tempo_Estadia_Feira; 

	int Hora_Entrada_Fila_Bar;
	int Hora_Desistencia_Fila_Bar;
	int Hora_Atendimento_Bar;

	int Hora_Entrada_Fila_Carroussel;
	int Hora_Desistencia_Fila_Carroussel;
	int Hora_Entrada_Carroussel;

	int Hora_Entrada_Fila_MontanhaRussa;
	int Hora_Desistencia_Fila_MontanhaRussa;
	int Hora_Andou_MontanhaRussa;

	int Hora_Entrada_Filas_Bilheteiras;
	int Hora_Desistencia_Filas_Bilheteiras;
	int Hora_Entrada_Feira;
	int Hora_Saida_Feira;

	int Lugar_Fila_Bilheteiras;
	int Lugar_Fila_Bar;
	int Lugar_Fila_Carroussel;
	int Lugar_Fila_MontanhaRussa;

	int Idade;

	int Hora_Vai_Desistir_Fila_Bilheteiras;
	int Hora_Vai_Desistir_Fila_Bar;
	int Hora_Vai_Desistir_Fila_Carroussel;
	int Hora_Vai_Desistir_Fila_MontanhaRussa;

	int Tempo_Fica_Zona;

} Struct_Clientes;


int Clientes_Criados=0;

int Tempo_Simulacao_Actual=0; //Guarda o Instante Actual da Simulacao. Comeca a 0. Vai Incrementando Ate Acabar a Simulacao 

Struct_Clientes Cliente[10000]; //[Numero_Clientes_A_Criar]; ->Numero de Structs de Clientes Criadas

//Declaracao Trincos:
pthread_mutex_t Trinco_MontanhaRussa;
pthread_mutex_t Trinco_Comunicacao_Socket; //Trinco Que Impede Atribuir Instrucoes a Varios Clientes ao Mesmo Tempo

//Declaracao Threads:
pthread_t Thread_Ciclo_Tempo_Simulacao;
pthread_t Thread_Cliente[10000]; //[Numero_Clientes_A_Criar]; ->Numero de Tarefas Criadas
pthread_t Thread_Gestor_Carroussel; //Definicao da Thread do Gestor do Carroussel
pthread_t Thread_Gestor_MontanhaRussa;

//		Declaracao Semaforos:
//Semaforos Feira:
sem_t Semaforo_Lotacao_Feira;

//Semaforos Bar:
sem_t Semaforo_Lotacao_Bar;

//Semaforos Carroussel:
//sem_t Semaforo_Lotacao_Carrousel; Retirado. Thread do Gestor Trata Disto Agora
//sem_t Semaforo_Espera_Cliente_Chegue;
//sem_t Semaforo_Deixa_Cliente_Entrar;

sem_t Espera_Cliente;
sem_t Espera_Sinal_Entrada;
sem_t Lotacao_Carroussel;
sem_t Semaforo_So_1_Cliente;


//Semaforos Montanha Russa:
sem_t Semaforo_Desce_MontanhaRussa;

int Clientes_A_Andar_MontanhaRussa=0;



//Prioridades Fila Bilheteiras:
int Clientes_MR_Fila_Bilheteiras=0;
int Resto_Clientes_Fila_Bilheteiras=0;

//Prioridades Fila Bar:
int Clientes_MR_Fila_Bar=0;
int Clientes_PG_Fila_Bar=0;
int Clientes_Cri_Fila_Bar=0;

//Prioridades Fila Montanha Russa:
//- Nao Tem, Pois Nao Importa o Tipo de Cliente. Apenas Tem de Descer aos Pares
int Clientes_Fila_MontanhaRussa=0;

//Prioridades Fila Carroussel:
int Clientes_Cri_Fila_Carroussel=0;
int Clientes_MR_Fila_Carroussel=0;
int Clientes_PG_Fila_Carroussel=0;
int Clientes_Cri_Pequenas_Fila_Carroussel=0;

////////////////////////////////////// BAR //////////////////////////////////////////////

//Aumenta o Numero de Clientes na Fila Dependendo do Seu Tipo
void Aumenta_Tipo_Cliente_Fila_Bilheteiras(int Tipo_Cliente)
{
	if(Tipo_Cliente==1)
	{
		Clientes_MR_Fila_Bilheteiras++;
	}
	else
	{
		Resto_Clientes_Fila_Bilheteiras++;
	}
}


//Diminui o Numero de Clientes na Fila Dependendo do Seu Tipo
void Diminui_Tipo_Cliente_Fila_Bilheteiras(int Tipo_Cliente)
{
	if(Tipo_Cliente==1)
	{
		Clientes_MR_Fila_Bilheteiras--;
	}
	else
	{
		Resto_Clientes_Fila_Bilheteiras--;
	}
}


//Quando o Cliente Chega à Fila das Bilheteiras É Verificado o Tipo do Cliente Que Chegou e Poe o Cliente no Seu Lugar da Fila
void Atribui_Lugar_Fila_Bilheteiras_A_Cliente(int ID_Cliente)
{
	int i;
	if(Cliente[ID_Cliente].Tipo_Cliente==1)//Se o Cliente Que Chegou For MR
	{
		Cliente[ID_Cliente].Lugar_Fila_Bilheteiras=Clientes_MR_Fila_Bilheteiras; //Isto é, Vai Para o Lugar Do Ultimo Cliente MR (Numero de Clientes MR na Fila <=> Ao Lugar Onde Ele Vai Ficar Se Entrar na Fila)


		for(i=0;i<Clientes_Criados;i++)
		{
			if(Cliente[i].Tipo_Cliente!=1 && Cliente[i].Lugar_Fila_Bilheteiras>0) //Se For Cri ou PG Vai 1 Lugar Para Tras && Esta na Fila das Bilheteiras (Se o Lugar For 0->Nao Esta na Fila)
			{
				Cliente[i].Lugar_Fila_Bilheteiras++;
			}
		}
		//Como MR Tem Prioridade, Fica Atras dos MR Existentes e a Frente dos PG e Cri Existentes. Todos os Outros de PG e Cri, Vao 1 Lugar Para Tras. I.E. Lugar[Cliente PG ou Cri]+1;


	}
	else //Se o Cliente Que Chegou Nao For MR
	{
		Cliente[ID_Cliente].Lugar_Fila_Bilheteiras=Clientes_MR_Fila_Bilheteiras+Resto_Clientes_Fila_Bilheteiras;

	}


}


//Funcao Que Faz o Cliente Recebido (1º da Fila) Sair da Fila e Entrar na Zona. E Todos Avancam 1 Posicao
void Avanca_Um_Lugar_Fila_Bilheteiras()//int ID_Cliente)
{
	int i;
	//Cliente[ID_Cliente].Lugar_Fila_Bilheteiras=0; Nao Precisa Desta Linha Porque o 1º da Fila Esta Incluido no Ciclo Em Baixo
			for(i=0;i<Clientes_Criados;i++)
		{
			if(Cliente[i].Lugar_Fila_Bilheteiras>0) //Se Esta na Fila das Bilheteiras)
			{
				Cliente[i].Lugar_Fila_Bilheteiras--;
			}
		}
} 

//Funcao Que Faz o Cliente Recebido Desistir da Fila e Todos Atras Dele Avancam 1 Posicao
void Desiste_Fila_E_Avancam_Um_Lugar_Fila_Bilheteiras(int ID_Cliente)
{
	int i;
	Cliente[ID_Cliente].Lugar_Fila_Bilheteiras=0; //Cliente Que Desiste Sai da Fila (Poiscao=0)

	for(i=0;i<Clientes_Criados;i++) //Todos os Clientes Atras Dele Avancam 1 Posicao
	{
		if(Cliente[i].Lugar_Fila_Bilheteiras>0) //Se Esta na Fila das Bilheteiras)
		{
			Cliente[i].Lugar_Fila_Bilheteiras--;
		}
	}
} 

//Aumenta o Numero de Clientes na Fila Dependendo do Seu Tipo
void Aumenta_Tipo_Cliente_Fila_Bar(int Tipo_Cliente)
{
	if(Tipo_Cliente==1)
	{
		Clientes_MR_Fila_Bar++;
	}
	else if(Tipo_Cliente==3)
	{
		Clientes_PG_Fila_Bar++;
	}
	else if(Tipo_Cliente==2)
	{
		Clientes_Cri_Fila_Bar++;
	}
}

//Diminui o Numero de Clientes na Fila Dependendo do Seu Tipo
void Diminui_Tipo_Cliente_Fila_Bar(int Tipo_Cliente)
{
	if(Tipo_Cliente==1)
	{
		Clientes_MR_Fila_Bar--;
	}
	else if(Tipo_Cliente==3)
	{
		Clientes_PG_Fila_Bar--;
	}
	else if(Tipo_Cliente==2)
	{
		Clientes_Cri_Fila_Bar--;
	}
}

//Quando o Cliente Chega à Fila do Bar É Verificado o Tipo do Cliente Que Chegou e Poe o Cliente no Seu Lugar da Fila
void Atribui_Lugar_Fila_Bar_A_Cliente(int ID_Cliente)
{
	int i;
	if(Cliente[ID_Cliente].Tipo_Cliente==1)//Se o Cliente Que Chegou For MR
	{
		Cliente[ID_Cliente].Lugar_Fila_Bar=Clientes_MR_Fila_Bar; //Isto é, Vai Para o Lugar Do Ultimo Cliente MR (Numero de Clientes MR na Fila <=> Ao Lugar Onde Ele Vai Ficar Se Entrar na Fila)


		for(i=0;i<Clientes_Criados;i++)
		{
			if(Cliente[i].Tipo_Cliente!=1 && Cliente[i].Lugar_Fila_Bar>0) //Se For Cri ou PG Vai 1 Lugar Para Tras && Esta na Fila das Bar (Se o Lugar For 0->Nao Esta na Fila)
			{
				Cliente[i].Lugar_Fila_Bar++;
			}
		}
		//Como MR Tem Prioridade, Fica Atras dos MR Existentes e a Frente dos PG e Cri Existentes. Todos os Outros de PG e Cri, Vao 1 Lugar Para Tras. I.E. Lugar[Cliente PG ou Cri]+1;


	}
	else if(Cliente[ID_Cliente].Tipo_Cliente==3)//Se o Cliente Que Chegou For PG
	{
		Cliente[ID_Cliente].Lugar_Fila_Bar=Clientes_MR_Fila_Bar+Clientes_PG_Fila_Bar; //Isto é, Vai Para o Lugar Do Ultimo Cliente PG (Numero de Clientes MR+PG na Fila <=> Ao Lugar Onde Ele Vai Ficar Se Entrar na Fila)


		for(i=0;i<Clientes_Criados;i++)
		{
			if(Cliente[i].Tipo_Cliente==2 && Cliente[i].Lugar_Fila_Bar>0) //Se For Cri Vai 1 Lugar Para Tras && Esta na Fila das Bar (Se o Lugar For 0->Nao Esta na Fila)
			{
				Cliente[i].Lugar_Fila_Bar++;
			}
		}
	}
	else if(Cliente[ID_Cliente].Tipo_Cliente==2)//Se o Cliente Que Chegou For Cri
	{
		Cliente[ID_Cliente].Lugar_Fila_Bar=Clientes_MR_Fila_Bar+Clientes_PG_Fila_Bar+Clientes_Cri_Fila_Bar; //Vai Para o Ultimo Lugar da Fila Composta Pela Soma de Todas as Outras "Sub Filas MR, PG e Cri"

	}


}

//Funcao Que Faz o Cliente Recebido (1º da Fila) Sair da Fila e Entrar na Zona. E Todos Avancam 1 Posicao
void Avanca_Um_Lugar_Fila_Bar()//int ID_Cliente)
{
	int i;
	//Cliente[ID_Cliente].Lugar_Fila_Bar=0; Nao Precisa Desta Linha Porque o 1º da Fila Esta Incluido no Ciclo Em Baixo
			for(i=0;i<Clientes_Criados;i++)
		{
			if(Cliente[i].Lugar_Fila_Bar>0) //Se Esta na Fila da Bar)
			{
				Cliente[i].Lugar_Fila_Bar--;
			}
		}
} 

//Funcao Que Faz o Cliente Recebido Desistir da Fila e Todos Atras Dele Avancam 1 Posicao
void Desiste_Fila_E_Avancam_Um_Lugar_Fila_Bar(int ID_Cliente)
{
	int i;
	Cliente[ID_Cliente].Lugar_Fila_Bar=0; //Cliente Que Desiste Sai da Fila (Poiscao=0)

	for(i=0;i<Clientes_Criados;i++) //Todos os Clientes Atras Dele Avancam 1 Posicao
	{
		if(Cliente[i].Lugar_Fila_Bar>0) //Se Esta na Fila do Bar)
		{
			Cliente[i].Lugar_Fila_Bar--;
		}
	}
} 


////////////////////////// Montanha Russa //////////////////////////////////////////////



//Quando o Cliente Chega à Fila da Montanha Russa Poe o Cliente no Seu Lugar da Fila
void Atribui_Lugar_Fila_MontanhaRussa_A_Cliente(int ID_Cliente)
{
	Cliente[ID_Cliente].Lugar_Fila_MontanhaRussa=Clientes_Fila_MontanhaRussa; //Vai Para o Ultimo Lugar da Fila
}

//Quando 2 Pessoas Andam na Montanha Russa, Todos Vao Avancar 2 Posicoes (Lugar_Fila-2)
void Avanca_Dois_Lugares_Fila_MontanhaRussa()//int ID_Cliente)
{
	int i;
	for(i=0;i<Clientes_Criados;i++)
	{
		if(Cliente[i].Lugar_Fila_MontanhaRussa==1) //Se o Cliente Estiver na Fila da Montanha Russa e For o 1º (Regra Especial Pois 1-2 =-1)
		{
			Cliente[i].Lugar_Fila_MontanhaRussa-1; //O 1º da Fila So Passa 1 Lugar Para a Frente!
		}
		if(Cliente[i].Lugar_Fila_MontanhaRussa>1) //Se o Cliente Estiver na Fila da Montanha Russa e Nao é o 1º da Fila
		{
			Cliente[i].Lugar_Fila_MontanhaRussa-2; //Passa 2 Lugares Para a Frente Pois os 2 Primeiros Andaram na Montanha Russa
		}
	}
//A Parte Importante Desta Funcao É o Facto de Que Sempre Que 2 Clientes Andam na Montanha Russa, TODOS Andam 2 Posicoes. O 3º Lugar Ate ao Fim da Fila Andam 2 Casas. O 2º Lugar da Fila Tambem Anda 2 Casas (2-2=0. 0<=> Nao Esta na Fila/Saiu da Fila.
//No Entanto o 1º Lugar Apenas Anda 1 Casa. Senao Ficaria (1-2=-1)
}

//Funcao Que Faz o Cliente Recebido Desistir da Fila e Todos Atras Dele Avancam 1 Posicao
void Desiste_Fila_E_Avancam_Um_Lugar_Fila_MontanhaRussa(int ID_Cliente)
{
	int i;
	Cliente[ID_Cliente].Lugar_Fila_MontanhaRussa=0; //Cliente Que Desiste Sai da Fila (Poiscao=0)

	for(i=0;i<Clientes_Criados;i++) //Todos os Clientes Atras Dele Avancam 1 Posicao
	{
		if(Cliente[i].Lugar_Fila_MontanhaRussa>0) //Se Esta na Fila da Montanha Russa)
		{
			Cliente[i].Lugar_Fila_MontanhaRussa--;
		}
	}
} 



/////////////////////////// Carroussel /////////////////////////////////////////////////


//Aumenta o Numero de Clientes na Fila Dependendo do Seu Tipo
void Aumenta_Tipo_Cliente_Fila_Carroussel(int Tipo_Cliente, int Idade)
{
	if(Tipo_Cliente==2 && Idade<=10)
	{
		Clientes_Cri_Pequenas_Fila_Carroussel++;
	}
	else if(Tipo_Cliente==2 && Idade>10)
	{
		Clientes_Cri_Fila_Carroussel++;
	}
	else if(Tipo_Cliente==1)
	{
		Clientes_MR_Fila_Carroussel++;
	}
	else if(Tipo_Cliente==3)
	{
		Clientes_PG_Fila_Carroussel++;
	}
}

//Diminui o Numero de Clientes na Fila Dependendo do Seu Tipo
void Diminui_Tipo_Cliente_Fila_Carroussel(int Tipo_Cliente, int Idade)
{
	if(Tipo_Cliente==2 && Idade<=10)
	{
		Clientes_Cri_Pequenas_Fila_Carroussel--;
	}
	else if(Tipo_Cliente==2 && Idade>10)
	{
		Clientes_Cri_Fila_Carroussel--;
	}
	else if(Tipo_Cliente==1)
	{
		Clientes_MR_Fila_Carroussel--;
	}
	else if(Tipo_Cliente==3)
	{
		Clientes_PG_Fila_Carroussel--;
	}
}


//Quando o Cliente Chega à Fila da Carroussel É Verificado o Tipo do Cliente Que Chegou e Poe o Cliente no Seu Lugar da Fila
void Atribui_Lugar_Fila_Carroussel_A_Cliente(int ID_Cliente)
{
	int i;
	if(Cliente[ID_Cliente].Tipo_Cliente==2 && Cliente[ID_Cliente].Idade<=10)//Se o Cliente Que Chegou For Cri Mais Nova
	{
		Cliente[ID_Cliente].Lugar_Fila_Carroussel=Clientes_Cri_Pequenas_Fila_Carroussel; //Isto é, Vai Para o Lugar Do Ultimo Cliente Cri Mais Nova (Numero de Clientes Cri Mais Nova na Fila <=> Ao Lugar Onde Ele Vai Ficar Se Entrar na Fila)


		for(i=0;i<Clientes_Criados;i++)
		{
			if(Cliente[i].Idade>10 && Cliente[i].Lugar_Fila_Carroussel>0) //Se For Criana Mais Velha, MR ou PG Vai 1 Lugar Para Tras && Esta na Fila do Carroussel (Se o Lugar For 0->Nao Esta na Fila). Aqui Verificamos Pela Idade! Cri Novas dos 2 aos 10
			{
				Cliente[i].Lugar_Fila_Carroussel++;
			}
		}
		//Como Cri Mais Novas Tem Prioridade, Fica Atras das Cri Mais Novas Existentes e a Frente das Criancas Mais Velhas e dos MR e PG Existentes. Todos os Outros , Vao 1 Lugar Para Tras. I.E. Lugar[Cliente Cri Velha ou MR ou PG]+1;


	}
	if(Cliente[ID_Cliente].Tipo_Cliente==2 && Cliente[ID_Cliente].Idade>10)//Se o Cliente Que Chegou For Cri Mais Velha
	{
		Cliente[ID_Cliente].Lugar_Fila_Carroussel=Clientes_Cri_Pequenas_Fila_Carroussel+Clientes_Cri_Fila_Carroussel; //Isto é, Vai Para o Lugar Do Ultimo Cliente Cri (Numero de Clientes Cri na Fila <=> Ao Lugar Onde Ele Vai Ficar Se Entrar na Fila)


		for(i=0;i<Clientes_Criados;i++)
		{
			if(Cliente[i].Tipo_Cliente!=2 && Cliente[i].Lugar_Fila_Carroussel>0) //Se For MR ou PG Vai 1 Lugar Para Tras && Esta na Fila do Carroussel (Se o Lugar For 0->Nao Esta na Fila)
			{
				Cliente[i].Lugar_Fila_Carroussel++;
			}
		}
		//Fica Atras das Criancas Todas Existentes e a Frente dos MR e PG Existentes. Todos os Outros de MR e PG, Vao 1 Lugar Para Tras. I.E. Lugar[Cliente MR ou PG]+1;


	}

	else if(Cliente[ID_Cliente].Tipo_Cliente==1)//Se o Cliente Que Chegou For MR
	{
		Cliente[ID_Cliente].Lugar_Fila_Carroussel=Clientes_Cri_Pequenas_Fila_Carroussel+Clientes_Cri_Fila_Carroussel+Clientes_MR_Fila_Carroussel; //Isto é, Vai Para o Lugar Do Ultimo Cliente MR (Numero de Clientes Cri+MR na Fila <=> Ao Lugar Onde Ele Vai Ficar Se Entrar na Fila)


		for(i=0;i<Clientes_Criados;i++)
		{
			if(Cliente[i].Tipo_Cliente==3 && Cliente[i].Lugar_Fila_Carroussel>0) //Se For PG Vai 1 Lugar Para Tras && Esta na Fila do Carroussel (Se o Lugar For 0->Nao Esta na Fila)
			{
				Cliente[i].Lugar_Fila_Carroussel++;
			}
		}
	}
	else if(Cliente[ID_Cliente].Tipo_Cliente==3)//Se o Cliente Que Chegou For PG
	{
		Cliente[ID_Cliente].Lugar_Fila_Carroussel=Clientes_Cri_Pequenas_Fila_Carroussel+Clientes_Cri_Fila_Carroussel+Clientes_MR_Fila_Carroussel+Clientes_PG_Fila_Carroussel; //Vai Para o Ultimo Lugar da Fila Composta Pela Soma de Todas as Outras "Sub Filas Cri, MR e PG"

	}


}

//Funcao Que Faz o Cliente Recebido (1º da Fila) Sair da Fila e Entrar na Zona. E Todos Avancam 1 Posicao
void Avanca_Um_Lugar_Fila_Carroussel()//int ID_Cliente)
{
	int i;
		for(i=0;i<Clientes_Criados;i++)
		{
			if(Cliente[i].Lugar_Fila_Carroussel>0) //Se Esta na Fila do Carroussel)
			{
				Cliente[i].Lugar_Fila_Carroussel--;
			}
		}
} 

//Funcao Que Faz o Cliente Recebido Desistir da Fila e Todos Atras Dele Avancam 1 Posicao
void Desiste_Fila_E_Avancam_Um_Lugar_Fila_Carroussel(int ID_Cliente)
{
	int i;
	Cliente[ID_Cliente].Lugar_Fila_Carroussel=0; //Cliente Que Desiste Sai da Fila (Poiscao=0)

	for(i=0;i<Clientes_Criados;i++) //Todos os Clientes Atras Dele Avancam 1 Posicao
	{
		if(Cliente[i].Lugar_Fila_Carroussel>0) //Se Esta na Fila do Carroussel)
		{
			Cliente[i].Lugar_Fila_Carroussel--;
		}
	}
} 

//Funcao de Tratamento dos Lugares Carroussel
void Gestao_Entradas_Carroussel()
{
	while(Fecha_Feira!=1)
	{
		sem_wait(&Espera_Cliente); //Semaforo Inicializado a 0. Quando Recebe Post do Cliente, Continua
		sem_wait(&Lotacao_Carroussel); //Se Tiver Lugar no carroussel
		sem_post(&Espera_Sinal_Entrada);
		sem_post(&Semaforo_So_1_Cliente); //Ultimo Passo da Rotina da Comunicacao Gestor Carroussel-Cliente
	}
}

//Cria Gestor de Lugares Carroussel
void Criacao_Gestor_Carroussel()
{
	if(pthread_create(&(Thread_Gestor_Carroussel), NULL ,(void *)&Gestao_Entradas_Carroussel,NULL) != 0)
	{
		printf("Erro na Criacao da Tarefa do Gestor do Carroussel\n");
		exit(1);
	}

}


int Executa_Proxima_Zona(int Proxima_Zona, int ID_Cliente_Recebido, int Tempo_Fica_Zona)
{
	int Hora_Actual=0;

	//Sinais Para Indicar Que Desistiu e Nao Vai Haver Entradas nas Zonas Nem Saidas
	int Desistencia_Fila_Bar=0;
	int Desistencia_Fila_Carroussel=0;
	int Desistencia_Fila_MontanhaRussa=0;

	srand(time(NULL));

	if(Proxima_Zona==2)//Vai ao Bar
	{
		//Instrucoes Bar:
	//------------------------------------------------------------------------------------------------------
		//case 4: Entra Fila Bar
	pthread_mutex_lock(&Trinco_Comunicacao_Socket);
	Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar=Tempo_Simulacao_Actual;
	//Hora_Actual=Tempo_Simulacao_Actual;
	Hora_Actual=Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar; //Nao va a Hora Mudar Entre Estas 2 Instrucoes e Haver Diferencas.


	//Define a Hora de Desistencia:
	Cliente[ID_Cliente_Recebido].Hora_Vai_Desistir_Fila_Bar=Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar+Cliente[ID_Cliente_Recebido].Tempo_Espera_Maximo_Filas;


//Aumenta o Numero de Clientes na Fila Dependendo do Seu Tipo
Aumenta_Tipo_Cliente_Fila_Bar(Cliente[ID_Cliente_Recebido].Tipo_Cliente);

	Escreve_Mensagem(newsockfd,       Cliente[ID_Cliente_Recebido].ID_Cliente, Cliente[ID_Cliente_Recebido].Tipo_Cliente,       4      , Hora_Actual, Cliente[ID_Cliente_Recebido].Idade, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira, Cliente[ID_Cliente_Recebido].Hora_Saida_Feira);
	//sleep(2);
	sleep(1);

Atribui_Lugar_Fila_Bar_A_Cliente(Cliente[ID_Cliente_Recebido].ID_Cliente);
	pthread_mutex_unlock(&Trinco_Comunicacao_Socket);




while(Cliente[ID_Cliente_Recebido].Lugar_Fila_Bar!=1 && Desistencia_Fila_Bar!=1) //Enquanto o Cliente da Tarefa Nao For 1º da Fila, Espera
{
	if(Tempo_Simulacao_Actual>=Cliente[ID_Cliente_Recebido].Hora_Vai_Desistir_Fila_Bar)
	{
		//Faz Evento de Desistencia da Fila do Bar
	//------------------------------------------------------------------------------------------------------
		//case 7: Desiste Fila Bar
	pthread_mutex_lock(&Trinco_Comunicacao_Socket);
	Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar=Tempo_Simulacao_Actual;
	//Hora_Actual=Tempo_Simulacao_Actual;
	Hora_Actual=Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar; //Nao va a Hora Mudar Entre Estas 2 Instrucoes e Haver Diferencas.

//Diminui o Numero de Clientes na Fila Dependendo do Seu Tipo
Diminui_Tipo_Cliente_Fila_Bar(Cliente[ID_Cliente_Recebido].Tipo_Cliente);

	Escreve_Mensagem(newsockfd,       Cliente[ID_Cliente_Recebido].ID_Cliente, Cliente[ID_Cliente_Recebido].Tipo_Cliente,       7      , Hora_Actual, Cliente[ID_Cliente_Recebido].Idade, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira, Cliente[ID_Cliente_Recebido].Hora_Saida_Feira);
	//sleep(2);
	sleep(1);

Desiste_Fila_E_Avancam_Um_Lugar_Fila_Bar(Cliente[ID_Cliente_Recebido].ID_Cliente);
	pthread_mutex_unlock(&Trinco_Comunicacao_Socket);
	
	if(Cliente[ID_Cliente_Recebido].Tipo_Cliente==1) 
	{	
		Proxima_Zona=(rand() %2)+2; //Cria Numero Random de 2 a 3
	}
	else
	{
		Proxima_Zona=(rand() %3)+2; //Cria Numero Random de 2 a 4
	}

	//Se Cliente Desistir, Activa Sinal Para Indicar Que Nao Vai Haver Entrada na Zona em Questao nem Saida
	Desistencia_Fila_Bar=1;
	}
}
//----------------------------------------------------------------------------------------------------------------------------

if(Desistencia_Fila_Bar==0)
{


	//----------------------------------------------------------------------------------------------------------------------------

	//Invoca Funcao Random Se Fica ou Nao no Bar Depois de Ser Atendido
	int Fica_Bar=Fica_Ou_Nao_Bar(Percentagem_Ficar_Bar);

	if(Fica_Bar==1)
	{
		//Procedimento É Atendido e Fica no Bar
	//------------------------------------------------------------------------------------------------------
			//case 6: Fica no Bar/Sai Fila Bar, é Atendido e Fica no Bar
	sem_wait(&Semaforo_Lotacao_Bar);

		pthread_mutex_lock(&Trinco_Comunicacao_Socket);
		Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar=Tempo_Simulacao_Actual;
		//Hora_Actual=Tempo_Simulacao_Actual;
		Hora_Actual=Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar; //Nao va a Hora Mudar Entre Estas 2 Instrucoes e Haver Diferencas.

	//Diminui o Numero de Clientes na Fila Dependendo do Seu Tipo
	Diminui_Tipo_Cliente_Fila_Bar(Cliente[ID_Cliente_Recebido].Tipo_Cliente);

		Escreve_Mensagem(newsockfd,       Cliente[ID_Cliente_Recebido].ID_Cliente, Cliente[ID_Cliente_Recebido].Tipo_Cliente,       6      , Hora_Actual, Cliente[ID_Cliente_Recebido].Idade, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira, Cliente[ID_Cliente_Recebido].Hora_Saida_Feira);
		//sleep(2);
		sleep(1);

		Avanca_Um_Lugar_Fila_Bar();//int ID_Cliente);
			pthread_mutex_unlock(&Trinco_Comunicacao_Socket);

		sleep(Tempo_Fica_Zona);
	

	//------------------------------------------------------------------------------------------------------
			//case 8: Sai Sala Bar
		pthread_mutex_lock(&Trinco_Comunicacao_Socket);
		//=Tempo_Simulacao_Actual;
		Hora_Actual=Tempo_Simulacao_Actual;

		Escreve_Mensagem(newsockfd,       Cliente[ID_Cliente_Recebido].ID_Cliente, Cliente[ID_Cliente_Recebido].Tipo_Cliente,       8      , Hora_Actual, Cliente[ID_Cliente_Recebido].Idade, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira, Cliente[ID_Cliente_Recebido].Hora_Saida_Feira);
	//sleep(2);
	sleep(1);

		pthread_mutex_unlock(&Trinco_Comunicacao_Socket);

	sem_post(&Semaforo_Lotacao_Bar);

	if(Cliente[ID_Cliente_Recebido].Tipo_Cliente==1) 
	{	
		Proxima_Zona=(rand() %2)+2; //Cria Numero Random de 2 a 3
	}
	else
	{
		Proxima_Zona=(rand() %3)+2; //Cria Numero Random de 2 a 4
	}




	}
	else
	{
		//Procedimento É Atendido e Sai do Bar

	//------------------------------------------------------------------------------------------------------
			//case 5: Sai Fila Bar, é Atendido e Sai do Bar
		pthread_mutex_lock(&Trinco_Comunicacao_Socket);
		Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar=Tempo_Simulacao_Actual;
		//Hora_Actual=Tempo_Simulacao_Actual;
		Hora_Actual=Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar; //Nao va a Hora Mudar Entre Estas 2 Instrucoes e Haver Diferencas.

	//Diminui o Numero de Clientes na Fila Dependendo do Seu Tipo
	Diminui_Tipo_Cliente_Fila_Bar(Cliente[ID_Cliente_Recebido].Tipo_Cliente);

		Escreve_Mensagem(newsockfd,       Cliente[ID_Cliente_Recebido].ID_Cliente, Cliente[ID_Cliente_Recebido].Tipo_Cliente,       5      , Hora_Actual, Cliente[ID_Cliente_Recebido].Idade, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira, Cliente[ID_Cliente_Recebido].Hora_Saida_Feira);
		sleep(1);

		Avanca_Um_Lugar_Fila_Bar();
			pthread_mutex_unlock(&Trinco_Comunicacao_Socket);
	
	if(Cliente[ID_Cliente_Recebido].Tipo_Cliente==1) 
	{	
		Proxima_Zona=(rand() %2)+2; //Cria Numero Random de 2 a 3
	}
	else
	{
		Proxima_Zona=(rand() %3)+2; //Cria Numero Random de 2 a 4
	}




	}//Fim do If se Fica ou Nao no Bar



}//Fim do If se Aconteceu Desistencia


	}
	else if(Proxima_Zona==3)//Vai ao Carroussel
	{
		//Instrucoes Carroussel:
//------------------------------------------------------------------------------------------------------
		//case 9: Entra Fila Carroussel
	pthread_mutex_lock(&Trinco_Comunicacao_Socket);
	Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel=Tempo_Simulacao_Actual;
	//Hora_Actual=Tempo_Simulacao_Actual;
	Hora_Actual=Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel; //porque a hora pode mudar enquanto está percorrendo as instruçoes


	//Define a Hora de Desistencia:
	Cliente[ID_Cliente_Recebido].Hora_Vai_Desistir_Fila_Carroussel=Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel+Cliente[ID_Cliente_Recebido].Tempo_Espera_Maximo_Filas;


//Aumenta o Numero de Clientes na Fila Dependendo do Seu Tipo
Aumenta_Tipo_Cliente_Fila_Carroussel(Cliente[ID_Cliente_Recebido].Tipo_Cliente, Cliente[ID_Cliente_Recebido].Idade);

	Escreve_Mensagem(newsockfd,       Cliente[ID_Cliente_Recebido].ID_Cliente, Cliente[ID_Cliente_Recebido].Tipo_Cliente,       9      , Hora_Actual, Cliente[ID_Cliente_Recebido].Idade, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira, Cliente[ID_Cliente_Recebido].Hora_Saida_Feira);
	//sleep(2);
	sleep(1);

Atribui_Lugar_Fila_Carroussel_A_Cliente(Cliente[ID_Cliente_Recebido].ID_Cliente);
	pthread_mutex_unlock(&Trinco_Comunicacao_Socket);




while(Cliente[ID_Cliente_Recebido].Lugar_Fila_Carroussel!=1 && Desistencia_Fila_Carroussel!=1) //Enquanto o Cliente da Tarefa Nao For 1º da Fila, Espera
{
	if(Tempo_Simulacao_Actual>=Cliente[ID_Cliente_Recebido].Hora_Vai_Desistir_Fila_Carroussel)
	{
		//Faz Evento de Desistencia da Fila da Carroussel
	//------------------------------------------------------------------------------------------------------
		//case 11: Desiste Fila Carroussel
	pthread_mutex_lock(&Trinco_Comunicacao_Socket);
	Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel=Tempo_Simulacao_Actual;
	//Hora_Actual=Tempo_Simulacao_Actual;
	Hora_Actual=Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel; 

//Diminui o Numero de Clientes na Fila Dependendo do Seu Tipo
Diminui_Tipo_Cliente_Fila_Carroussel(Cliente[ID_Cliente_Recebido].Tipo_Cliente, Cliente[ID_Cliente_Recebido].Idade);

	Escreve_Mensagem(newsockfd,       Cliente[ID_Cliente_Recebido].ID_Cliente, Cliente[ID_Cliente_Recebido].Tipo_Cliente,       11      , Hora_Actual, Cliente[ID_Cliente_Recebido].Idade, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira, Cliente[ID_Cliente_Recebido].Hora_Saida_Feira);
	//sleep(2);
	sleep(1);

Desiste_Fila_E_Avancam_Um_Lugar_Fila_Carroussel(Cliente[ID_Cliente_Recebido].ID_Cliente);
	pthread_mutex_unlock(&Trinco_Comunicacao_Socket);
	
	if(Cliente[ID_Cliente_Recebido].Tipo_Cliente==1) 
	{	
		Proxima_Zona=(rand() %2)+2; //Cria Numero Random de 2 a 3
	}
	else
	{
		Proxima_Zona=(rand() %3)+2; //Cria Numero Random de 2 a 4
	}

	//Se Cliente Desistir, Activa Sinal Para Indicar Que Nao Vai Haver Entrada na Zona em Questao nem Saida
	Desistencia_Fila_Carroussel=1;
	}
}
//----------------------------------------------------------------------------------------------------------------------------

if(Desistencia_Fila_Carroussel==0)
{


	sem_wait(&Semaforo_So_1_Cliente); //Semaforo Inicializado a 1. Quando Cliente Chega ao Carroussel, Faz Wait. Gestor Trata de 1 Cliente de Cada Vez (Exclusao Mutua Com Semaforo Em Vez de Trincos)
	sem_post(&Espera_Cliente); //Sinaliza Que Chegou 1 Cliente
	sem_wait(&Espera_Sinal_Entrada); //Semaforo Inicializacao a 0.



		pthread_mutex_lock(&Trinco_Comunicacao_Socket);
		Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel=Tempo_Simulacao_Actual;
		//Hora_Actual=Tempo_Simulacao_Actual;
		Hora_Actual=Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel;

	//Diminui o Numero de Clientes na Fila Dependendo do Seu Tipo
	Diminui_Tipo_Cliente_Fila_Carroussel(Cliente[ID_Cliente_Recebido].Tipo_Cliente, Cliente[ID_Cliente_Recebido].Idade);

		Escreve_Mensagem(newsockfd,       Cliente[ID_Cliente_Recebido].ID_Cliente, Cliente[ID_Cliente_Recebido].Tipo_Cliente,       10      , Hora_Actual, Cliente[ID_Cliente_Recebido].Idade, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira, Cliente[ID_Cliente_Recebido].Hora_Saida_Feira);
	//sleep(2);
	sleep(1);

	Avanca_Um_Lugar_Fila_Carroussel();//int ID_Cliente);
		pthread_mutex_unlock(&Trinco_Comunicacao_Socket);

	sleep(Tempo_Fica_Zona);


	//------------------------------------------------------------------------------------------------------

			//case 16: Sai Carroussel
		pthread_mutex_lock(&Trinco_Comunicacao_Socket);
		//=Tempo_Simulacao_Actual;
		Hora_Actual=Tempo_Simulacao_Actual;

		Escreve_Mensagem(newsockfd,       Cliente[ID_Cliente_Recebido].ID_Cliente, Cliente[ID_Cliente_Recebido].Tipo_Cliente,       16      , Hora_Actual, Cliente[ID_Cliente_Recebido].Idade, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira, Cliente[ID_Cliente_Recebido].Hora_Saida_Feira);
	//sleep(2);
	sleep(1);

		pthread_mutex_unlock(&Trinco_Comunicacao_Socket);

		//Abre Vaga no Semaforo do Carroussel
		sem_post(&Lotacao_Carroussel);

	if(Cliente[ID_Cliente_Recebido].Tipo_Cliente==1)
	{	
		Proxima_Zona=(rand() %2)+2; //Cria Numero Random de 2 a 3
	}
	else
	{
		Proxima_Zona=(rand() %3)+2; //Cria Numero Random de 2 a 4
	}
	
}//Fim do If se Houve Desistencia

	}
	else if(Proxima_Zona==4)//Vai ao Carroussel
	{
		int Cliente_Esta_No_Semaforo=0;
		//Instrucoes Carroussel:

//------------------------------------------------------------------------------------------------------
		//case 12: Entra Fila Montanha Russa
	pthread_mutex_lock(&Trinco_Comunicacao_Socket);
	Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa=Tempo_Simulacao_Actual;
	//Hora_Actual=Tempo_Simulacao_Actual;
	Hora_Actual=Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa; 


	//Define a Hora de Desistencia:
	Cliente[ID_Cliente_Recebido].Hora_Vai_Desistir_Fila_MontanhaRussa=Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa+Cliente[ID_Cliente_Recebido].Tempo_Espera_Maximo_Filas;

//Aumenta o Numero de Clientes na Fila
Clientes_Fila_MontanhaRussa++;

	Escreve_Mensagem(newsockfd,       Cliente[ID_Cliente_Recebido].ID_Cliente, Cliente[ID_Cliente_Recebido].Tipo_Cliente,       12      , Hora_Actual, Cliente[ID_Cliente_Recebido].Idade, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira, Cliente[ID_Cliente_Recebido].Hora_Saida_Feira);
	sleep(1);

Atribui_Lugar_Fila_MontanhaRussa_A_Cliente(Cliente[ID_Cliente_Recebido].ID_Cliente);
	pthread_mutex_unlock(&Trinco_Comunicacao_Socket);


if(Cliente[ID_Cliente_Recebido].Lugar_Fila_MontanhaRussa<3 && Cliente[ID_Cliente_Recebido].Lugar_Fila_MontanhaRussa>0) //Se For 1º ou 2º da Fila
{
	Cliente_Esta_No_Semaforo=1;
	Pessoas_No_Semaforo_MontanhaRussa++;
}


while((Pessoas_No_Semaforo_MontanhaRussa!=2 || Cliente_Esta_No_Semaforo!=1) && Desistencia_Fila_MontanhaRussa!=1) //Enquanto Nao Chegar o Momento de Desistir, Espera. Nao Pode Ter Condicao dos 2 Primeiros Nao Desistem, Porque se So Chegar 1, Vai Ficar Sempre La, Como Tem de Esperar Por Outro. Neste Caso, Se Estiver no Semaforo e Estiver +1 Pessoa Tambem, Acaba o Ciclo
{
	if(Tempo_Simulacao_Actual>=Cliente[ID_Cliente_Recebido].Hora_Vai_Desistir_Fila_MontanhaRussa)
	{

		if(Cliente[ID_Cliente_Recebido].Lugar_Fila_MontanhaRussa<3 && Cliente[ID_Cliente_Recebido].Lugar_Fila_MontanhaRussa>0) //Se For 1º ou 2º da Fila
		{
			Cliente_Esta_No_Semaforo=0;
			Pessoas_No_Semaforo_MontanhaRussa--;
		}

		//Faz Evento de Desistencia da Fila da Montanha Russa
		//------------------------------------------------------------------------------------------------------
			//case 14: Desiste Fila Montanha Russa
		pthread_mutex_lock(&Trinco_Comunicacao_Socket);
		Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa=Tempo_Simulacao_Actual;
		//Hora_Actual=Tempo_Simulacao_Actual;
		Hora_Actual=Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa; 

	//Diminui o Numero de Clientes na Fila
	Clientes_Fila_MontanhaRussa--;

		Escreve_Mensagem(newsockfd,       Cliente[ID_Cliente_Recebido].ID_Cliente, Cliente[ID_Cliente_Recebido].Tipo_Cliente,       14      , Hora_Actual, Cliente[ID_Cliente_Recebido].Idade, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira, Cliente[ID_Cliente_Recebido].Hora_Saida_Feira);
		//sleep(2);
		sleep(1);


	Desiste_Fila_E_Avancam_Um_Lugar_Fila_MontanhaRussa(Cliente[ID_Cliente_Recebido].ID_Cliente);
		pthread_mutex_unlock(&Trinco_Comunicacao_Socket);
	
		//Proxima_Zona=(rand() %3)+2; //Cria Numero Random de 2 a 4
		if(Cliente[ID_Cliente_Recebido].Tipo_Cliente==1) 
		{	
			Proxima_Zona=(rand() %2)+2; //Cria Numero Random de 2 a 3
		}
		else
		{
			Proxima_Zona=(rand() %3)+2; //Cria Numero Random de 2 a 4
		}

		//Se Cliente Desistir, Activa Sinal Para Indicar Que Nao Vai Haver Entrada na Zona em Questao nem Saida
		Desistencia_Fila_MontanhaRussa=1;
	}
}
//----------------------------------------------------------------------------------------------------------------------------

if(Desistencia_Fila_MontanhaRussa==0)
{

//------------------------------------------------------------------------------------------------------
		//case 13: Sai Fila Montanha Russa e Anda na Montanha Russa

	pthread_mutex_lock(&Trinco_Comunicacao_Socket);
	Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa=Tempo_Simulacao_Actual;
	//Hora_Actual=Tempo_Simulacao_Actual;
	Hora_Actual=Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa; 

//Diminui o Numero de Clientes na Fila
Clientes_Fila_MontanhaRussa--;

	Escreve_Mensagem(newsockfd,       Cliente[ID_Cliente_Recebido].ID_Cliente, Cliente[ID_Cliente_Recebido].Tipo_Cliente,       13      , Hora_Actual, Cliente[ID_Cliente_Recebido].Idade, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira, Cliente[ID_Cliente_Recebido].Hora_Saida_Feira);
	sleep(1);


	pthread_mutex_lock(&Trinco_MontanhaRussa);
	Clientes_A_Andar_MontanhaRussa++;

	pthread_mutex_unlock(&Trinco_MontanhaRussa);


	pthread_mutex_unlock(&Trinco_Comunicacao_Socket);


	sleep(2);
	

		Aux_MontanhaRussa--; // Quando For 0 <=> 2 Clientes Que Estavam no Semaforo da Montanha Russa Ja Desceram, Semaforo Volta ao Valor Original e Clientes na Fila Avancam 2 Lugares
		if(Aux_MontanhaRussa==0)
		{
			Avanca_Dois_Lugares_Fila_MontanhaRussa();
			Pessoas_No_Semaforo_MontanhaRussa=0;
			Aux_MontanhaRussa=2;
		}


	sleep(2);



	if(Cliente[ID_Cliente_Recebido].Tipo_Cliente==1) 
	{	
		Proxima_Zona=(rand() %2)+2; //Cria Numero Random de 2 a 3
	}
	else
	{
		Proxima_Zona=(rand() %3)+2; //Cria Numero Random de 2 a 4
	}


	}
}//Fim do if Se Houve Desistencia
}


void Gestor_Eventos_Clientes(int ID_Cliente_Recebido) //(int Numero_Cliente)
{

	Cliente[ID_Cliente_Recebido].ID_Cliente=ID_Cliente_Recebido;
	Cliente[ID_Cliente_Recebido].Tipo_Cliente=Define_Tipo_Cliente_Random(Percentagem_Ser_Cliente_Crianca, Percentagem_Ser_Cliente_Mobilidade_Reduzida);
	Cliente[ID_Cliente_Recebido].Tempo_Espera_Maximo_Filas=Tempo_Maximo_Espera_Filas; //Variavel Lida do Config. //Tirar em Principio da Mensagem
	Cliente[ID_Cliente_Recebido].Tempo_Estadia_Feira=Calcula_Tempos_Medios_Random(Tempo_Medio_Estadia_Cliente_Na_Feira);//Tirar em Principio da Mensagem

	Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar=0;
	Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar=0;
	Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar=0;

	Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel=0;
	Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel=0;
	Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel=0;

	Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa=0;
	Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa=0;
	Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa=0;

	Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras=0;
	Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras=0;
	Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira=0;
	Cliente[ID_Cliente_Recebido].Hora_Saida_Feira=0;

	Cliente[ID_Cliente_Recebido].Lugar_Fila_Bilheteiras=0;
	Cliente[ID_Cliente_Recebido].Lugar_Fila_Bar=0;
	Cliente[ID_Cliente_Recebido].Lugar_Fila_Carroussel=0;
	Cliente[ID_Cliente_Recebido].Lugar_Fila_MontanhaRussa=0;

	Cliente[ID_Cliente_Recebido].Idade=Define_Idade_Cliente(Cliente[ID_Cliente_Recebido].Tipo_Cliente);

	//Chega a Esta Hora e Desiste da Fila:
	Cliente[ID_Cliente_Recebido].Hora_Vai_Desistir_Fila_Bilheteiras=0;
	Cliente[ID_Cliente_Recebido].Hora_Vai_Desistir_Fila_Bar=0;
	Cliente[ID_Cliente_Recebido].Hora_Vai_Desistir_Fila_Carroussel=0;
	Cliente[ID_Cliente_Recebido].Hora_Vai_Desistir_Fila_MontanhaRussa=0;
	
	//Tempo Que Vai Ficar na Zona:
	Cliente[ID_Cliente_Recebido].Tempo_Fica_Zona=Calcula_Tempos_Medios_Random(Tempo_Medio_Fica_Zonas);

	//int Estado; - Inserido Manualmente
	int Hora_Actual=0; //Variavel Auxiliar (Podia Nao Ter, Mas Facilita Perceber o Que Estamos a Por na Mensagem)

	//Variaveis Para a Decisao de Entrada de Zonas:
	int Proxima_Zona=0;

	//Variavel Que Guarda Se Houve Desistencia ou Nao
	int Desistencia_Fila_Bilheteiras=0;


		//case 1: Entra Fila Bilheteiras da Feira
//usleep(10000);
	pthread_mutex_lock(&Trinco_Comunicacao_Socket);
	Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras=Tempo_Simulacao_Actual;
	//Hora_Actual=Tempo_Simulacao_Actual;
	Hora_Actual=Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras; //Nao va a Hora Mudar Entre Estas 2 Instrucoes e Haver Diferencas.

	//Define a Hora de Desistencia:
	Cliente[ID_Cliente_Recebido].Hora_Vai_Desistir_Fila_Bilheteiras=Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras+Cliente[ID_Cliente_Recebido].Tempo_Espera_Maximo_Filas;

//Aumenta o Numero de Clientes na Fila Dependendo do Seu Tipo
Aumenta_Tipo_Cliente_Fila_Bilheteiras(Cliente[ID_Cliente_Recebido].Tipo_Cliente);

	Escreve_Mensagem(newsockfd,       Cliente[ID_Cliente_Recebido].ID_Cliente, Cliente[ID_Cliente_Recebido].Tipo_Cliente,       1      , Hora_Actual, Cliente[ID_Cliente_Recebido].Idade, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira, Cliente[ID_Cliente_Recebido].Hora_Saida_Feira);
//sleep(2);
sleep(1);
	
Atribui_Lugar_Fila_Bilheteiras_A_Cliente(Cliente[ID_Cliente_Recebido].ID_Cliente);
	pthread_mutex_unlock(&Trinco_Comunicacao_Socket);
//usleep(10000);
//sleep(2);



while(Cliente[ID_Cliente_Recebido].Lugar_Fila_Bilheteiras!=1 && Desistencia_Fila_Bilheteiras!=1) //Enquanto o Cliente da Tarefa Nao For 1º da Fila, Espera
{
	if(Tempo_Simulacao_Actual>=Cliente[ID_Cliente_Recebido].Hora_Vai_Desistir_Fila_Bilheteiras)
	{
		//Faz Evento de Desistencia da Fila das Bilheteiras
	//------------------------------------------------------------------------------------------------------
		//case 3: Desiste Fila Bilheteiras da Feira
	pthread_mutex_lock(&Trinco_Comunicacao_Socket);
	Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras=Tempo_Simulacao_Actual;
	//Hora_Actual=Tempo_Simulacao_Actual;
	Hora_Actual=Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras; //Nao va a Hora Mudar Entre Estas 2 Instrucoes e Haver Diferencas.

//Diminui o Numero de Clientes na Fila Dependendo do Seu Tipo
Diminui_Tipo_Cliente_Fila_Bilheteiras(Cliente[ID_Cliente_Recebido].Tipo_Cliente);

	Escreve_Mensagem(newsockfd,       Cliente[ID_Cliente_Recebido].ID_Cliente, Cliente[ID_Cliente_Recebido].Tipo_Cliente,       3      , Hora_Actual, Cliente[ID_Cliente_Recebido].Idade, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira, Cliente[ID_Cliente_Recebido].Hora_Saida_Feira);
//sleep(2);
sleep(1);

Desiste_Fila_E_Avancam_Um_Lugar_Fila_Bilheteiras(Cliente[ID_Cliente_Recebido].ID_Cliente);
	pthread_mutex_unlock(&Trinco_Comunicacao_Socket);
	
	//Proxima_Zona=(rand() %3)+2; //Cria Numero Random de 2 a 4

	//Se Cliente Desistir, Activa Sinal Para Indicar Que Nao Vai Haver Entrada na Zona em Questao nem Saida
	Desistencia_Fila_Bilheteiras=1;
	}
}


//------------------------------------------------------------------------------------------------------
	if(Desistencia_Fila_Bilheteiras==0)
	{

	//------------------------------------------------------------------------------------------------------
			//case 2: Sai Fila Bilheteiras da Feira e Entra na Feira
	sem_wait(&Semaforo_Lotacao_Feira);

	//usleep(10000);
		pthread_mutex_lock(&Trinco_Comunicacao_Socket);
		Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira=Tempo_Simulacao_Actual;
		//Hora_Actual=Tempo_Simulacao_Actual;
		Hora_Actual=Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira; //Nao va a Hora Mudar Entre Estas 2 Instrucoes e Haver Diferencas.

	//Diminui o Numero de Clientes na Fila Dependendo do Seu Tipo
	Diminui_Tipo_Cliente_Fila_Bilheteiras(Cliente[ID_Cliente_Recebido].Tipo_Cliente);

		Escreve_Mensagem(newsockfd,       Cliente[ID_Cliente_Recebido].ID_Cliente, Cliente[ID_Cliente_Recebido].Tipo_Cliente,       2      , Hora_Actual, Cliente[ID_Cliente_Recebido].Idade, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira, Cliente[ID_Cliente_Recebido].Hora_Saida_Feira);
	//sleep(2);
	sleep(1);
	
	Avanca_Um_Lugar_Fila_Bilheteiras();//int ID_Cliente);
		pthread_mutex_unlock(&Trinco_Comunicacao_Socket);
	//usleep(10000);
	//sleep(2);



	srand(time(NULL));

	//Escolha da 1a Zona Para Onde o Cliente Vai: (Fica Fora do Ciclo While Porque na Funcao "Executa_Proxima_Zona" ja Tem Esta Funcao no Final da Saida da Zona (Seja Desistencia ou Saida Normal)+ Restricao da Montanha Russa se Tipo Cliente for MR	
	if(Cliente[ID_Cliente_Recebido].Tipo_Cliente==1) 
	{	
		Proxima_Zona=(rand() %2)+2; //Cria Numero Random de 2 a 3
	}
	else
	{
		Proxima_Zona=(rand() %3)+2; //Cria Numero Random de 2 a 4
	}

	//Escolhe as Zonas de Forma Random Atraves das Funcoes Auxiliares:
		while(Tempo_Simulacao_Actual<Cliente[ID_Cliente_Recebido].Tempo_Estadia_Feira)
		{
		
			Proxima_Zona=Executa_Proxima_Zona(Proxima_Zona, ID_Cliente_Recebido, Cliente[ID_Cliente_Recebido].Tempo_Fica_Zona);
		}
	//------------------------------------------------------------------------------------------------------
			//case 15: Sai Feira
		pthread_mutex_lock(&Trinco_Comunicacao_Socket);
		Cliente[ID_Cliente_Recebido].Hora_Saida_Feira=Tempo_Simulacao_Actual;
		//Hora_Actual=Tempo_Simulacao_Actual;
		Hora_Actual=Cliente[ID_Cliente_Recebido].Hora_Saida_Feira; //Nao va a Hora Mudar Entre Estas 2 Instrucoes e Haver Diferencas.

		Escreve_Mensagem(newsockfd,       Cliente[ID_Cliente_Recebido].ID_Cliente, Cliente[ID_Cliente_Recebido].Tipo_Cliente,       15      , Hora_Actual, Cliente[ID_Cliente_Recebido].Idade, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Bar, Cliente[ID_Cliente_Recebido].Hora_Atendimento_Bar, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Carroussel, Cliente[ID_Cliente_Recebido].Hora_Entrada_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Fila_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Andou_MontanhaRussa, Cliente[ID_Cliente_Recebido].Hora_Entrada_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Desistencia_Filas_Bilheteiras, Cliente[ID_Cliente_Recebido].Hora_Entrada_Feira, Cliente[ID_Cliente_Recebido].Hora_Saida_Feira);
	sleep(1);
	
		pthread_mutex_unlock(&Trinco_Comunicacao_Socket);	
	sleep(1);

	sem_post(&Semaforo_Lotacao_Feira);
	}//Fim do If se Houve Desistencia
}

//Vai Criar Clientes Ate Alcancar o Tempo de Simulacao OU Criar Todos os Clientes Definidos
void Criacao_Clientes()
{
   	srand(time(NULL));
	long Clientes_Criados2=0;

	//Explicacao Para Este Long  -> Google linuxquestions.org a question about passing arguments into a thread in c

	while(Tempo_Simulacao_Actual<Tempo_Simulacao && Clientes_Criados<=Numero_Clientes_A_Criar)
	{
		if(pthread_create(&(Thread_Cliente[Clientes_Criados]), NULL ,(void *)&Gestor_Eventos_Clientes,(void *)Clientes_Criados2) != 0)
		{
			printf("Erro na criacao da tarefa de criacao dos clientes\n");
			exit(1);
		}	
		sleep(Calcula_Tempos_Medios_Random(Tempo_Medio_Chegadas));
		Clientes_Criados++;	//Variavel Global
		Clientes_Criados2++;	//Variavel Local ->Senao, Para Eliminar Esta Variavel, Talvez Por a Variavel Global Como Long!
	}
}

//Relogio Funciona Ate Todos os Clientes Sairem
void Funcao_Incrementa_Tempo_Actual()
{
	while(Fecha_Feira!=1)
	{
		Tempo_Simulacao_Actual=Tempo_Simulacao_Actual+1;
		sleep(1);
	}
}

//Quando Recebe Mensagem do Monitor Com Sinal Para Comecar a Simulacao
void Comeca_Simulacao()
{
	if(pthread_create(&(Thread_Ciclo_Tempo_Simulacao), NULL ,(void *)&Funcao_Incrementa_Tempo_Actual, NULL) != 0)
	{
		printf("Erro na criacao da tarefa do ciclo do tempo\n");
		exit(1);
	}
	
	
}

int main(int argc, char *argv[])
{
	//Le os Parametros do Ficheiro Config
	Ler_Ficheiro_Config();

	pthread_mutex_init(&Trinco_Comunicacao_Socket, NULL);
	pthread_mutex_init(&Trinco_MontanhaRussa, NULL);

	//Iniciar Semaforos/Semaphores: (Sintaxe: [Semaforo,Flag se o Semaforo Deve Ser Partilhado Com Outras Threads,Valor Inicial do Semaforo]
	sem_init(&Semaforo_Lotacao_Feira,0,Tamanho_Maximo_Feira);	
	sem_init(&Semaforo_Lotacao_Bar,0,Tamanho_Maximo_Bar);	
	sem_init(&Semaforo_Desce_MontanhaRussa,0,2); //Iniciado a 2 Porque Quando Tiver 2 Clientes, Estes Vao Juntos na Montanha Russa

	//Semaforos Para o Gestor do Carroussel:
	sem_init(&Espera_Cliente,0,0);
	sem_init(&Espera_Sinal_Entrada,0,0);
	sem_init(&Lotacao_Carroussel,0,Tamanho_Maximo_Carroussel);
	sem_init(&Semaforo_So_1_Cliente,0,1);


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


	//Comeca Simulacao

	Criacao_Gestor_Carroussel();
	Comeca_Simulacao();
	Criacao_Clientes();

	sleep(2);


	int qwe;
	for(qwe=0;qwe<Clientes_Criados;qwe++)
	{
		pthread_join(Thread_Cliente[qwe],NULL);
	}

	printf("Acabou Join Threads dos Clientes\n");

	sleep(2);

	//Quando Termina Todas as Threads Criadas, a Feira Comeca a Fechar
	Fecha_Feira=1;
	pthread_join(Thread_Ciclo_Tempo_Simulacao,NULL);

	printf("Acabou Join Thread do Ciclo do Tempo de Simulacao\n");


	sleep(2);

	//Mensagem de Fim de Simulacao
	pthread_mutex_lock(&Trinco_Comunicacao_Socket);
	Escreve_Mensagem(newsockfd,0,0,50,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
	printf("Enviou Mensagem Fim Simulacao!\n");
	pthread_mutex_unlock(&Trinco_Comunicacao_Socket);


	sleep(2);

	//Fechar Socket 
	close(newsockfd);


	return 0;

}







