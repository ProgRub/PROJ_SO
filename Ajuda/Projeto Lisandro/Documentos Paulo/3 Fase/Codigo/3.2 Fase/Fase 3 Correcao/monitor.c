#include "unix.h"

int sockfd;
int newsockfd;

pthread_t Thread_Recebe_Mensagens;

//Variaveis a Receber do Simulador (Para a Funcao "Mostra_Zonas_Tempo_Real"):
int Tamanho_Maximo_Bar=0;
int Tamanho_Maximo_Carroussel=0; 
int Tamanho_Maximo_Feira=0;

//------------------------------------------------------------------------------------------------------------------

//Funcao Para Ler do Ficheiro Config:
void Ler_Ficheiro_Config2()
{
	//Declaracao e Inicializacao de Variaveis Auxiliares da Funcao
	char Le_Linha[100];
	int Valor_Variavel_Lida;
	char Nome_Variavel_Lida[100];

	char Var1[]="Tam_Max_Bar"; //"Tamanho_Maximo_Bar";
	char Var2[]="Tam_Max_Carroussel"; //"Tamanho_Maximo_Carroussel";
	char Var3[]="Tam_Max_Feira"; //"Tamanho_Maximo_Feira";
	
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
				Tamanho_Maximo_Bar=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var2)==0)
			{
				Tamanho_Maximo_Carroussel=Valor_Variavel_Lida;
			}
			else if(strcmp(Nome_Variavel_Lida,Var3)==0)
			{
				Tamanho_Maximo_Feira=Valor_Variavel_Lida;
			}
			
		}
		fclose(fp);
	}

	else
	{
		printf("Erro Abertura Ficheiro");
		//abort();
	}

}


//Estatisticas:

//Total Clientes)

//                                  |----------------------------------------------------------------------------|
//                                  | Soma Tempos de Espera dos Clientes Antes de Entrarem e Antes de Desistirem |
//                                  |----------------------------------------------------------------------------|


//Soma Tempos de Espera dos Clientes Em Cada Fila de Cada Zona Antes de ser Atendido/Entrar/Andar nas Zonas
int Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido;//=0;
int Soma_Tempos_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa;//=0;
int Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel;//=0;
int Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira;//=0;

//Soma Tempos de Espera dos Clientes Antes de Desistirem da Fila de Cada Zona
int Soma_Tempos_Espera_Antes_Desistir_Fila_Bar;//=0;
int Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussa;//=0;
int Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel;//=0;
int Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras;//=0;




//                                  |------------------------------------------------------|
//                                  | Mais Variaveis Auxiliares Para Tratamento dos Tempos |
//                                  |------------------------------------------------------|


int Tempo_Espera_Filas_Bilheteiras_Antes_Entrar_Feira;
int Tempo_Espera_Filas_Bilheteiras_Antes_Desistir;
int Tempo_Espera_Fila_Bar_Antes_Ser_Atendido_Bar;
int Tempo_Espera_Fila_Bar_Antes_Desistir;
int Tempo_Espera_Fila_Carroussel_Antes_Entrar_Carroussel;
int Tempo_Espera_Fila_Carroussel_Antes_Desistir;
int Tempo_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa;
int Tempo_Espera_Fila_MontanhaRussa_Antes_Desistir;

//                                                          |----------------|
//                                                          | Tempos Maximos |
//                                                          |----------------|

//Tempo Maximo de Espera dos Clientes Antes de Desistirem da Fila de Cada Zona
int Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar;
int Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussa;
int Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel;
int Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras;

//Tempo Maximo de Espera dos Clientes Antes de Entrarem em Cada Zona
int Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar;
int Tempo_Maximo_Espera_Antes_Andar_MontanhaRussa;
int Tempo_Maximo_Espera_Antes_Entrar_Carroussel;
int Tempo_Maximo_Espera_Antes_Entrar_Feira;


//                                             |----------------------------------|
//                                             | Mais Variaveis Para Estatisticas |
//                                             |----------------------------------|

//Clientes Mobilidade Reduzida)

//Total de Clientes Mobilidade Reduzida que Entraram nas Filas
int Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar;
int Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel;
int Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras;

//Total de Clientes Mobilidade Reduzida que Foram Atendidos/Entraram/Andaram nas Zonas
int Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar;
int Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel;
int Total_Clientes_Mobilidade_Reduzida_Entraram_Feira;

//Total de Clientes Mobilidade Reduzida que Desistiram
int Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar;
int Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel;
int Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras;

//Clientes Crianças)

//Total de Clientes Crianças que Entraram nas Filas
int Total_Clientes_Criancas_Entraram_Fila_Bar;
int Total_Clientes_Criancas_Entraram_Fila_MontanhaRussa;
int Total_Clientes_Criancas_Entraram_Fila_Carroussel;
int Total_Clientes_Criancas_Entraram_Filas_Bilheteiras;

//Total de Clientes Crianças que Foram Atendidos/Entraram/Andaram nas Zonas
int Total_Clientes_Criancas_Atendidos_Bar; 
int Total_Clientes_Criancas_Andaram_MontanhaRussa;
int Total_Clientes_Criancas_Entraram_Carroussel;
int Total_Clientes_Criancas_Entraram_Feira;

//Total de Clientes Crianças que Desistiram
int Total_Clientes_Criancas_Desistiram_Fila_Bar;
int Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussa;
int Total_Clientes_Criancas_Desistiram_Fila_Carroussel;
int Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras;

//Clientes Publico Geral)

//Total de Clientes Publico Geral que Entraram nas Filas
int Total_Clientes_Publico_Geral_Entraram_Fila_Bar;
int Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussa;
int Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel;
int Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras;

//Total de Clientes Publico Geral que Foram Atendidos/Entraram/Andaram nas Zonas
int Total_Clientes_Publico_Geral_Atendidos_Bar;
int Total_Clientes_Publico_Geral_Andaram_MontanhaRussa;
int Total_Clientes_Publico_Geral_Entraram_Carroussel;
int Total_Clientes_Publico_Geral_Entraram_Feira;

//Total de Clientes Publico Geral que Desistiram
int Total_Clientes_Publico_Geral_Desistiram_Fila_Bar;
int Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRusssa;
int Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel;
int Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras;


//---------------------------------------------------------------------------------------------------------------------------------

//                                          |-------------------------------------------|
//                                          | Variaveis Para Estatisticas em Tempo Real |
//                                          |-------------------------------------------|

//Total de Clientes ACTUALMENTE nas Filas
int Total_Clientes_Acutalmente_Na_Fila_Bar;
int Total_Clientes_Acutalmente_Na_Fila_MontanhaRussa;
int Total_Clientes_Acutalmente_Na_Fila_Carroussel;
int Total_Clientes_Acutalmente_Nas_Filas_Bilheteiras;

//Total de Clientes ACTUALMENTE nas Zonas
int Total_Clientes_Acutalmente_No_Bar;
int Total_Clientes_Acutalmente_No_Carroussel;
int Total_Clientes_Acutalmente_Na_Feira;

//                                              |--------------------------------------|
//                                              | Variaveis Tempo Medio Estadia Feira  |
//                                              |--------------------------------------|

int Soma_Tempos_Estadia_Feira;
int Total_Clientes_Sairam_Feira;


//---------------------------------------------------------------------------------------------------------------------------------

//Variaveis da Simulacao:
bool Simulacao_Em_Curso=false; //Variavel Vai Indicar que Esta Uma Simulacao em Curso (Se For True Podemos Activar a Ocupacao das Zonas em Tempo Real)
int Escolha_Menu=0; //Variavel Que Guarda a Escolha do Utilizador. Quando É Igual a 7, Aplicacao Fecha. Tem de Ser Inicializado a 0 Por Causa da Funcao "Escreve_Cabecalho(int Escolha_Menu)"

//---------------------------------------------------------------------------------------------------------------------------------

// Cria Cabeçalho do Interface do Monitor
void Escreve_Cabecalho(int Escolha_Menu)
{
system("clear");
printf("                        ┌──────────────────────────────────────────────────────────────┐\n");
printf("                        │   Sistemas Operativos 2017/2018 ─ Simulacao Feira            │\n");
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



//Funcao Que Coloca Linhas em Branco Para Manter Efeito visual
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

//Auxiliar da Sub Funcao de Ocupcao das Zonas, Mas Que Trata Apenas das Filas
void Preenche_Ocupacao_Filas(int Total_Clientes_Actual, char* Zona)
{
		//printf("\n");
		printf("┌─────────────────────────┐\n");
		printf("%s │ =%d Clientes  \n", Zona, Total_Clientes_Actual);
		printf("└─────────────────────────┘\n");
}

//Auxiliar da Sub Funcao de Ocupacao das Zonas. Trata da Parte Estetica. Basicamente a Operacao "Trata_Ocupacao_Zonas_Tempo_Real" Vai Descodificar o Tipo de Zona Recebeido em Int e Vai Traduzir Para String. Esta Funcao "Preenche_Ocupacao" Vai Receber os Dados Numericos e a String com o Nome da Zona e Vai Mostrar a Ocupacao Dessa Zona
void Preenche_Ocupacao(int Tamanho_Maximo_Zona, int Total_Clientes_Actual, char* Zona)
{
	float Ocupacao_Actual;
//Salvaguarda Contra Tamanho das Zonas Ser ==0 <=> Erro De Divisao Por 0
	if(Tamanho_Maximo_Zona==0)
	{
		Ocupacao_Actual=0;
	}
	else
	{
		Ocupacao_Actual=Total_Clientes_Actual*100/(float)Tamanho_Maximo_Zona;
	}

//Se Nao Pusermos (float) no Denominador, Ele Faz a Divisao, Mas Apenas Mostra a Parte Inteira (O Que Vem Antes da Virgula)



	if(Ocupacao_Actual==0)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>0 && Ocupacao_Actual<=5)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>5 && Ocupacao_Actual<=10)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>10 && Ocupacao_Actual<=15)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>15 && Ocupacao_Actual<=20)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>20 && Ocupacao_Actual<=25)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │■ │  │  │  │  │  │  │  │  │  │  │  │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>25 && Ocupacao_Actual<=30)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │■ │■ │  │  │  │  │  │  │  │  │  │  │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>30 && Ocupacao_Actual<=35)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │■ │■ │■ │  │  │  │  │  │  │  │  │  │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>35 && Ocupacao_Actual<=40)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │■ │■ │■ │■ │  │  │  │  │  │  │  │  │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>40 && Ocupacao_Actual<=45)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │■ │■ │■ │■ │■ │  │  │  │  │  │  │  │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>45 && Ocupacao_Actual<=50)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │  │  │  │  │  │  │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>50 && Ocupacao_Actual<=55)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │  │  │  │  │  │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>55 && Ocupacao_Actual<=60)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │  │  │  │  │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>60 && Ocupacao_Actual<=65)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │  │  │  │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>65 && Ocupacao_Actual<=70)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │  │  │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>70 && Ocupacao_Actual<=75)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │  │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>75 && Ocupacao_Actual<=80)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │  │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>80 && Ocupacao_Actual<=85)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │  │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>85 && Ocupacao_Actual<=90)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │  │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>90 && Ocupacao_Actual<=95)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │  │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}
	else if(Ocupacao_Actual>95 && Ocupacao_Actual<=100)
	{
		printf("┌─────────────────────────┐───────────────────────────────────────────────────────────┐\n");
		printf("%s │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │■ │ (%.1f%% =%d Clientes|Max=%d)\n", Zona, Ocupacao_Actual,Total_Clientes_Actual,Tamanho_Maximo_Zona);
		printf("└─────────────────────────┘───────────────────────────────────────────────────────────┘\n");
	}

}

//Subfuncao Para Ajudar a Representar a Ocupacao das Zonas
void Trata_Ocupacao_Zonas_Tempo_Real(int Tamanho_Maximo_Zona_Recebido, int Total_Clientes_Actual_Recebido, int Tipo_Zona_Recebido)
{

	char Zona1[]="│    Lotacao Sala Bar    ";
	char Zona2[]="│     Lotacao Feira     ";
	char Zona3[]="│    Lotacao Carroussel     ";
	char Zona4[]="│    Lotacao Fila Bar    ";
	char Zona5[]="│ Lotacao Fila Montanha Russa ";
	char Zona6[]="│   Lotacao Fila Feira  ";
	char Zona7[]="│  Lotacao Fila Carroussel  ";

	switch(Tipo_Zona_Recebido)
	{	
		case 1: //1=Lotacao Bar
			Preenche_Ocupacao(Tamanho_Maximo_Zona_Recebido, Total_Clientes_Actual_Recebido, Zona1);
			break;
	//---------------------------------------------------------------------------------------
		case 2: //2=Lotacao Feira
			Preenche_Ocupacao(Tamanho_Maximo_Zona_Recebido, Total_Clientes_Actual_Recebido, Zona2);
			break;
	//---------------------------------------------------------------------------------------
		case 3: //3=Lotacao Carroussel
			Preenche_Ocupacao(Tamanho_Maximo_Zona_Recebido, Total_Clientes_Actual_Recebido, Zona3);
			break;
	//---------------------------------------------------------------------------------------
		case 4: //4=Lotacao Fila Bar
			Preenche_Ocupacao_Filas(Total_Clientes_Actual_Recebido, Zona4);
			break;
	//---------------------------------------------------------------------------------------
		case 5: //5=Lotacao Fila Montanha Russa
			Preenche_Ocupacao_Filas(Total_Clientes_Actual_Recebido, Zona5);
			break;
	//---------------------------------------------------------------------------------------
		case 6: //6=Lotacao Fila Feira
			Preenche_Ocupacao_Filas(Total_Clientes_Actual_Recebido, Zona6);
			break;
	//---------------------------------------------------------------------------------------
		case 7: //7=Lotacao Fila Carroussel
			Preenche_Ocupacao_Filas(Total_Clientes_Actual_Recebido, Zona7);
			break;
	//---------------------------------------------------------------------------------------
		default:
			printf("Erro: Tipo de Zona Incorrecto \n");
	}
}



//Funcao que Vai Mostrar a Ocupacao das Zonas em Tempo Real
void Mostra_Zonas_Tempo_Real(int Tamanho_Maximo_Bar,int Tamanho_Maximo_Carroussel,int Tamanho_Maximo_Feira,int Total_Clientes_Acutalmente_No_Bar,int Total_Clientes_Acutalmente_No_Carroussel,int Total_Clientes_Acutalmente_Na_Feira,int Total_Clientes_Acutalmente_Na_Fila_Bar,int Total_Clientes_Acutalmente_Na_Fila_MontanhaRussa,int Total_Clientes_Acutalmente_Na_Fila_Carroussel,int Total_Clientes_Acutalmente_Nas_Filas_Bilheteiras)
{
	Escreve_Cabecalho(Escolha_Menu);
printf("\n");
	Trata_Ocupacao_Zonas_Tempo_Real(Tamanho_Maximo_Bar,Total_Clientes_Acutalmente_No_Bar,1);
	Trata_Ocupacao_Zonas_Tempo_Real(Tamanho_Maximo_Feira,Total_Clientes_Acutalmente_Na_Feira,2);
	Trata_Ocupacao_Zonas_Tempo_Real(Tamanho_Maximo_Carroussel,Total_Clientes_Acutalmente_No_Carroussel,3);
printf("\n");
//Filas Nao Tem Tamanho Maximo, Logo Vamos Deixar a 0
	Trata_Ocupacao_Zonas_Tempo_Real(0,Total_Clientes_Acutalmente_Na_Fila_Bar,4);
	Trata_Ocupacao_Zonas_Tempo_Real(0,Total_Clientes_Acutalmente_Na_Fila_MontanhaRussa,5);
	Trata_Ocupacao_Zonas_Tempo_Real(0,Total_Clientes_Acutalmente_Nas_Filas_Bilheteiras,6);
	Trata_Ocupacao_Zonas_Tempo_Real(0,Total_Clientes_Acutalmente_Na_Fila_Carroussel,7);

	printf("\n");
	printf("\n");
	printf("\n");
}

//---------------------------------------------------------------------------------------------------------------------------------



//Funcao Que Converte "Tempo de Execucao" em "Tempo Real"
char * Converte_Tempo_Para_Horas(int Tempo)
{
		static char String[8];
		int Horas,Minutos,Segundos,Aux;
		
		Horas=Tempo/3600;
		Aux=Tempo%3600;
		Minutos=Aux/60;
		Segundos=Aux%60;	

		if(Tempo==0)
		{
			Horas=0;
			Minutos=0;
			sprintf(String,"%d:%d:%d",Horas,Minutos,Segundos);	
		}
		else if(Tempo!=0)
		{
			sprintf(String,"%d:%d:%d",Horas,Minutos,Segundos);	
		}
			return String;
}

//---------------------------------------------------------------------------------------------------------------------------------

//Funcao Para Preparar Ficheiro do Log Eventos (Invocado Quando o Utilizador Escolhe a Opcao de Simulacao no Menu Principal):
void Prepara_Ficheiro_Eventos()
{
	FILE *fp;
	fp=fopen("relatorio_eventos.c","w");

	fprintf(fp, "Eventos Feira: \n");
	fprintf(fp, "\n");

	fclose(fp);
}

//Funcao Para Escrever Eventos no Ficheiro Log (Precisamos da Funcao de "Preparacao" Anterior Pois Esta Funcao Vai Ser Invocada Sempre Que Houver um Evento. Dessa Forma Temos de Usar Append em Vez de Write, Senao Apagava Sempre o Conteudo do Ficheiro):
void Escreve_Ficheiro_Eventos(int Evento, int ID_Cliente, int Horas_Evento, int Tipo_Cliente, int Idade)
{
	char *Nome_Tipo_Cliente;
	char Tipo_Cliente1[]="(MR)";
	char Tipo_Cliente2[]="(Cri)";
	char Tipo_Cliente3[]="(PG)";
	if(Tipo_Cliente==1)
	{
		Nome_Tipo_Cliente=Tipo_Cliente1;
	}
	else if(Tipo_Cliente==2)
	{
		Nome_Tipo_Cliente=Tipo_Cliente2;
	}
	else if(Tipo_Cliente==3)
	{
		Nome_Tipo_Cliente=Tipo_Cliente3;
	}
	//ID's dos Clientes Começam no 0, Logo Temos de Por ID_Cliente+1

	FILE *fp;
	fp=fopen("relatorio_eventos.c","a"); //"a" = Append (Ou Seja, a Linha é Adicionada ao Ficheiro Sem Eliminar Nada)
	
	switch(Evento)
	{
		case 1:
		fprintf(fp, "->Cliente Nº %i (%s) Chegou a  Fila das Bilheteiras As: %s \n", ID_Cliente+1,Nome_Tipo_Cliente,Converte_Tempo_Para_Horas(Horas_Evento)); 
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 2:
		fprintf(fp, "->Cliente Nº %i Entrou na Feira As: %s \n", ID_Cliente+1,Converte_Tempo_Para_Horas(Horas_Evento));  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 3:
		fprintf(fp, "->Cliente Nº %i Desistiu da Fila das Bilheteiras As: %s \n", ID_Cliente+1, Converte_Tempo_Para_Horas(Horas_Evento));  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 4:
		fprintf(fp, "->Cliente Nº %i Chegou a Fila do Bar As: %s \n", ID_Cliente+1,Converte_Tempo_Para_Horas(Horas_Evento));  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 5:
		fprintf(fp, "->Cliente Nº %i Foi Atendido no Bar As: %s e Saiu do Bar \n", ID_Cliente+1, Converte_Tempo_Para_Horas(Horas_Evento));  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 6:
		fprintf(fp, "->Cliente Nº %i Foi Atendido no Bar As: %s E Ficou no Bar \n", ID_Cliente+1, Converte_Tempo_Para_Horas(Horas_Evento));  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 7:
		fprintf(fp, "->Cliente Nº %i Desistiu da Fila do Bar As: %s \n", ID_Cliente+1,Converte_Tempo_Para_Horas(Horas_Evento));  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 8:
		fprintf(fp, "->Cliente Nº %i Saiu da Sala do Bar As: %s \n", ID_Cliente+1,Converte_Tempo_Para_Horas(Horas_Evento));  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 9:
		fprintf(fp, "->Cliente Nº %i (Idade=%d) Chegou a Fila do Carroussel As: %s \n", ID_Cliente+1,Idade,Converte_Tempo_Para_Horas(Horas_Evento));  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 10:
		fprintf(fp, "->Cliente Nº %i (Idade=%d) Entrou no Carroussel As: %s \n", ID_Cliente+1,Idade,Converte_Tempo_Para_Horas(Horas_Evento));  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 11:
		fprintf(fp, "->Cliente Nº %i (Idade=%d) Desistiu da Fila do Carroussel As: %s \n", ID_Cliente+1,Idade,Converte_Tempo_Para_Horas(Horas_Evento));  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 12:
		fprintf(fp, "->Cliente Nº %i Chegou a Fila da Montanha Russa As: %s \n", ID_Cliente+1,Converte_Tempo_Para_Horas(Horas_Evento));  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 13:
		fprintf(fp, "->Cliente Nº %i Andou na Montanha Russa As: %s \n", ID_Cliente+1,Converte_Tempo_Para_Horas(Horas_Evento));  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 14:
		fprintf(fp, "->Cliente Nº %i Desistiu da Fila da Montanha Russa As: %s \n", ID_Cliente+1, Converte_Tempo_Para_Horas(Horas_Evento));  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 15:
		fprintf(fp, "->Cliente Nº %i Saiu da Feira As: %s \n", ID_Cliente+1,Converte_Tempo_Para_Horas(Horas_Evento));  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 16:
		fprintf(fp, "->Cliente Nº %i (Idade=%d) Saiu do Carroussel As: %s \n", ID_Cliente+1,Idade, Converte_Tempo_Para_Horas(Horas_Evento));  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------		
		case 50:
		fprintf(fp, "*Simulacao Terminada* \n");  
		fprintf(fp, "\n");
		//fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		default:
		fprintf(fp, "-Erro: Evento Nao Definido\n"); 
		fprintf(fp, "-----------------------------------------------------\n");
	}
	fclose(fp);
}

//---------------------------------------------------------------------------------------------------------------------------------

//Funcao que Mostra o Log de Eventos em Tempo Real
void Mostra_No_Monitor_Eventos_Tempo_Real(int Evento, int ID_Cliente, int Horas_Evento, int Tipo_Cliente, int Idade)
{
//☺=Chegou Fila
//☹=Desistiu Fila
//😃=Entrou/Atendido/Andou

	char *Nome_Tipo_Cliente;
	char Tipo_Cliente1[]="(MR)";
	char Tipo_Cliente2[]="(Cri)";
	char Tipo_Cliente3[]="(PG)";
	if(Tipo_Cliente==1)
	{
		Nome_Tipo_Cliente=Tipo_Cliente1;
	}
	else if(Tipo_Cliente==2)
	{
		Nome_Tipo_Cliente=Tipo_Cliente2;
	}
	else if(Tipo_Cliente==3)
	{
		Nome_Tipo_Cliente=Tipo_Cliente3;
	}
	switch(Evento)
	{
		case 1:
		printf("│ ☺  Cliente Nº%d %s Chegou a Fila das Bilheteiras As %s  \n",ID_Cliente+1,Nome_Tipo_Cliente,Converte_Tempo_Para_Horas(Horas_Evento));

		break;
	//---------------------------------------------------------------------------------------------------------
		case 2:
		printf("│ 😃  Cliente Nº%d %s Entrou na Feira As %s  \n",ID_Cliente+1,Nome_Tipo_Cliente,Converte_Tempo_Para_Horas(Horas_Evento));

		break;
	//---------------------------------------------------------------------------------------------------------
		case 3:
		printf("│ ☹  Cliente Nº%d %s Desistiu da Fila das Bilheteiras As %s  \n",ID_Cliente+1,Nome_Tipo_Cliente,Converte_Tempo_Para_Horas(Horas_Evento));

		break;
	//---------------------------------------------------------------------------------------------------------
		case 4:
		printf("│ ☺  Cliente Nº%d %s Chegou a Fila do Bar As %s  \n",ID_Cliente+1,Nome_Tipo_Cliente,Converte_Tempo_Para_Horas(Horas_Evento));

		break;
	//---------------------------------------------------------------------------------------------------------
		case 5:
		printf("│ 😃  Cliente Nº%d %s Foi Atendido no Bar As %s e Saiu do Bar  \n",ID_Cliente+1,Nome_Tipo_Cliente,Converte_Tempo_Para_Horas(Horas_Evento));

		break;
	//---------------------------------------------------------------------------------------------------------
		case 6:
		printf("│ 😃  Cliente Nº%d %s Foi Atendido no Bar As %s e Ficou no Bar  \n",ID_Cliente+1,Nome_Tipo_Cliente,Converte_Tempo_Para_Horas(Horas_Evento));

		break;
	//---------------------------------------------------------------------------------------------------------
		case 7:
		printf("│ ☹  Cliente Nº%d %s Desistiu da Fila do Bar As %s  \n",ID_Cliente+1,Nome_Tipo_Cliente,Converte_Tempo_Para_Horas(Horas_Evento));

		break;
	//---------------------------------------------------------------------------------------------------------
		case 8:
		printf("│ ☹  Cliente Nº%d %s Saiu da Sala do Bar As %s  \n",ID_Cliente+1,Nome_Tipo_Cliente,Converte_Tempo_Para_Horas(Horas_Evento));

		break;
	//---------------------------------------------------------------------------------------------------------
		case 9:
		printf("│ ☺  Cliente Nº%d %s (Idade=%d) Chegou a Fila do Carroussel As %s  \n",ID_Cliente+1,Nome_Tipo_Cliente,Idade,Converte_Tempo_Para_Horas(Horas_Evento));

		break;
	//---------------------------------------------------------------------------------------------------------
		case 10:
		printf("│ 😃  Cliente Nº%d %s (Idade=%d) Entrou no Carroussel As %s  \n",ID_Cliente+1,Nome_Tipo_Cliente,Idade,Converte_Tempo_Para_Horas(Horas_Evento));

		break;
	//---------------------------------------------------------------------------------------------------------
		case 11:
		printf("│ ☹  Cliente Nº%d %s (Idade=%d) Desistiu da Fila do Carroussel As %s  \n",ID_Cliente+1,Nome_Tipo_Cliente,Idade,Converte_Tempo_Para_Horas(Horas_Evento));

		break;
	//---------------------------------------------------------------------------------------------------------
		case 12:
		printf("│ ☺  Cliente Nº%d %s Chegou a Fila da Montanha Russa As %s  \n",ID_Cliente+1,Nome_Tipo_Cliente,Converte_Tempo_Para_Horas(Horas_Evento));

		break;
	//---------------------------------------------------------------------------------------------------------
		case 13:
		printf("│ 😃  Cliente Nº%d %s Andou na Montanha Russa As %s  \n",ID_Cliente+1,Nome_Tipo_Cliente,Converte_Tempo_Para_Horas(Horas_Evento));

		break;
	//---------------------------------------------------------------------------------------------------------
		case 14:
		printf("│ ☹  Cliente Nº%d %s Desistiu da Fila da Montanha Russa As %s  \n",ID_Cliente+1,Nome_Tipo_Cliente,Converte_Tempo_Para_Horas(Horas_Evento));

		break;
	//---------------------------------------------------------------------------------------------------------
		case 15:
		printf("│ ☹  Cliente Nº%d %s Saiu da Feira As %s  \n",ID_Cliente+1,Nome_Tipo_Cliente,Converte_Tempo_Para_Horas(Horas_Evento));

		break;
	//---------------------------------------------------------------------------------------------------------
		case 16:
		printf("│ ☹  Cliente Nº%d %s (Idade=%d) Saiu do Carroussel As %s  \n",ID_Cliente+1,Nome_Tipo_Cliente,Idade,Converte_Tempo_Para_Horas(Horas_Evento));

		break;
	//---------------------------------------------------------------------------------------------------------		
		case 50:
		printf("│ Simulacao Terminada!  \n");

		break;
	//---------------------------------------------------------------------------------------------------------
		default:
		printf("│ Erro: Caso Nao Definido  \n");
	}
}

//---------------------------------------------------------------------------------------------------------------------------------


//Funcao Para Mostrar as Estatisticas da Feira no Monitor: (Dependendo dos Valores, os Floats Podem Ter Valores Muito Baixos Pois So Consideramos a Parte Inteira e Nao Ha Arredondamentos ao Exagero)
void Mostra_Estatisticas_Monitor(int Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar,int Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussa,int Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel,int Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras,int Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar,int Tempo_Maximo_Espera_Antes_Andar_MontanhaRussa,int Tempo_Maximo_Espera_Antes_Entrar_Carroussel,int Tempo_Maximo_Espera_Antes_Entrar_Feira,int Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido,int Soma_Tempos_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa,int Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel,int Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira,int Soma_Tempos_Espera_Antes_Desistir_Fila_Bar,int Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussa,int Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel,int Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras,int Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar,int Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel,int Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras,int Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar,int Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel,int Total_Clientes_Mobilidade_Reduzida_Entraram_Feira,int Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar,int Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel,int Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras,int Total_Clientes_Criancas_Entraram_Fila_Bar,int Total_Clientes_Criancas_Entraram_Fila_MontanhaRussa,int Total_Clientes_Criancas_Entraram_Fila_Carroussel,int Total_Clientes_Criancas_Entraram_Filas_Bilheteiras,int Total_Clientes_Criancas_Atendidos_Bar,int Total_Clientes_Criancas_Andaram_MontanhaRussa,int Total_Clientes_Criancas_Entraram_Carroussel,int Total_Clientes_Criancas_Entraram_Feira,int Total_Clientes_Criancas_Desistiram_Fila_Bar,int Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussa,int Total_Clientes_Criancas_Desistiram_Fila_Carroussel,int Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras,int Total_Clientes_Publico_Geral_Entraram_Fila_Bar,int Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussa,int Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel,int Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras,int Total_Clientes_Publico_Geral_Atendidos_Bar,int Total_Clientes_Publico_Geral_Andaram_MontanhaRussa,int Total_Clientes_Publico_Geral_Entraram_Carroussel,int Total_Clientes_Publico_Geral_Entraram_Feira,int Total_Clientes_Publico_Geral_Desistiram_Fila_Bar,int Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRusssa,int Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel,int Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras,int Soma_Tempos_Estadia_Feira,int Total_Clientes_Sairam_Feira)
{

	Escreve_Cabecalho(Escolha_Menu);

//---------------------------------------------------------------------------------------------------------------------------------
//1º Fazer Soma de Totais de Clientes dos Varios Tipos
//2º Fazer Medias de Tempos
//---------------------------------------------------------------------------------------------------------------------------------

//Soma Totais:
//Total de Clientes que Foram Atendidos/Entraram/Andaram nas Zonas
int Total_Clientes_Atendidos_Bar=Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar+Total_Clientes_Criancas_Atendidos_Bar+Total_Clientes_Publico_Geral_Atendidos_Bar;
int Total_Clientes_Andaram_MontanhaRussa=Total_Clientes_Criancas_Andaram_MontanhaRussa+Total_Clientes_Publico_Geral_Andaram_MontanhaRussa;
int Total_Clientes_Entraram_Carroussel=Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel+Total_Clientes_Criancas_Entraram_Carroussel+Total_Clientes_Publico_Geral_Entraram_Carroussel;
int Total_Clientes_Entraram_Feira=Total_Clientes_Mobilidade_Reduzida_Entraram_Feira+Total_Clientes_Criancas_Entraram_Feira+Total_Clientes_Publico_Geral_Entraram_Feira;

//Total de Clientes que Entraram nas Filas
int Total_Clientes_Entraram_Fila_Bar=Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar+Total_Clientes_Criancas_Entraram_Fila_Bar+Total_Clientes_Publico_Geral_Entraram_Fila_Bar;
int Total_Clientes_Entraram_Fila_MontanhaRussa=Total_Clientes_Criancas_Entraram_Fila_MontanhaRussa+Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussa;
int Total_Clientes_Entraram_Fila_Carroussel=Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel+Total_Clientes_Criancas_Entraram_Fila_Carroussel+Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel;
int Total_Clientes_Entraram_Filas_Bilheteiras=Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras+Total_Clientes_Criancas_Entraram_Filas_Bilheteiras+Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras;

//Total de Clientes que Desistiram
int Total_Clientes_Desistiram_Fila_Bar=Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar+Total_Clientes_Criancas_Desistiram_Fila_Bar+Total_Clientes_Publico_Geral_Desistiram_Fila_Bar;
int Total_Clientes_Desistiram_Fila_MontanhaRussa=Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussa+Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRusssa;
int Total_Clientes_Desistiram_Fila_Carroussel=Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel+Total_Clientes_Criancas_Desistiram_Fila_Carroussel+Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel;
int Total_Clientes_Desistiram_Filas_Bilheteiras=Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras+Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras+Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras;

//----------------------------------------------------------------------------------------------------------------------------------

//Tempos Medios:
//Tempos Medios nas Filas Antes Clientes Serem Atendidos/Entrarem/Andarem nas Zonas
float Tempo_Medio_Antes_Ser_Atendido_Bar=0;//=Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido/Total_Clientes_Atendidos_Bar;
float Tempo_Medio_Antes_Andar_MontanhaRussa=0;
float Tempo_Medio_Antes_Entrar_Carroussel=0;
float Tempo_Medio_Antes_Entrar_Feira=0;

//Tempos Medios nas Filas Antes Clientes Desistirem das Filas das Zonas
float Tempo_Medio_Antes_Desistir_Fila_Bar=0;//=Soma_Tempos_Espera_Antes_Desistir_Fila_Bar/Total_Clientes_Desistiram_Fila_Bar;
float Tempo_Medio_Antes_Desistir_Fila_MontanhaRussa=0;
float Tempo_Medio_Antes_Desistir_Fila_Carroussel=0;
float Tempo_Medio_Antes_Desistir_Filas_Bilheteiras=0;


if(Total_Clientes_Atendidos_Bar==0) {Tempo_Medio_Antes_Ser_Atendido_Bar=0;} else {Tempo_Medio_Antes_Ser_Atendido_Bar=Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido/Total_Clientes_Atendidos_Bar;};
if(Total_Clientes_Andaram_MontanhaRussa==0) {Tempo_Medio_Antes_Andar_MontanhaRussa=0;} else {Tempo_Medio_Antes_Andar_MontanhaRussa=Soma_Tempos_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa/Total_Clientes_Andaram_MontanhaRussa;};
if(Total_Clientes_Entraram_Carroussel==0) {Tempo_Medio_Antes_Entrar_Carroussel=0;} else {Tempo_Medio_Antes_Entrar_Carroussel=Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel/Total_Clientes_Entraram_Carroussel;};
if(Total_Clientes_Entraram_Feira==0) {Tempo_Medio_Antes_Entrar_Feira=0;} else {Tempo_Medio_Antes_Entrar_Feira=Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira/Total_Clientes_Entraram_Feira;};

if(Total_Clientes_Desistiram_Fila_Bar==0) {Tempo_Medio_Antes_Desistir_Fila_Bar=0;} else {Tempo_Medio_Antes_Desistir_Fila_Bar=Soma_Tempos_Espera_Antes_Desistir_Fila_Bar/Total_Clientes_Desistiram_Fila_Bar;};
if(Total_Clientes_Desistiram_Fila_MontanhaRussa==0) {Tempo_Medio_Antes_Desistir_Fila_MontanhaRussa=0;} else {Tempo_Medio_Antes_Desistir_Fila_MontanhaRussa=Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussa/Total_Clientes_Desistiram_Fila_MontanhaRussa;};
if(Total_Clientes_Desistiram_Fila_Carroussel==0) {Tempo_Medio_Antes_Desistir_Fila_Carroussel=0;} else {Tempo_Medio_Antes_Desistir_Fila_Carroussel=Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel/Total_Clientes_Desistiram_Fila_Carroussel;};
if(Total_Clientes_Desistiram_Filas_Bilheteiras==0) {Tempo_Medio_Antes_Desistir_Filas_Bilheteiras=0;} else {Tempo_Medio_Antes_Desistir_Filas_Bilheteiras=Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras/Total_Clientes_Desistiram_Filas_Bilheteiras;};

//Tempo Medio Estadia na Feira
float Tempo_Medio_Estadia_Feira=0;
if(Total_Clientes_Sairam_Feira==0) {Tempo_Medio_Estadia_Feira=0;} else {Tempo_Medio_Estadia_Feira=Soma_Tempos_Estadia_Feira/Total_Clientes_Sairam_Feira;};

//Incluir Um If Para Cada 1 Destas Variaveis de Tempos Medios. Se o Total de CLientes Atendidos ou que Desistiram For=0, o Tempo Medio=0 (Para Evitar Erro de Divisao Por 0)

	//printf("─────────────────────────────[ Estatisticas ]─────────────────────────────\n");
	printf("│ Total Clientes Atendidos Bar: %d \n", Total_Clientes_Atendidos_Bar);
	printf("│ Total Clientes Andaram Montanha Russa: %d \n",Total_Clientes_Andaram_MontanhaRussa );
	printf("│ Total Clientes Entraram Carroussel: %d \n",Total_Clientes_Entraram_Carroussel );
	printf("│ Total Clientes Entraram Feira: %d \n",Total_Clientes_Entraram_Feira );
	
	//Variavel de Controlo Para Ver Se Todos os Clientes Sairam da Feira
	printf("│ Total Clientes Sairam Feira: %d \n",Total_Clientes_Sairam_Feira );

	printf("│ Total Clientes Entraram Fila Bar: %d \n", Total_Clientes_Entraram_Fila_Bar);
	printf("│ Total Clientes Entraram Fila Montanha Russa: %d \n",Total_Clientes_Entraram_Fila_MontanhaRussa );
	printf("│ Total Clientes Entraram Fila Carroussel: %d \n",Total_Clientes_Entraram_Fila_Carroussel );
	printf("│ Total Clientes Entraram Fila Feira: %d \n",Total_Clientes_Entraram_Filas_Bilheteiras );

	printf("│ Total Clientes Desistiram Fila Bar: %d \n", Total_Clientes_Desistiram_Fila_Bar);
	printf("│ Total Clientes Desistiram Fila Montanha Russa: %d \n",Total_Clientes_Desistiram_Fila_MontanhaRussa );
	printf("│ Total Clientes Desistiram Fila Carroussel: %d \n",Total_Clientes_Desistiram_Fila_Carroussel );
	printf("│ Total Clientes Desistiram Fila Feira: %d \n",Total_Clientes_Desistiram_Filas_Bilheteiras );

	printf("│ Tempo Maximo Antes Entrar Feira: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Entrar_Feira));
	printf("│ Tempo Maximo Antes Desistir Fila Bilheteiras: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras));

	printf("│ Tempo Medio Antes Ser Atendido Bar: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Ser_Atendido_Bar));
	printf("│ Tempo Medio Antes Andar Montanha: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Andar_MontanhaRussa));
	printf("│ Tempo Medio Antes Entrar Carroussel: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Entrar_Carroussel));
	printf("│ Tempo Medio Antes Entrar Feira: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Entrar_Feira));

	printf("│ Tempo Medio Antes Desistir Fila Bar: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Desistir_Fila_Bar));
	printf("│ Tempo Medio Antes Desistir Fila Montanha Russa: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Desistir_Fila_MontanhaRussa));
	printf("│ Tempo Medio Antes Desistir Fila Carroussel: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Desistir_Fila_Carroussel));
	printf("│ Tempo Medio Antes Desistir Fila Bilheteiras: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Desistir_Filas_Bilheteiras));

	printf("│ Tempo Medio Estadia Feira: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Estadia_Feira));

	Coloca_Linhas_Em_Branco(0);
}

//Funcao Para Escrever as Estatisticas da Ffeira no Ficheiro Estatisticas (So se Invoca no Final da Simulacao. Esta Funcao Nao Precisa de Uma Funcao "Preparatoria" Porque So Vai Ser Invocada Uma Vez No Final.):


void Escreve_Estatisticas(int Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar,int Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussa,int Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel,int Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras,int Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar,int Tempo_Maximo_Espera_Antes_Andar_MontanhaRussa,int Tempo_Maximo_Espera_Antes_Entrar_Carroussel,int Tempo_Maximo_Espera_Antes_Entrar_Feira,int Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido,int Soma_Tempos_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa,int Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel,int Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira,int Soma_Tempos_Espera_Antes_Desistir_Fila_Bar,int Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussa,int Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel,int Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras,int Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar,int Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel,int Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras,int Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar,int Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel,int Total_Clientes_Mobilidade_Reduzida_Entraram_Feira,int Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar,int Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel,int Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras,int Total_Clientes_Criancas_Entraram_Fila_Bar,int Total_Clientes_Criancas_Entraram_Fila_MontanhaRussa,int Total_Clientes_Criancas_Entraram_Fila_Carroussel,int Total_Clientes_Criancas_Entraram_Filas_Bilheteiras,int Total_Clientes_Criancas_Atendidos_Bar,int Total_Clientes_Criancas_Andaram_MontanhaRussa,int Total_Clientes_Criancas_Entraram_Carroussel,int Total_Clientes_Criancas_Entraram_Feira,int Total_Clientes_Criancas_Desistiram_Fila_Bar,int Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussa,int Total_Clientes_Criancas_Desistiram_Fila_Carroussel,int Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras,int Total_Clientes_Publico_Geral_Entraram_Fila_Bar,int Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussa,int Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel,int Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras,int Total_Clientes_Publico_Geral_Atendidos_Bar,int Total_Clientes_Publico_Geral_Andaram_MontanhaRussa,int Total_Clientes_Publico_Geral_Entraram_Carroussel,int Total_Clientes_Publico_Geral_Entraram_Feira,int Total_Clientes_Publico_Geral_Desistiram_Fila_Bar,int Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussa,int Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel,int Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras,int Soma_Tempos_Estadia_Feira,int Total_Clientes_Sairam_Feira)
{

//---------------------------------------------------------------------------------------------------------------------------------
//1º Fazer Soma de Totais de Clientes dos Varios Tipos
//2º Fazer Medias de Tempos
//---------------------------------------------------------------------------------------------------------------------------------

//Soma Totais:
//Total de Clientes que Foram Atendidos/Entraram/Andaram nas Zonas
int Total_Clientes_Atendidos_Bar=Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar+Total_Clientes_Criancas_Atendidos_Bar+Total_Clientes_Publico_Geral_Atendidos_Bar;
int Total_Clientes_Andaram_MontanhaRussa=Total_Clientes_Criancas_Andaram_MontanhaRussa+Total_Clientes_Publico_Geral_Andaram_MontanhaRussa;
int Total_Clientes_Entraram_Carroussel=Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel+Total_Clientes_Criancas_Entraram_Carroussel+Total_Clientes_Publico_Geral_Entraram_Carroussel;
int Total_Clientes_Entraram_Feira=Total_Clientes_Mobilidade_Reduzida_Entraram_Feira+Total_Clientes_Criancas_Entraram_Feira+Total_Clientes_Publico_Geral_Entraram_Feira;

//Total de Clientes que Entraram nas Filas
int Total_Clientes_Entraram_Fila_Bar=Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar+Total_Clientes_Criancas_Entraram_Fila_Bar+Total_Clientes_Publico_Geral_Entraram_Fila_Bar;
int Total_Clientes_Entraram_Fila_MontanhaRussa=Total_Clientes_Criancas_Entraram_Fila_MontanhaRussa+Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussa;
int Total_Clientes_Entraram_Fila_Carroussel=Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel+Total_Clientes_Criancas_Entraram_Fila_Carroussel+Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel;
int Total_Clientes_Entraram_Filas_Bilheteiras=Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras+Total_Clientes_Criancas_Entraram_Filas_Bilheteiras+Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras;

//Total de Clientes que Desistiram
int Total_Clientes_Desistiram_Fila_Bar=Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar+Total_Clientes_Criancas_Desistiram_Fila_Bar+Total_Clientes_Publico_Geral_Desistiram_Fila_Bar;
int Total_Clientes_Desistiram_Fila_MontanhaRussa=Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussa+Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRusssa;
int Total_Clientes_Desistiram_Fila_Carroussel=Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel+Total_Clientes_Criancas_Desistiram_Fila_Carroussel+Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel;
int Total_Clientes_Desistiram_Filas_Bilheteiras=Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras+Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras+Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras;

//---------------------------------------------------------------------------------------------------------------------------------

//Tempos Medios:
//Tempos Medios nas Filas Antes Clientes Serem Atendidos/Entrarem/Andarem nas Zonas
float Tempo_Medio_Antes_Ser_Atendido_Bar=0;//=Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido/Total_Clientes_Atendidos_Bar;
float Tempo_Medio_Antes_Andar_MontanhaRussa=0;
float Tempo_Medio_Antes_Entrar_Carroussel=0;
float Tempo_Medio_Antes_Entrar_Feira=0;

//Tempos Medios nas Filas Antes Clientes Desistirem das Filas das Zonas
float Tempo_Medio_Antes_Desistir_Fila_Bar=0;//=Soma_Tempos_Espera_Antes_Desistir_Fila_Bar/Total_Clientes_Desistiram_Fila_Bar;
float Tempo_Medio_Antes_Desistir_Fila_MontanhaRussa=0;
float Tempo_Medio_Antes_Desistir_Fila_Carroussel=0;
float Tempo_Medio_Antes_Desistir_Filas_Bilheteiras=0;

if(Total_Clientes_Atendidos_Bar==0) {Tempo_Medio_Antes_Ser_Atendido_Bar=0;} else {Tempo_Medio_Antes_Ser_Atendido_Bar=Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido/Total_Clientes_Atendidos_Bar;};
if(Total_Clientes_Andaram_MontanhaRussa==0) {Tempo_Medio_Antes_Andar_MontanhaRussa=0;} else {Tempo_Medio_Antes_Andar_MontanhaRussa=Soma_Tempos_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa/Total_Clientes_Andaram_MontanhaRussa;};
if(Total_Clientes_Entraram_Carroussel==0) {Tempo_Medio_Antes_Entrar_Carroussel=0;} else {Tempo_Medio_Antes_Entrar_Carroussel=Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel/Total_Clientes_Entraram_Carroussel;};
if(Total_Clientes_Entraram_Feira==0) {Tempo_Medio_Antes_Entrar_Feira=0;} else {Tempo_Medio_Antes_Entrar_Feira=Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira/Total_Clientes_Entraram_Feira;};

if(Total_Clientes_Desistiram_Fila_Bar==0) {Tempo_Medio_Antes_Desistir_Fila_Bar=0;} else {Tempo_Medio_Antes_Desistir_Fila_Bar=Soma_Tempos_Espera_Antes_Desistir_Fila_Bar/Total_Clientes_Desistiram_Fila_Bar;};
if(Total_Clientes_Desistiram_Fila_MontanhaRussa==0) {Tempo_Medio_Antes_Desistir_Fila_MontanhaRussa=0;} else {Tempo_Medio_Antes_Desistir_Fila_MontanhaRussa=Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussa/Total_Clientes_Desistiram_Fila_MontanhaRussa;};
if(Total_Clientes_Desistiram_Fila_Carroussel==0) {Tempo_Medio_Antes_Desistir_Fila_Carroussel=0;} else {Tempo_Medio_Antes_Desistir_Fila_Carroussel=Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel/Total_Clientes_Desistiram_Fila_Carroussel;};
if(Total_Clientes_Desistiram_Filas_Bilheteiras==0) {Tempo_Medio_Antes_Desistir_Filas_Bilheteiras=0;} else {Tempo_Medio_Antes_Desistir_Filas_Bilheteiras=Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras/Total_Clientes_Desistiram_Filas_Bilheteiras;};


//Tempo Medio Estadia na Feira
float Tempo_Medio_Estadia_Feira=0;
if(Total_Clientes_Sairam_Feira==0) {Tempo_Medio_Estadia_Feira=0;} else {Tempo_Medio_Estadia_Feira=Soma_Tempos_Estadia_Feira/Total_Clientes_Sairam_Feira;};


//Incluir Um If Para Cada 1 Destas Variaveis de Tempos Medios. Se o Total de CLientes Atendidos ou que Desistiram For=0, o Tempo Medio=0 (Para Evitar Erro de Divisao Por 0)

	FILE *fp;
	fp=fopen("estatisticas.c","w");

	fprintf(fp, "Estatisticas: \n");
	fprintf(fp, "\n");
	fprintf(fp, "|-------------------------------------------------------| \n");
	fprintf(fp, "|-------------------------------------------------------| \n");
	fprintf(fp, "|----------- Clientes Atendidos no Bar -----------------| \n");
	fprintf(fp, "|-------------------------------------------------------| \n");
	fprintf(fp, "|-------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Atendidos Bar: %d \n", Total_Clientes_Atendidos_Bar);
	fprintf(fp, "Total Clientes Mobilidade Reduzida Atendidos Bar: %d \n", Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar);
	fprintf(fp, "Total Clientes Crianças Atendidos Bar: %d \n", Total_Clientes_Criancas_Atendidos_Bar);
	fprintf(fp, "Total Clientes Publico Geral Atendidos Bar: %d \n", Total_Clientes_Publico_Geral_Atendidos_Bar);
	fprintf(fp, "\n");
	fprintf(fp, "|-----------------------------------------------------------| \n");
	fprintf(fp, "|-----------------------------------------------------------| \n");
	fprintf(fp, "|-------- Clientes Andaram na Montanha Russa ---------------| \n");
	fprintf(fp, "|-----------------------------------------------------------| \n");
	fprintf(fp, "|-----------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Andaram Montanha Russa: %d \n",Total_Clientes_Andaram_MontanhaRussa );
	fprintf(fp, "Total Clientes Crianças Andaram Montanha Russa: %d \n",Total_Clientes_Criancas_Andaram_MontanhaRussa );
	fprintf(fp, "Total Clientes Publico Geral Andaram Montanha Russa: %d \n",Total_Clientes_Publico_Geral_Andaram_MontanhaRussa );
	fprintf(fp, "\n");
	fprintf(fp, "|----------------------------------------------------------| \n");
	fprintf(fp, "|----------------------------------------------------------| \n");
	fprintf(fp, "|----------- Clientes Entraram no Carroussel---------------| \n");
	fprintf(fp, "|----------------------------------------------------------| \n");
	fprintf(fp, "|----------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Entraram Carroussel: %d \n",Total_Clientes_Entraram_Carroussel );
	fprintf(fp, "Total Clientes Mobilidade Reduzida Entraram Carroussel: %d \n",Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel );
	fprintf(fp, "Total Clientes Crianças Entraram Carroussel: %d \n",Total_Clientes_Criancas_Entraram_Carroussel );
	fprintf(fp, "Total Clientes Publico Geral Entraram Carroussel: %d \n",Total_Clientes_Publico_Geral_Entraram_Carroussel );
	fprintf(fp, "\n");
	fprintf(fp, "|---------------------------------------------------------| \n");
	fprintf(fp, "|---------------------------------------------------------| \n");
	fprintf(fp, "|----------- Clientes Entraram na Feira ------------------| \n");
	fprintf(fp, "|---------------------------------------------------------| \n");
	fprintf(fp, "|---------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Entraram Feira: %d \n",Total_Clientes_Entraram_Feira );
	fprintf(fp, "Total Clientes Mobilidade Reduzida Entraram Feira: %d \n",Total_Clientes_Mobilidade_Reduzida_Entraram_Feira );
	fprintf(fp, "Total Clientes Crianças Entraram Feira: %d \n",Total_Clientes_Criancas_Entraram_Feira );
	fprintf(fp, "Total Clientes Publico Geral Entraram Feira: %d \n",Total_Clientes_Publico_Geral_Entraram_Feira );
	fprintf(fp, "\n");


	fprintf(fp, "|--------------------------------------------------------------| \n");
	fprintf(fp, "|--------------------------------------------------------------| \n");
	fprintf(fp, "|----------- Clientes Entraram na Fila do Bar -----------------| \n");
	fprintf(fp, "|--------------------------------------------------------------| \n");
	fprintf(fp, "|--------------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Entraram Fila Bar: %d \n", Total_Clientes_Entraram_Fila_Bar);
	fprintf(fp, "Total Clientes Mobilidade Reduzida Entraram Fila Bar: %d \n", Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar);
	fprintf(fp, "Total Clientes Crianças Entraram Fila Bar: %d \n", Total_Clientes_Criancas_Entraram_Fila_Bar);
	fprintf(fp, "Total Clientes Publico Geral Entraram Fila Bar: %d \n", Total_Clientes_Publico_Geral_Entraram_Fila_Bar);
	fprintf(fp, "\n");
	fprintf(fp, "|--------------------------------------------------------------------| \n");
	fprintf(fp, "|--------------------------------------------------------------------| \n");
	fprintf(fp, "|-------- Clientes Entraram na Fila do Montanha Russa ---------------| \n");
	fprintf(fp, "|--------------------------------------------------------------------| \n");
	fprintf(fp, "|--------------------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Entraram Fila Montanha Russa: %d \n",Total_Clientes_Entraram_Fila_MontanhaRussa );
	fprintf(fp, "Total Clientes Crianças Entraram Fila Montanha Russa: %d \n",Total_Clientes_Criancas_Entraram_Fila_MontanhaRussa );
	fprintf(fp, "Total Clientes Publico Geral Entraram Fila Montanha Russa: %d \n",Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussa );
	fprintf(fp, "\n");
	fprintf(fp, "|------------------------------------------------------------------| \n");
	fprintf(fp, "|------------------------------------------------------------------| \n");
	fprintf(fp, "|----------- Clientes Entraram na Fila da Carroussel---------------| \n");
	fprintf(fp, "|------------------------------------------------------------------| \n");
	fprintf(fp, "|------------------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Entraram Fila Carroussel: %d \n",Total_Clientes_Entraram_Fila_Carroussel );
	fprintf(fp, "Total Clientes Mobilidade Reduzida Entraram Fila Carroussel: %d \n",Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel );
	fprintf(fp, "Total Clientes Crianças Entraram Fila Carroussel: %d \n",Total_Clientes_Criancas_Entraram_Fila_Carroussel );
	fprintf(fp, "Total Clientes Publico Geral Entraram Fila Carroussel: %d \n",Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel );
	fprintf(fp, "\n");
	fprintf(fp, "|-----------------------------------------------------------------------| \n");
	fprintf(fp, "|-----------------------------------------------------------------------| \n");
	fprintf(fp, "|----------- Clientes Entraram na Fila das Bilheteiras -----------------| \n");
	fprintf(fp, "|-----------------------------------------------------------------------| \n");
	fprintf(fp, "|-----------------------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Entraram Fila Bilheteiras: %d \n",Total_Clientes_Entraram_Filas_Bilheteiras );
	fprintf(fp, "Total Clientes Mobilidade Reduzida Entraram Fila Bilheteiras: %d \n",Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras );
	fprintf(fp, "Total Clientes Crianças Entraram Fila Bilheteiras: %d \n",Total_Clientes_Criancas_Entraram_Filas_Bilheteiras );
	fprintf(fp, "Total Clientes Publico Geral Entraram Fila Bilheteiras: %d \n",Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras );
	fprintf(fp, "\n");

	fprintf(fp, "|----------------------------------------------------------------| \n");
	fprintf(fp, "|----------------------------------------------------------------| \n");
	fprintf(fp, "|----------- Clientes Desistiram da Fila do Bar -----------------| \n");
	fprintf(fp, "|----------------------------------------------------------------| \n");
	fprintf(fp, "|----------------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Desistiram Fila Bar: %d \n", Total_Clientes_Desistiram_Fila_Bar);
	fprintf(fp, "Total Clientes Mobilidade Reduzida Desistiram Fila Bar: %d \n", Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar);
	fprintf(fp, "Total Clientes Crianças Desistiram Fila Bar: %d \n", Total_Clientes_Criancas_Desistiram_Fila_Bar);
	fprintf(fp, "Total Clientes Publico Geral Desistiram Fila Bar: %d \n", Total_Clientes_Publico_Geral_Desistiram_Fila_Bar);
	fprintf(fp, "\n");
	fprintf(fp, "|----------------------------------------------------------------------| \n");
	fprintf(fp, "|----------------------------------------------------------------------| \n");
	fprintf(fp, "|--------- Clientes Desistiram da Fila do Montanha Russa --------------| \n");
	fprintf(fp, "|----------------------------------------------------------------------| \n");
	fprintf(fp, "|----------------------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Desistiram Fila Montanha Russa: %d \n",Total_Clientes_Desistiram_Fila_MontanhaRussa );
	fprintf(fp, "Total Clientes Crianças Desistiram Fila Montanha Russa: %d \n",Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussa );
	fprintf(fp, "Total Clientes Publico Geral Desistiram Fila Montanha Russa: %d \n",Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRusssa );
	fprintf(fp, "\n");
	fprintf(fp, "|--------------------------------------------------------------------| \n");
	fprintf(fp, "|--------------------------------------------------------------------| \n");
	fprintf(fp, "|----------- Clientes Desistiram da Fila da Carroussel---------------| \n");
	fprintf(fp, "|--------------------------------------------------------------------| \n");
	fprintf(fp, "|--------------------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Desistiram Fila Carroussel: %d \n",Total_Clientes_Desistiram_Fila_Carroussel );
	fprintf(fp, "Total Clientes Mobilidade Reduzida Desistiram Fila Carroussel: %d \n",Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel );
	fprintf(fp, "Total Clientes Crianças Desistiram Fila Carroussel: %d \n",Total_Clientes_Criancas_Desistiram_Fila_Carroussel );
	fprintf(fp, "Total Clientes Publico Geral Desistiram Fila Carroussel: %d \n",Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel );
	fprintf(fp, "\n");
	fprintf(fp, "|-------------------------------------------------------------------------| \n");
	fprintf(fp, "|-------------------------------------------------------------------------| \n");
	fprintf(fp, "|----------- Clientes Desistiram da Fila das Bilheteiras -----------------| \n");
	fprintf(fp, "|-------------------------------------------------------------------------| \n");
	fprintf(fp, "|-------------------------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Desistiram Fila Bilheteiras: %d \n",Total_Clientes_Desistiram_Filas_Bilheteiras );
	fprintf(fp, "Total Clientes Mobilidade Reduzida Desistiram Fila Bilheteiras: %d \n",Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras );
	fprintf(fp, "Total Clientes Crianças Desistiram Fila Bilheteiras: %d \n",Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras );
	fprintf(fp, "Total Clientes Publico Geral Desistiram Fila Bilheteiras: %d \n",Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras );
	fprintf(fp, "\n");


	fprintf(fp, "|-------------------------------------------------------------------------| \n");
	fprintf(fp, "|-------------------------------------------------------------------------| \n");
	fprintf(fp, "|---------------------------- Tempos Medios ------------------------------| \n");
	fprintf(fp, "|-------------------------------------------------------------------------| \n");
	fprintf(fp, "|-------------------------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Tempo Medio Antes Ser Atendido Bar: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Ser_Atendido_Bar));
	fprintf(fp, "Tempo Medio Antes Andar Montanha Russa: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Andar_MontanhaRussa));
	fprintf(fp, "Tempo Medio Antes Entrar Carroussel: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Entrar_Carroussel));
	fprintf(fp, "Tempo Medio Antes Entrar Feira: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Entrar_Feira));

	fprintf(fp, "Tempo Medio Antes Desistir Fila Bar: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Desistir_Fila_Bar));
	fprintf(fp, "Tempo Medio Antes Desistir Fila Montanha Russa: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Desistir_Fila_MontanhaRussa));
	fprintf(fp, "Tempo Medio Antes Desistir Fila Carroussel: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Desistir_Fila_Carroussel));
	fprintf(fp, "Tempo Medio Antes Desistir Fila Bilheteiras: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Desistir_Filas_Bilheteiras));
	fprintf(fp, "Tempo Medio Estadia Feira: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Estadia_Feira));

	fprintf(fp, "|-------------------------------------------------------------------------| \n");
	fprintf(fp, "|-------------------------------------------------------------------------| \n");
	fprintf(fp, "|--------------------------- Tempos Maximos ------------------------------| \n");
	fprintf(fp, "|-------------------------------------------------------------------------| \n");
	fprintf(fp, "|-------------------------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Tempo Maximo Antes Ser Atendido Bar: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar));
	fprintf(fp, "Tempo Maximo Antes Andar Montanha Russa: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Andar_MontanhaRussa));
	fprintf(fp, "Tempo Maximo Antes Entrar Carroussel: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Entrar_Carroussel));
	fprintf(fp, "Tempo Maximo Antes Entrar Feira: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Entrar_Feira));

	fprintf(fp, "Tempo Maximo Antes Desistir Fila Bar: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar));
	fprintf(fp, "Tempo Maximo Antes Desistir Fila Montanha Russa: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussa));
	fprintf(fp, "Tempo Maximo Antes Desistir Fila Carroussel: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel));
	fprintf(fp, "Tempo Maximo Antes Desistir Fila Bilheteiras: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras));
	fprintf(fp, "\n");
	fprintf(fp, "*Estatisticas Terminadas* \n");  
	fprintf(fp, "\n");

	fclose(fp);
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

//Funcao que Mostra Informacao do Grupo
void Mostra_Informacao_Grupo()
{
	Escreve_Cabecalho(Escolha_Menu);

	printf("│                                         │       Sistemas Operativos      │                                   │\n");
	printf("│                                         └────────────────────────────────┘                                   │\n");
	printf("│                                                                                                              │\n");
	printf("│ -Eurico Teixeira                                                                                             │\n");
	printf("│  Nº 2083014                                                                                                  │\n");
	printf("│                                                                                                              │\n");
	printf("│ -Paulo Gil de Freitas                                                                                        │\n");
	printf("│  Nº 2012413                                                                                                  │\n");
	printf("│                                                                                                              │\n");
	printf("│ -Josue Ferreira                                                                                              │\n");
	printf("│  Nº 2066513                                                                                                  │\n");
	Coloca_Linhas_Em_Branco(13);
}

//Funcao Que Mostra Instrucoes de Funcionamento da Aplicacao
void Mostra_Funcionamento()
{
	Escreve_Cabecalho(Escolha_Menu);

	printf("│ -Temos 7 Hipóteses que Podem Ser Escolhidas.                                                                 │\n");
	printf("│ -Para Podermos Ter Acesso aos Menus de Tempo Real É Necessario uma Simulação Activa.                         │\n");
	printf("│ -É Criado Automaticamente o Log de Registos e as Estatisticas Finais Após a Realização de Uma Simulação      │\n");

	Coloca_Linhas_Em_Branco(21);

}


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
	int Hora_Saida_Feira=Buffer[17];

	switch(Estado) //Evento
	{
		case 1: //Entra Fila Bilheteiras da Feira
		Total_Clientes_Acutalmente_Nas_Filas_Bilheteiras++;

		if(Tipo_Cliente==1) //Mobilidade Reduzida
		{
			Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras++;
		}
		else if(Tipo_Cliente==2) //Criancas
		{
			Total_Clientes_Criancas_Entraram_Filas_Bilheteiras++;
		}
		else if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras++;
		}
		break;
	//---------------------------------------------------------------------------------------
		case 2: //Sai Fila Bilheteiras da Feira e Entra na Feira
		Total_Clientes_Acutalmente_Nas_Filas_Bilheteiras--;
		Total_Clientes_Acutalmente_Na_Feira++;

		if(Tipo_Cliente==1) //Mobilidade Reduzida
		{
			Total_Clientes_Mobilidade_Reduzida_Entraram_Feira++;
		}
		else if(Tipo_Cliente==2) //Criancas
		{
			Total_Clientes_Criancas_Entraram_Feira++;
		}
		else if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Entraram_Feira++;
		}

		//Tratamento dos Tempos Maximos:
		Tempo_Espera_Filas_Bilheteiras_Antes_Entrar_Feira=Hora_Entrada_Feira-Hora_Entrada_Filas_Bilheteiras;
		if(Tempo_Maximo_Espera_Antes_Entrar_Feira==0)
		{
			Tempo_Maximo_Espera_Antes_Entrar_Feira=Tempo_Espera_Filas_Bilheteiras_Antes_Entrar_Feira;
		}
		else if(Tempo_Espera_Filas_Bilheteiras_Antes_Entrar_Feira>Tempo_Maximo_Espera_Antes_Entrar_Feira)
		{
			Tempo_Maximo_Espera_Antes_Entrar_Feira=Tempo_Espera_Filas_Bilheteiras_Antes_Entrar_Feira;
		}

		//Variaveis Para Tempos Médios:
		Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira=Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira+Tempo_Espera_Filas_Bilheteiras_Antes_Entrar_Feira;

		break;
	//---------------------------------------------------------------------------------------
		case 3: //Desiste Fila Bilheteiras da Feira
		Total_Clientes_Acutalmente_Nas_Filas_Bilheteiras--;

		if(Tipo_Cliente==1) //Mobilidade Reduzida
		{
			Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras++;
		}
		else if(Tipo_Cliente==2) //Criancas
		{
			Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras++;
		}
		else if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras++;
		}

		//Tratamento dos Tempos Maximos:
		Tempo_Espera_Filas_Bilheteiras_Antes_Desistir=Hora_Desistencia_Filas_Bilheteiras-Hora_Entrada_Filas_Bilheteiras;
		if(Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras==0)
		{
			Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras=Tempo_Espera_Filas_Bilheteiras_Antes_Desistir;
		}
		else if(Tempo_Espera_Filas_Bilheteiras_Antes_Desistir>Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras)
		{
			Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras=Tempo_Espera_Filas_Bilheteiras_Antes_Desistir;
		}

		//Variaveis Para Tempos Médios:
		Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras=Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras+Tempo_Espera_Filas_Bilheteiras_Antes_Desistir;

		break;
	//---------------------------------------------------------------------------------------
		case 4: //Entra Fila Bar
		Total_Clientes_Acutalmente_Na_Fila_Bar++;

		if(Tipo_Cliente==1) //Mobilidade Reduzida
		{
			Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar++;
		}
		else if(Tipo_Cliente==2) //Criancas
		{
			Total_Clientes_Criancas_Entraram_Fila_Bar++;
		}
		else if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Entraram_Fila_Bar++;
		}
		break;
	//---------------------------------------------------------------------------------------
		case 5: //Sai Fila Bar, é Atendido e Sai do Bar
		Total_Clientes_Acutalmente_Na_Fila_Bar--;

		if(Tipo_Cliente==1) //Mobilidade Reduzida
		{
			Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar++;
		}
		else if(Tipo_Cliente==2) //Criancas
		{
			Total_Clientes_Criancas_Atendidos_Bar++;
		}
		else if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Atendidos_Bar++;
		}

		//Tratamento dos Tempos Maximos:
		Tempo_Espera_Fila_Bar_Antes_Ser_Atendido_Bar=Hora_Atendimento_Bar-Hora_Entrada_Fila_Bar;
		if(Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar==0)
		{
			Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar=Tempo_Espera_Fila_Bar_Antes_Ser_Atendido_Bar;
		}
		else if(Tempo_Espera_Fila_Bar_Antes_Ser_Atendido_Bar>Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar)
		{
			Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar=Tempo_Espera_Fila_Bar_Antes_Ser_Atendido_Bar;
		}

		//Variaveis Para Tempos Médios:
		Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido=Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido+Tempo_Espera_Fila_Bar_Antes_Ser_Atendido_Bar;

		break;

	//---------------------------------------------------------------------------------------
		case 6: //Fica no Bar//Sai Fila Bar, é Atendido e Fica no Bar
		Total_Clientes_Acutalmente_Na_Fila_Bar--;
		Total_Clientes_Acutalmente_No_Bar++;

		if(Tipo_Cliente==1) //Mobilidade Reduzida
		{
			Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar++;
		}
		else if(Tipo_Cliente==2) //Criancas
		{
			Total_Clientes_Criancas_Atendidos_Bar++;
		}
		else if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Atendidos_Bar++;
		}

		//Tratamento dos Tempos Maximos:
		Tempo_Espera_Fila_Bar_Antes_Ser_Atendido_Bar=Hora_Atendimento_Bar-Hora_Entrada_Fila_Bar;
		if(Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar==0)
		{
			Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar=Tempo_Espera_Fila_Bar_Antes_Ser_Atendido_Bar;
		}
		else if(Tempo_Espera_Fila_Bar_Antes_Ser_Atendido_Bar>Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar)
		{
			Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar=Tempo_Espera_Fila_Bar_Antes_Ser_Atendido_Bar;
		}

		//Variaveis Para Tempos Médios:
		Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido=Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido+Tempo_Espera_Fila_Bar_Antes_Ser_Atendido_Bar;

		break;
	//---------------------------------------------------------------------------------------
		case 7: //Desiste Fila Bar
		Total_Clientes_Acutalmente_Na_Fila_Bar--;

		if(Tipo_Cliente==1) //Mobilidade Reduzida
		{
			Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar++;
		}
		else if(Tipo_Cliente==2) //Criancas
		{
			Total_Clientes_Criancas_Desistiram_Fila_Bar++;
		}
		else if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Desistiram_Fila_Bar++;
		}

		//Tratamento dos Tempos Maximos:
		Tempo_Espera_Fila_Bar_Antes_Desistir=Hora_Desistencia_Fila_Bar-Hora_Entrada_Fila_Bar;
		if(Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar==0)
		{
			Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar=Tempo_Espera_Fila_Bar_Antes_Desistir;
		}
		else if(Tempo_Espera_Fila_Bar_Antes_Desistir>Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar)
		{
			Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar=Tempo_Espera_Fila_Bar_Antes_Desistir;
		}

		//Variaveis Para Tempos Médios:
		Soma_Tempos_Espera_Antes_Desistir_Fila_Bar=Soma_Tempos_Espera_Antes_Desistir_Fila_Bar+Tempo_Espera_Fila_Bar_Antes_Desistir;

		break;
	//---------------------------------------------------------------------------------------
		case 8: //Sai Sala Bar
		Total_Clientes_Acutalmente_No_Bar--;

		break;
	//---------------------------------------------------------------------------------------
		case 9: //Entra Fila Carroussel
		Total_Clientes_Acutalmente_Na_Fila_Carroussel++;

		if(Tipo_Cliente==1) //Mobilidade Reduzida
		{
			Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel++;
		}
		else if(Tipo_Cliente==2) //Criancas
		{
			Total_Clientes_Criancas_Entraram_Fila_Carroussel++;
		}
		else if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel++;
		}
		break;
	//---------------------------------------------------------------------------------------
		case 10: //Sai Fila Carroussel e Entra no Carroussel
		Total_Clientes_Acutalmente_Na_Fila_Carroussel--;
		Total_Clientes_Acutalmente_No_Carroussel++;

		if(Tipo_Cliente==1) //Mobilidade Reduzida
		{
			Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel++;
		}
		else if(Tipo_Cliente==2) //Criancas
		{
			Total_Clientes_Criancas_Entraram_Carroussel++;
		}
		else if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Entraram_Carroussel++;
		}

		//Tratamento dos Tempos Maximos:
		Tempo_Espera_Fila_Carroussel_Antes_Entrar_Carroussel=Hora_Entrada_Carroussel-Hora_Entrada_Fila_Carroussel;
		if(Tempo_Maximo_Espera_Antes_Entrar_Carroussel==0)
		{
			Tempo_Maximo_Espera_Antes_Entrar_Carroussel=Tempo_Espera_Fila_Carroussel_Antes_Entrar_Carroussel;
		}
		else if(Tempo_Espera_Fila_Carroussel_Antes_Entrar_Carroussel>Tempo_Maximo_Espera_Antes_Entrar_Carroussel)
		{
			Tempo_Maximo_Espera_Antes_Entrar_Carroussel=Tempo_Espera_Fila_Carroussel_Antes_Entrar_Carroussel;
		}

		//Variaveis Para Tempos Médios:
		Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel=Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel+Tempo_Espera_Fila_Carroussel_Antes_Entrar_Carroussel;

		break;
	//---------------------------------------------------------------------------------------
		case 11: //Desiste Fila Carroussel
		Total_Clientes_Acutalmente_Na_Fila_Carroussel--;

		if(Tipo_Cliente==1) //Mobilidade Reduzida
		{
			Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel++;
		}
		else if(Tipo_Cliente==2) //Criancas
		{
			Total_Clientes_Criancas_Desistiram_Fila_Carroussel++;
		}
		else if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel++;
		}

		//Tratamento dos Tempos Maximos:
		Tempo_Espera_Fila_Carroussel_Antes_Desistir=Hora_Desistencia_Fila_Carroussel-Hora_Entrada_Fila_Carroussel;
		if(Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel==0)
		{
			Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel=Tempo_Espera_Fila_Carroussel_Antes_Desistir;
		}
		else if(Tempo_Espera_Fila_Carroussel_Antes_Desistir>Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel)
		{
			Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel=Tempo_Espera_Fila_Carroussel_Antes_Desistir;
		}

		//Variaveis Auxiliares Para Tempos Médios:
		Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel=Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel+Tempo_Espera_Fila_Carroussel_Antes_Desistir;

		break;
	//---------------------------------------------------------------------------------------
		case 12: //Entra Fila Montanha Russa
		Total_Clientes_Acutalmente_Na_Fila_MontanhaRussa++;

		if(Tipo_Cliente==2) //Criancas
		{
			Total_Clientes_Criancas_Entraram_Fila_MontanhaRussa++;
		}
		else if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussa++;
		}
		break;
	//---------------------------------------------------------------------------------------
		case 13: //Sai Fila Montanha Russa e Anda na Montanha Russa
		Total_Clientes_Acutalmente_Na_Fila_MontanhaRussa--;

		if(Tipo_Cliente==2) //Criancas
		{
			Total_Clientes_Criancas_Andaram_MontanhaRussa++;
		}
		else if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Andaram_MontanhaRussa++;
		}

		//Tratamento dos Tempos Maximos:
		Tempo_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa=Hora_Andou_MontanhaRussa-Hora_Entrada_Fila_MontanhaRussa;
		if(Tempo_Maximo_Espera_Antes_Andar_MontanhaRussa==0)
		{
			Tempo_Maximo_Espera_Antes_Andar_MontanhaRussa=Tempo_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa;
		}
		else if(Tempo_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa>Tempo_Maximo_Espera_Antes_Andar_MontanhaRussa)
		{
			Tempo_Maximo_Espera_Antes_Andar_MontanhaRussa=Tempo_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa;
		}

		//Variaveis Para Tempos Médios:
		Soma_Tempos_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa=Soma_Tempos_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa+Tempo_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa;

		break;
	//---------------------------------------------------------------------------------------
		case 14: //Desiste Fila Montanha Russa
		Total_Clientes_Acutalmente_Na_Fila_MontanhaRussa--;

		if(Tipo_Cliente==2) //Criancas
		{
			Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussa++;
		}
		else if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRusssa++;
		}

		//Tratamento dos Tempos Maximos:
		Tempo_Espera_Fila_MontanhaRussa_Antes_Desistir=Hora_Desistencia_Fila_MontanhaRussa-Hora_Entrada_Fila_MontanhaRussa;
		if(Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussa==0)
		{
			Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussa=Tempo_Espera_Fila_MontanhaRussa_Antes_Desistir;
		}
		else if(Tempo_Espera_Fila_MontanhaRussa_Antes_Desistir>Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussa)
		{
			Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussa=Tempo_Espera_Fila_MontanhaRussa_Antes_Desistir;
		}

		//Variaveis Para Tempos Médios:
		Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussa=Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussa+Tempo_Espera_Fila_MontanhaRussa_Antes_Desistir;

		break;
	//---------------------------------------------------------------------------------------
		case 15: //Sai Feira
		Total_Clientes_Acutalmente_Na_Feira--;
		
		Total_Clientes_Sairam_Feira++;

		//Variaveis Para Tempos Medios:
		Soma_Tempos_Estadia_Feira=Soma_Tempos_Estadia_Feira+(Hora_Saida_Feira - Hora_Entrada_Feira); //Tempo_Estadia_Feira;


		break;
	//---------------------------------------------------------------------------------------
		case 16: //Sai Carroussel
		Total_Clientes_Acutalmente_No_Carroussel--;

		break;
	//---------------------------------------------------------------------------------------
		case 50: //Fim Simulação

		Simulacao_Em_Curso=false;

//Escreve Estatisticas Finais no Ficheiro Quando a Simulacao Acaba
Escreve_Estatisticas(Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar, Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussa, Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel, Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras, Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar, Tempo_Maximo_Espera_Antes_Andar_MontanhaRussa, Tempo_Maximo_Espera_Antes_Entrar_Carroussel, Tempo_Maximo_Espera_Antes_Entrar_Feira, Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido, Soma_Tempos_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa, Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel, Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira, Soma_Tempos_Espera_Antes_Desistir_Fila_Bar, Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussa, Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel, Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras, Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar, Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel, Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras, Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar, Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel, Total_Clientes_Mobilidade_Reduzida_Entraram_Feira, Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar, Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel, Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras, Total_Clientes_Criancas_Entraram_Fila_Bar, Total_Clientes_Criancas_Entraram_Fila_MontanhaRussa, Total_Clientes_Criancas_Entraram_Fila_Carroussel, Total_Clientes_Criancas_Entraram_Filas_Bilheteiras, Total_Clientes_Criancas_Atendidos_Bar, Total_Clientes_Criancas_Andaram_MontanhaRussa, Total_Clientes_Criancas_Entraram_Carroussel, Total_Clientes_Criancas_Entraram_Feira, Total_Clientes_Criancas_Desistiram_Fila_Bar, Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussa, Total_Clientes_Criancas_Desistiram_Fila_Carroussel, Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras, Total_Clientes_Publico_Geral_Entraram_Fila_Bar, Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussa, Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel, Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras, Total_Clientes_Publico_Geral_Atendidos_Bar, Total_Clientes_Publico_Geral_Andaram_MontanhaRussa, Total_Clientes_Publico_Geral_Entraram_Carroussel, Total_Clientes_Publico_Geral_Entraram_Feira, Total_Clientes_Publico_Geral_Desistiram_Fila_Bar, Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRusssa, Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel, Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras, Soma_Tempos_Estadia_Feira, Total_Clientes_Sairam_Feira);		

		break;
	//---------------------------------------------------------------------------------------
		default: printf("Erro: Estado do Processo Errado. \n");
		break;


	}//Fim Switch


	//Adiciona ao Ficheiro Eventos o Novo Evento que Aconteceu
	Escreve_Ficheiro_Eventos(Estado, ID_Cliente, Hora_Actual,Tipo_Cliente, Idade); //Funcao no Envia_Para_Ficheiros.c

//Estes If's Sao Para Actualizar o Ecra Dependendo da Hipotese Que o Utilizador Escolheu no Menu Principal

	if(Escolha_Menu==2)
	{
		
		Mostra_Estatisticas_Monitor(Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar, Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussa, Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel, Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras, Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar, Tempo_Maximo_Espera_Antes_Andar_MontanhaRussa, Tempo_Maximo_Espera_Antes_Entrar_Carroussel, Tempo_Maximo_Espera_Antes_Entrar_Feira, Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido, Soma_Tempos_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa, Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel, Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira, Soma_Tempos_Espera_Antes_Desistir_Fila_Bar, Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussa, Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel, Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras, Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar, Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel, Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras, Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar, Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel, Total_Clientes_Mobilidade_Reduzida_Entraram_Feira, Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar, Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel, Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras, Total_Clientes_Criancas_Entraram_Fila_Bar, Total_Clientes_Criancas_Entraram_Fila_MontanhaRussa, Total_Clientes_Criancas_Entraram_Fila_Carroussel, Total_Clientes_Criancas_Entraram_Filas_Bilheteiras, Total_Clientes_Criancas_Atendidos_Bar, Total_Clientes_Criancas_Andaram_MontanhaRussa, Total_Clientes_Criancas_Entraram_Carroussel, Total_Clientes_Criancas_Entraram_Feira, Total_Clientes_Criancas_Desistiram_Fila_Bar, Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussa, Total_Clientes_Criancas_Desistiram_Fila_Carroussel, Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras, Total_Clientes_Publico_Geral_Entraram_Fila_Bar, Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussa, Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel, Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras, Total_Clientes_Publico_Geral_Atendidos_Bar, Total_Clientes_Publico_Geral_Andaram_MontanhaRussa, Total_Clientes_Publico_Geral_Entraram_Carroussel, Total_Clientes_Publico_Geral_Entraram_Feira, Total_Clientes_Publico_Geral_Desistiram_Fila_Bar, Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRusssa, Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel, Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras, Soma_Tempos_Estadia_Feira, Total_Clientes_Sairam_Feira);

	}
	else if(Escolha_Menu==3) //Isto é, Se o Menu Actual For o 3 (Mostrar Eventos em Tempo Real), Sempre Que Houver um Evento, Mostra no Ecra
	{
		Mostra_No_Monitor_Eventos_Tempo_Real(Estado, ID_Cliente, Hora_Actual, Tipo_Cliente, Idade);
	}
	else if(Escolha_Menu==4) //Isto é, Se o Menu Actual For o 4 (Mostrar Zonas em Tempo Real), Sempre Que Houver uma Alteracao nas Zonas, Vai Ser Mostrada no Ecra
	{
		Mostra_Zonas_Tempo_Real(Tamanho_Maximo_Bar, Tamanho_Maximo_Carroussel, Tamanho_Maximo_Feira, Total_Clientes_Acutalmente_No_Bar, Total_Clientes_Acutalmente_No_Carroussel, Total_Clientes_Acutalmente_Na_Feira, Total_Clientes_Acutalmente_Na_Fila_Bar, Total_Clientes_Acutalmente_Na_Fila_MontanhaRussa, Total_Clientes_Acutalmente_Na_Fila_Carroussel, Total_Clientes_Acutalmente_Nas_Filas_Bilheteiras);
	}

}



//Funcao Que Poe o Valor das Variaveis Globais de Estatisticas no Seu Estado Original (=0) Para Permitir Varias Simulacoes Sem Ter de Correr a Aplicacao de Novo.
void Funcao_Limpa_Variaveis_Estatisticas()
{
Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido=0;
Soma_Tempos_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa=0;
Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel=0;
Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira=0;

Soma_Tempos_Espera_Antes_Desistir_Fila_Bar=0;
Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussa=0;
Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel=0;
Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras=0;

Tempo_Espera_Filas_Bilheteiras_Antes_Entrar_Feira=0;
Tempo_Espera_Filas_Bilheteiras_Antes_Desistir=0;
Tempo_Espera_Fila_Bar_Antes_Ser_Atendido_Bar=0;
Tempo_Espera_Fila_Bar_Antes_Desistir=0;
Tempo_Espera_Fila_Carroussel_Antes_Entrar_Carroussel=0;
Tempo_Espera_Fila_Carroussel_Antes_Desistir=0;
Tempo_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa=0;
Tempo_Espera_Fila_MontanhaRussa_Antes_Desistir=0;

Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar=0;
Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussa=0;
Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel=0;
Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras=0;

Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar=0;
Tempo_Maximo_Espera_Antes_Andar_MontanhaRussa=0;
Tempo_Maximo_Espera_Antes_Entrar_Carroussel=0;
Tempo_Maximo_Espera_Antes_Entrar_Feira=0;

Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar=0;
Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel=0;
Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras=0;

Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar=0; 
Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel=0;
Total_Clientes_Mobilidade_Reduzida_Entraram_Feira=0;

Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar=0;
Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel=0;
Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras=0;

Total_Clientes_Criancas_Entraram_Fila_Bar=0;
Total_Clientes_Criancas_Entraram_Fila_MontanhaRussa=0;
Total_Clientes_Criancas_Entraram_Fila_Carroussel=0;
Total_Clientes_Criancas_Entraram_Filas_Bilheteiras=0;

Total_Clientes_Criancas_Atendidos_Bar=0; 
Total_Clientes_Criancas_Andaram_MontanhaRussa=0;
Total_Clientes_Criancas_Entraram_Carroussel=0;
Total_Clientes_Criancas_Entraram_Feira=0;

Total_Clientes_Criancas_Desistiram_Fila_Bar=0;
Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussa=0;
Total_Clientes_Criancas_Desistiram_Fila_Carroussel=0;
Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras=0;

Total_Clientes_Publico_Geral_Entraram_Fila_Bar=0;
Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussa=0;
Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel=0;
Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras=0;

Total_Clientes_Publico_Geral_Atendidos_Bar=0; 
Total_Clientes_Publico_Geral_Andaram_MontanhaRussa=0;
Total_Clientes_Publico_Geral_Entraram_Carroussel=0;
Total_Clientes_Publico_Geral_Entraram_Feira=0;

Total_Clientes_Publico_Geral_Desistiram_Fila_Bar=0;
Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRusssa=0;
Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel=0;
Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras=0;

Total_Clientes_Acutalmente_Na_Fila_Bar=0;
Total_Clientes_Acutalmente_Na_Fila_MontanhaRussa=0;
Total_Clientes_Acutalmente_Na_Fila_Carroussel=0;
Total_Clientes_Acutalmente_Nas_Filas_Bilheteiras=0;

Total_Clientes_Acutalmente_No_Bar=0;
Total_Clientes_Acutalmente_No_Carroussel=0;
Total_Clientes_Acutalmente_Na_Feira=0;

Soma_Tempos_Estadia_Feira=0;
Total_Clientes_Sairam_Feira=0;
}

	
//Teste Funcao da Thread Que Recebe Mensagens.
void Activa_Ciclo_Recebe_Mensagens()
{
	while(Simulacao_Em_Curso!=false)//==true)
	{
		Interpreta_Mensagem();
	}
}
	

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
			Funcao_Limpa_Variaveis_Estatisticas();
			Prepara_Ficheiro_Eventos(); //Funcao Que Prepara o Ficheiro do Relatorio de Eventos (Limpa/Cria Ficheiro "Relatorio_Eventos.c" e Poe Titulo)

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

		if(Simulacao_Em_Curso==true)
		{
			//Funcao Para Mostrar as Estatisticas da Feira no Monitor:
			Mostra_Estatisticas_Monitor(Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar, Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussa, Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel, Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras, Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar, Tempo_Maximo_Espera_Antes_Andar_MontanhaRussa, Tempo_Maximo_Espera_Antes_Entrar_Carroussel, Tempo_Maximo_Espera_Antes_Entrar_Feira, Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido, Soma_Tempos_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa, Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel, Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira, Soma_Tempos_Espera_Antes_Desistir_Fila_Bar, Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussa, Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel, Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras, Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar, Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel, Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras, Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar, Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel, Total_Clientes_Mobilidade_Reduzida_Entraram_Feira, Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar, Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel, Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras, Total_Clientes_Criancas_Entraram_Fila_Bar, Total_Clientes_Criancas_Entraram_Fila_MontanhaRussa, Total_Clientes_Criancas_Entraram_Fila_Carroussel, Total_Clientes_Criancas_Entraram_Filas_Bilheteiras, Total_Clientes_Criancas_Atendidos_Bar, Total_Clientes_Criancas_Andaram_MontanhaRussa, Total_Clientes_Criancas_Entraram_Carroussel, Total_Clientes_Criancas_Entraram_Feira, Total_Clientes_Criancas_Desistiram_Fila_Bar, Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussa, Total_Clientes_Criancas_Desistiram_Fila_Carroussel, Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras, Total_Clientes_Publico_Geral_Entraram_Fila_Bar, Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussa, Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel, Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras, Total_Clientes_Publico_Geral_Atendidos_Bar, Total_Clientes_Publico_Geral_Andaram_MontanhaRussa, Total_Clientes_Publico_Geral_Entraram_Carroussel, Total_Clientes_Publico_Geral_Entraram_Feira, Total_Clientes_Publico_Geral_Desistiram_Fila_Bar, Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRusssa, Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel, Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras, Soma_Tempos_Estadia_Feira, Total_Clientes_Sairam_Feira);

		}
		else
		{
			printf("Erro: Nenhuma Simulacao Activa Neste Momento \n");
		}
		break;
//---------------------------------------------------------------------------------------
		case 3: //Eventos em Tempo Real

		if(Simulacao_Em_Curso==true)
		{
			Escreve_Cabecalho(Escolha_Menu);
		}
		else
		{
			printf("Erro: Nenhuma Simulacao Activa Neste Momento \n");
		}
		break;
//---------------------------------------------------------------------------------------
		case 4: //Estado Zonas em Tempo Real

		if(Simulacao_Em_Curso==true)
		{
			Mostra_Zonas_Tempo_Real(Tamanho_Maximo_Bar, Tamanho_Maximo_Carroussel, Tamanho_Maximo_Feira, Total_Clientes_Acutalmente_No_Bar, Total_Clientes_Acutalmente_No_Carroussel, Total_Clientes_Acutalmente_Na_Feira, Total_Clientes_Acutalmente_Na_Fila_Bar, Total_Clientes_Acutalmente_Na_Fila_MontanhaRussa, Total_Clientes_Acutalmente_Na_Fila_Carroussel, Total_Clientes_Acutalmente_Nas_Filas_Bilheteiras);
		}
		else
		{
			printf("Erro: Nenhuma Simulacao Activa Neste Momento \n");
		}
		break;
//---------------------------------------------------------------------------------------
		case 5: //Sobre

		Mostra_Informacao_Grupo();
		break;
//---------------------------------------------------------------------------------------
		case 6: //Ajuda

		Mostra_Funcionamento();
		break;		
//---------------------------------------------------------------------------------------
		case 7: //Fechar
		if(Simulacao_Em_Curso==true)
		{
			//Escreve Estatisticas (do Momento em Que a Simulacao Foi Interrompida) no Ficheiro
Escreve_Estatisticas(Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar, Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussa, Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel, Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras, Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar, Tempo_Maximo_Espera_Antes_Andar_MontanhaRussa, Tempo_Maximo_Espera_Antes_Entrar_Carroussel, Tempo_Maximo_Espera_Antes_Entrar_Feira, Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido, Soma_Tempos_Espera_Fila_MontanhaRussa_Antes_Andar_MontanhaRussa, Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel, Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira, Soma_Tempos_Espera_Antes_Desistir_Fila_Bar, Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussa, Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel, Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras, Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar, Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel, Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras, Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar, Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel, Total_Clientes_Mobilidade_Reduzida_Entraram_Feira, Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar, Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel, Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras, Total_Clientes_Criancas_Entraram_Fila_Bar, Total_Clientes_Criancas_Entraram_Fila_MontanhaRussa, Total_Clientes_Criancas_Entraram_Fila_Carroussel, Total_Clientes_Criancas_Entraram_Filas_Bilheteiras, Total_Clientes_Criancas_Atendidos_Bar, Total_Clientes_Criancas_Andaram_MontanhaRussa, Total_Clientes_Criancas_Entraram_Carroussel, Total_Clientes_Criancas_Entraram_Feira, Total_Clientes_Criancas_Desistiram_Fila_Bar, Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussa, Total_Clientes_Criancas_Desistiram_Fila_Carroussel, Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras, Total_Clientes_Publico_Geral_Entraram_Fila_Bar, Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussa, Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel, Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras, Total_Clientes_Publico_Geral_Atendidos_Bar, Total_Clientes_Publico_Geral_Andaram_MontanhaRussa, Total_Clientes_Publico_Geral_Entraram_Carroussel, Total_Clientes_Publico_Geral_Entraram_Feira, Total_Clientes_Publico_Geral_Desistiram_Fila_Bar, Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRusssa, Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel, Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras, Soma_Tempos_Estadia_Feira, Total_Clientes_Sairam_Feira);	
		}

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
	
	

int main(int argc, char *argv[])
{
	Ler_Ficheiro_Config2();

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








	