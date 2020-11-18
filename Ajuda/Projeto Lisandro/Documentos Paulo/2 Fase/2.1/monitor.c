#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "unix.h"
//#include "funcoes_auxiliares_partilhadas.c"

int sockfd;
int newsockfd;

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------

//Estatisticas:

//Total Clientes)

//                                  |----------------------------------------------------------------------------|
//                                  | Soma Tempos de Espera dos Clientes Antes de Entrarem e Antes de Desistirem |
//                                  |----------------------------------------------------------------------------|


//Soma Tempos de Espera dos Clientes Em Cada Fila de Cada Zona Antes de ser Atendido/Entrar/Andar nas Zonas
int Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido=0;
int Soma_Tempos_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG=0;
int Soma_Tempos_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP=0;
int Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel=0;
int Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira=0;

//Soma Tempos de Espera dos Clientes Antes de Desistirem da Fila de Cada Zona
int Soma_Tempos_Espera_Antes_Desistir_Fila_Bar=0;
int Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaG=0;
int Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaP=0;
int Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel=0;
int Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras=0;


//                                  |------------------------------------------------------|
//                                  | Mais Variaveis Auxiliares Para Tratamento dos Tempos |
//                                  |------------------------------------------------------|


int Tempo_Espera_Filas_Bilheteiras_Antes_Entrar_Feira=0;
int Tempo_Espera_Filas_Bilheteiras_Antes_Desistir=0;

int Tempo_Espera_Fila_Bar_Antes_Ser_Atendido_Bar=0;
int Tempo_Espera_Fila_Bar_Antes_Desistir=0;

int Tempo_Espera_Fila_Carroussel_Antes_Entrar_Carrousssel=0;
int Tempo_Espera_Fila_Carroussel_Antes_Desistir=0;

int Tempo_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG=0;
int Tempo_Espera_Fila_MontanhaRussaG_Antes_Desistir=0;

int Tempo_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP=0;
int Tempo_Espera_Fila_MontanhaRussaP_Antes_Desistir=0;



//                                                          |----------------|
//                                                          | Tempos Maximos |
//                                                          |----------------|


//Tempo Maximo de Espera dos Clientes Antes de Desistirem da Fila de Cada Zona
int Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar=0;
int Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaG=0;
int Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaP=0;
int Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel=0;
int Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras=0;

//Tempo Maximo de Espera dos Clientes Antes de Entrarem em Cada Zona
int Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar=0;
int Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaG=0;
int Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaP=0;
int Tempo_Maximo_Espera_Antes_Entrar_Carroussel=0;
int Tempo_Maximo_Espera_Antes_Entrar_Feira=0;





//                                             |----------------------------------|
//                                             | Mais Variaveis Para Estatisticas |
//                                             |----------------------------------|


//Clientes Mobilidade Reduzida)

//Total de Clientes Mobilidade Reduzida que Entraram nas Filas
int Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar=0;
int Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel=0;
int Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras=0;

//Total de Clientes Mobilidade Reduzida que Foram Atendidos/Entraram/Andaram nas Zonas
int Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar=0; 
int Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel=0;
int Total_Clientes_Mobilidade_Reduzida_Entraram_Feira=0;

//Total de Clientes Mobilidade Reduzida que Desistiram
int Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar=0;
int Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel=0;
int Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras=0;


//Clientes Crianças)

//Total de Clientes Crianças que Entraram nas Filas
int Total_Clientes_Criancas_Entraram_Fila_Bar=0;
int Total_Clientes_Criancas_Entraram_Fila_MontanhaRussaP=0;
int Total_Clientes_Criancas_Entraram_Fila_Carroussel=0;
int Total_Clientes_Criancas_Entraram_Filas_Bilheteiras=0;

//Total de Clientes Crianças que Foram Atendidos/Entraram/Andaram nas Zonas
int Total_Clientes_Criancas_Atendidos_Bar=0; 
int Total_Clientes_Criancas_Andaram_MontanhaRussaP=0;
int Total_Clientes_Criancas_Entraram_Carroussel=0;
int Total_Clientes_Criancas_Entraram_Feira=0;

//Total de Clientes Crianças que Desistiram
int Total_Clientes_Criancas_Desistiram_Fila_Bar=0;
int Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussaP=0;
int Total_Clientes_Criancas_Desistiram_Fila_Carroussel=0;
int Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras=0;


//Clientes Publico Geral)

//Total de Clientes Publico Geral que Entraram nas Filas
int Total_Clientes_Publico_Geral_Entraram_Fila_Bar=0;
int Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaG=0;
int Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaP=0;
int Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel=0;
int Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras=0;

//Total de Clientes Publico Geral que Foram Atendidos/Entraram/Andaram nas Zonas
int Total_Clientes_Publico_Geral_Atendidos_Bar=0; 
int Total_Clientes_Publico_Geral_Andaram_MontanhaRussaG=0;
int Total_Clientes_Publico_Geral_Andaram_MontanhaRussaP=0;
int Total_Clientes_Publico_Geral_Entraram_Carroussel=0;
int Total_Clientes_Publico_Geral_Entraram_Feira=0;

//Total de Clientes Publico Geral que Desistiram
int Total_Clientes_Publico_Geral_Desistiram_Fila_Bar=0;
int Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaG=0;
int Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaP=0;
int Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel=0;
int Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras=0;



//                                          |-------------------------------------------|
//                                          | Variaveis Para Estatisticas em Tempo Real |
//                                          |-------------------------------------------|


//Total de Clientes ACTUALMENTE nas Filas
int Total_Clientes_Acutalmente_Na_Fila_Bar=0;
int Total_Clientes_Acutalmente_Na_Fila_MontanhaRussaG=0;
int Total_Clientes_Acutalmente_Na_Fila_MontanhaRussaP=0;
int Total_Clientes_Acutalmente_Na_Fila_Carroussel=0;
int Total_Clientes_Acutalmente_Nas_Filas_Bilheteiras=0;

//Total de Clientes ACTUALMENTE nas Zonas
int Total_Clientes_Acutalmente_No_Bar=0;
int Total_Clientes_Acutalmente_No_Carroussel=0;
int Total_Clientes_Acutalmente_Na_Feira=0;
int Total_Clientes_Actualmente_Na_MontanhaRussaG=0;
int Total_Clientes_Actualmente_Na_MontanhaRussaP=0;




//                                              |--------------------------------------|
//                                              | Variaveis Tempo Medio Estadia Feira |
//                                              |--------------------------------------|

int Soma_Tempos_Estadia_Feira=0;
int Total_Clientes_Sairam_Feira=0;



//                                              |------------------------|
//                                              | Variaveis da Simulacao |
//                                              |------------------------|



//Variaveis da Simulacao:
bool Simulacao_Concluida=false; //Variavel Vai Indicar se a Simulacao Ja Terminou
bool Simulacao_Em_Curso=false; //Variavel Vai Indicar que Esta Uma Simulacao em Curso (Se For True Podemos Activar a Ocupacao das Zonas em Tempo Real)
int Escolha_Menu=0; //Guarda a Escolha do Sub Menu que o Utilizador Pressionou. (Dependendo da Escolha, Vai Acontecer Algo)

//Funcao Que Mostra Instrucoes de Funcionamento da Aplicacao
void Mostra_Funcionamento()
{
	Escreve_Cabecalho(Escolha_Menu);

	printf("│ -Temos 7 Hipóteses que Podem Ser Escolhidas.                                                                 │\n");
	printf("│ -Para Podermos Ter Acesso aos Menus de Tempo Real É Necessario uma Simulação Activa.                         │\n");
	printf("│ -É Criado Automaticamente o Log de Registos e as Estatisticas Finais Após a Realização de Uma Simulação      │\n");

	Coloca_Linhas_Em_Branco(21);

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

//Auxiliar da Sub Funcao. Trata da Parte Estetica. Basicamente a Operacao "Trata_Ocupacao_Zonas_Tempo_Real" Vai Descodificar o Tipo de Zona Recebeido em Int e Vai Traduzir Para String. Esta Funcao "Preenche_Ocupacao" Vai Receber os Dados Numericos e a String com o Nome da Zona e Vai Mostrar a Ocupacao Dessa Zona
void Preenche_Ocupacao(int Tamanho_Maximo_Zona, int Total_Clientes_Actual, char* Zona)
{
	//É Preciso Ver se os Espacos Estao Certos ou se é Preciso por Tudo Alinhado Com a Ultima Hipotese
	int Ocupacao_Actual=Total_Clientes_Actual*100/Tamanho_Maximo_Zona;

	if(Ocupacao_Actual==0)
	{
		printf("%s | | | | | | | | | | | | | | | | | | | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>0 && Ocupacao_Actual<=5)
	{
		printf("%s |■| | | | | | | | | | | | | | | | | | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>5 && Ocupacao_Actual<=10)
	{
		printf("%s |■|■| | | | | | | | | | | | | | | | | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>10 && Ocupacao_Actual<=15)
	{
		printf("%s |■|■|■| | | | | | | | | | | | | | | | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>15 && Ocupacao_Actual<=20)
	{
		printf("%s |■|■|■|■| | | | | | | | | | | | | | | | | (%i %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>20 && Ocupacao_Actual<=25)
	{
		printf("%s |■|■|■|■|■| | | | | | | | | | | | | | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>25 && Ocupacao_Actual<=30)
	{
		printf("%s |■|■|■|■|■|■| | | | | | | | | | | | | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>30 && Ocupacao_Actual<=35)
	{
		printf("%s |■|■|■|■|■|■|■| | | | | | | | | | | | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>35 && Ocupacao_Actual<=40)
	{
		printf("%s |■|■|■|■|■|■|■|■| | | | | | | | | | | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>40 && Ocupacao_Actual<=45)
	{
		printf("%s |■|■|■|■|■|■|■|■|■| | | | | | | | | | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>45 && Ocupacao_Actual<=50)
	{
		printf("%s |■|■|■|■|■|■|■|■|■|■| | | | | | | | | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>50 && Ocupacao_Actual<=55)
	{
		printf("%s |■|■|■|■|■|■|■|■|■|■|■| | | | | | | | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>55 && Ocupacao_Actual<=60)
	{
		printf("%s |■|■|■|■|■|■|■|■|■|■|■|■| | | | | | | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>60 && Ocupacao_Actual<=65)
	{
		printf("%s |■|■|■|■|■|■|■|■|■|■|■|■|■| | | | | | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>65 && Ocupacao_Actual<=70)
	{
		printf("%s |■|■|■|■|■|■|■|■|■|■|■|■|■|■| | | | | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>70 && Ocupacao_Actual<=75)
	{
		printf("%s |■|■|■|■|■|■|■|■|■|■|■|■|■|■|■| | | | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>75 && Ocupacao_Actual<=80)
	{
		printf("%s |■|■|■|■|■|■|■|■|■|■|■|■|■|■|■|■| | | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>80 && Ocupacao_Actual<=85)
	{
		printf("%s |■|■|■|■|■|■|■|■|■|■|■|■|■|■|■|■|■| | | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>85 && Ocupacao_Actual<=90)
	{
		printf("%s |■|■|■|■|■|■|■|■|■|■|■|■|■|■|■|■|■|■| | | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>90 && Ocupacao_Actual<=95)
	{
		printf("%s |■|■|■|■|■|■|■|■|■|■|■|■|■|■|■|■|■|■|■| | (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}
	else if(Ocupacao_Actual>95 && Ocupacao_Actual<=100)
	{
		printf("%s |■|■|■|■|■|■|■|■|■|■|■|■|■|■|■|■|■|■|■|■| (%d %%)\n", Zona, Ocupacao_Actual);
		printf("\n");
	}

}

//Subfuncao Para Ajudar a Representar a Ocupacao das Zonas
void Trata_Ocupacao_Zonas_Tempo_Real(int Tamanho_Maximo_Zona_Recebido, int Total_Clientes_Actual_Recebido, int Tipo_Zona_Recebido)
{
int Tipo_Zona=Tipo_Zona_Recebido;

char Zona1[]="Lotacao Bar";
char Zona2[]="Lotacao Carroussel";
char Zona3[]="Lotacao Feira";
char Zona4[]="Lotacao Montanha Russa Grande";
char Zona5[]="Lotacao Montanha Russa Pequena";




switch(Tipo_Zona)
	{	
		case 1: //1=Lotacao Bar
			//char* Zona1[]="Lotacao Bar";
	Preenche_Ocupacao(Tamanho_Maximo_Zona_Recebido, Total_Clientes_Actual_Recebido, Zona1);
			break;
	//---------------------------------------------------------------------------------------
		case 2: //2=Lotacao Carroussel
			//char* Zona2[]="Lotacao Carroussel";
	Preenche_Ocupacao(Tamanho_Maximo_Zona_Recebido, Total_Clientes_Actual_Recebido, Zona2);
			break;
	//---------------------------------------------------------------------------------------
		case 3: //3=Lotacao Feira
			//char* Zona3[]="Lotacao Feira";
	Preenche_Ocupacao(Tamanho_Maximo_Zona_Recebido, Total_Clientes_Actual_Recebido, Zona3);
			break;
	//---------------------------------------------------------------------------------------
		case 4: //3=Lotacao Montanha Russa Grande
			//char* Zona3[]="Lotacao Montanha Russa Grande";
	Preenche_Ocupacao(Tamanho_Maximo_Zona_Recebido, Total_Clientes_Actual_Recebido, Zona4);
			break;
	//---------------------------------------------------------------------------------------
		case 5: //3=Lotacao Montanha Russa Pequena
			//char* Zona3[]="Lotacao Montanha Russa Pequena";
	Preenche_Ocupacao(Tamanho_Maximo_Zona_Recebido, Total_Clientes_Actual_Recebido, Zona5);
			break;
		
		default:
			printf("Erro: Tipo de Zona Incorrecto \n");

	}
		
}

//--------------------------------------------------------------------

//Funcao que Vai Mostrar a Ocupacao das Zonas em Tempo Real
void Mostra_Zonas_Tempo_Real(int Tamanho_Maximo_Bar,int Tamanho_Maximo_Carroussel,int Tamanho_Maximo_Feira,int Tamanho_Maximo_MontanhaRussaG, int Tamanho_Maximo_MontanhaRussaP,int Total_Clientes_Acutalmente_No_Bar,int Total_Clientes_Acutalmente_No_Carroussel,int Total_Clientes_Acutalmente_Na_Feira, int Total_Clientes_Actualmente_Na_MontanhaRussaG, int Total_Clientes_Actualmente_Na_MontanhaRussaP)
/*,int Total_Clientes_Acutalmente_Na_Fila_Bar,int Total_Clientes_Acutalmente_Na_Fila_Escorrega,int Total_Clientes_Acutalmente_Na_Fila_Piscina,int Total_Clientes_Acutalmente_Nas_Filas_Bilheteiras)*/
{
	Trata_Ocupacao_Zonas_Tempo_Real(Tamanho_Maximo_Bar,Total_Clientes_Acutalmente_No_Bar,1);
	Trata_Ocupacao_Zonas_Tempo_Real(Tamanho_Maximo_Carroussel,Total_Clientes_Acutalmente_No_Carroussel,2);
	Trata_Ocupacao_Zonas_Tempo_Real(Tamanho_Maximo_Feira,Total_Clientes_Acutalmente_Na_Feira,3);
	Trata_Ocupacao_Zonas_Tempo_Real(Tamanho_Maximo_MontanhaRussaG,Total_Clientes_Actualmente_Na_MontanhaRussaG,4);
	Trata_Ocupacao_Zonas_Tempo_Real(Tamanho_Maximo_MontanhaRussaP,Total_Clientes_Actualmente_Na_MontanhaRussaP,5);
}

//Funcao Para Mostrar as Estatisticas da Feira no Monitor:
void Mostra_Estatisticas_Monitor(int Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar, int Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaG, int Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaP, int Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel, int Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras, int Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar, int Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaG, int Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaP, int Tempo_Maximo_Espera_Antes_Entrar_Carroussel, int Tempo_Maximo_Espera_Antes_Entrar_Feira, int Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido, int Soma_Tempos_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG, int Soma_Tempos_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP, int Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel, int Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira, int Soma_Tempos_Espera_Antes_Desistir_Fila_Bar, int Soma_Tempos_Espera_Antes_Desistir_MontanhaRussaG, int Soma_Tempos_Espera_Antes_Desistir_MontanhaRussaP, int Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel, int Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras, int Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar, int Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel, int Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras, int Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar, int Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel, int Total_Clientes_Mobilidade_Reduzida_Entraram_Feira, int Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar, int Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel, int Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras, int Total_Clientes_Criancas_Entraram_Fila_Bar, int Total_Clientes_Criancas_Entraram_Fila_MontanhaRussaP, int Total_Clientes_Criancas_Entraram_Fila_Carroussel, int Total_Clientes_Criancas_Entraram_Filas_Bilheteiras,int Total_Clientes_Criancas_Atendidos_Bar, int Total_Clientes_Criancas_Andaram_MontanhaRussaP, int Total_Clientes_Criancas_Entraram_Carroussel, int Total_Clientes_Criancas_Entraram_Feira, int Total_Clientes_Criancas_Desistiram_Fila_Bar, int Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussaP, int Total_Clientes_Criancas_Desistiram_Fila_Carroussel, int Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras, int Total_Clientes_Publico_Geral_Entraram_Fila_Bar, int Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaG, int Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaP, int Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel, int Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras,int Total_Clientes_Publico_Geral_Atendidos_Bar,int Total_Clientes_Publico_Geral_Andaram_MontanhaRussaG, int Total_Clientes_Publico_Geral_Andaram_MontanhaRussaP, int Total_Clientes_Publico_Geral_Entraram_Carroussel,int Total_Clientes_Publico_Geral_Entraram_Feira,int Total_Clientes_Publico_Geral_Desistiram_Fila_Bar,int Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaG,int Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaP,int Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel,int Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras,int Soma_Tempos_Estadia_Feira,int Total_Clientes_Sairam_Feira)
{



//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
//1º Fazer Soma de Totais de Clientes dos Varios Tipos
//2º Fazer Medias de Tempos
//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

//Soma Totais:
//Total de Clientes que Foram Atendidos/Entraram/Andaram nas Zonas
int Total_Clientes_Atendidos_Bar=Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar+Total_Clientes_Criancas_Atendidos_Bar+Total_Clientes_Publico_Geral_Atendidos_Bar;
int Total_Clientes_Andaram_MontanhaRussaG=Total_Clientes_Publico_Geral_Andaram_MontanhaRussaG;
int Total_Clientes_Andaram_MontanhaRussaP=Total_Clientes_Publico_Geral_Andaram_MontanhaRussaP+Total_Clientes_Criancas_Andaram_MontanhaRussaP;
int Total_Clientes_Entraram_Carroussel=Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel+Total_Clientes_Criancas_Entraram_Carroussel+Total_Clientes_Publico_Geral_Entraram_Carroussel;
int Total_Clientes_Entraram_Feira=Total_Clientes_Mobilidade_Reduzida_Entraram_Feira+Total_Clientes_Criancas_Entraram_Feira+Total_Clientes_Publico_Geral_Entraram_Feira;

//Total de Clientes que Entraram nas Filas
int Total_Clientes_Entraram_Fila_Bar=Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar+Total_Clientes_Criancas_Entraram_Fila_Bar+Total_Clientes_Publico_Geral_Entraram_Fila_Bar;
int Total_Clientes_Entraram_Fila_MontanhaRussaG=Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaG;
int Total_Clientes_Entraram_Fila_MontanhaRussaP=Total_Clientes_Criancas_Entraram_Fila_MontanhaRussaP+Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaP;
int Total_Clientes_Entraram_Fila_Carroussel=Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel+Total_Clientes_Criancas_Entraram_Fila_Carroussel+Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel;
int Total_Clientes_Entraram_Filas_Bilheteiras=Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras+Total_Clientes_Criancas_Entraram_Filas_Bilheteiras+Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras;

//Total de Clientes que Desistiram
int Total_Clientes_Desistiram_Fila_Bar=Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar+Total_Clientes_Criancas_Desistiram_Fila_Bar+Total_Clientes_Publico_Geral_Desistiram_Fila_Bar;
int Total_Clientes_Desistiram_Fila_MontanhaRussaG=Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaG;
int Total_Clientes_Desistiram_Fila_MontanhaRussaP=Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussaP+Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaP;
int Total_Clientes_Desistiram_Fila_Carroussel=Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel+Total_Clientes_Criancas_Desistiram_Fila_Carroussel+Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel;
int Total_Clientes_Desistiram_Filas_Bilheteiras=Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras+Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras+Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras;

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

//Tempos Medios:
//Tempos Medios nas Filas Antes Clientes Serem Atendidos/Entrarem/Andarem nas Zonas
int Tempo_Medio_Antes_Ser_Atendido_Bar=Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido/Total_Clientes_Atendidos_Bar;
int Tempo_Medio_Antes_Andar_MontanhaRussaG=Soma_Tempos_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG/Total_Clientes_Andaram_MontanhaRussaG;
int Tempo_Medio_Antes_Andar_MontanhaRussaP=Soma_Tempos_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP/Total_Clientes_Andaram_MontanhaRussaP;
int Tempo_Medio_Antes_Entrar_Carroussel=Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel/Total_Clientes_Entraram_Carroussel;
int Tempo_Medio_Antes_Entrar_Feira=Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira/Total_Clientes_Entraram_Feira;

//Tempos Medios nas Filas Antes Clientes Desistirem das Filas das Zonas
int Tempo_Medio_Antes_Desistir_Fila_Bar=Soma_Tempos_Espera_Antes_Desistir_Fila_Bar/Total_Clientes_Desistiram_Fila_Bar;
int Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaG=Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaG/Total_Clientes_Desistiram_Fila_MontanhaRussaG;
int Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaP=Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaP/Total_Clientes_Desistiram_Fila_MontanhaRussaP;
int Tempo_Medio_Antes_Desistir_Fila_Carroussel=Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel/Total_Clientes_Desistiram_Fila_Carroussel;
int Tempo_Medio_Antes_Desistir_Filas_Bilheteiras=Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras/Total_Clientes_Desistiram_Filas_Bilheteiras;



if(Total_Clientes_Atendidos_Bar==0) {Tempo_Medio_Antes_Ser_Atendido_Bar=0;} else {Tempo_Medio_Antes_Ser_Atendido_Bar=Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido/Total_Clientes_Atendidos_Bar;};
if(Total_Clientes_Andaram_MontanhaRussaG==0) {Tempo_Medio_Antes_Andar_MontanhaRussaG=0;} else {Tempo_Medio_Antes_Andar_MontanhaRussaG=Soma_Tempos_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG/Total_Clientes_Andaram_MontanhaRussaG;};
if(Total_Clientes_Andaram_MontanhaRussaP==0) {Tempo_Medio_Antes_Andar_MontanhaRussaP=0;} else {Tempo_Medio_Antes_Andar_MontanhaRussaP=Soma_Tempos_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP/Total_Clientes_Andaram_MontanhaRussaP;};
if(Total_Clientes_Entraram_Carroussel==0) {Tempo_Medio_Antes_Entrar_Carroussel=0;} else {Tempo_Medio_Antes_Entrar_Carroussel=Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel/Total_Clientes_Entraram_Carroussel;};
if(Total_Clientes_Entraram_Feira==0) {Tempo_Medio_Antes_Entrar_Feira=0;} else {Tempo_Medio_Antes_Entrar_Feira=Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira/Total_Clientes_Entraram_Feira;};
if(Total_Clientes_Desistiram_Fila_Bar==0) {Tempo_Medio_Antes_Desistir_Fila_Bar=0;} else {Tempo_Medio_Antes_Desistir_Fila_Bar=Soma_Tempos_Espera_Antes_Desistir_Fila_Bar/Total_Clientes_Desistiram_Fila_Bar;};
if(Total_Clientes_Desistiram_Fila_MontanhaRussaG==0) {Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaG=0;} else {Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaG=Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaG/Total_Clientes_Desistiram_Fila_MontanhaRussaG;};
if(Total_Clientes_Desistiram_Fila_MontanhaRussaP==0) {Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaP=0;} else {Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaP=Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaP/Total_Clientes_Desistiram_Fila_MontanhaRussaP;};
if(Total_Clientes_Desistiram_Fila_Carroussel==0) {Tempo_Medio_Antes_Desistir_Fila_Carroussel=0;} else {Tempo_Medio_Antes_Desistir_Fila_Carroussel=Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel/Total_Clientes_Desistiram_Fila_Carroussel;};
if(Total_Clientes_Desistiram_Filas_Bilheteiras==0) {Tempo_Medio_Antes_Desistir_Filas_Bilheteiras=0;} else {Tempo_Medio_Antes_Desistir_Filas_Bilheteiras=Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras/Total_Clientes_Desistiram_Filas_Bilheteiras;};


//Tempo Medio Estadia no Parque
int Tempo_Medio_Estadia_Feira=0;
if(Total_Clientes_Sairam_Feira==0) {Tempo_Medio_Estadia_Feira=0;} else {Tempo_Medio_Estadia_Feira=Soma_Tempos_Estadia_Feira/Total_Clientes_Sairam_Feira;};



//Incluir Um If Para Cada 1 Destas Variaveis de Tempos Medios. Se o Total de CLientes Atendidos ou que Desistiram For=0, o Tempo Medio=0 (Para Evitar Erro de Divisao Por 0)


	printf("   ──────────────────────────[ Estatisticas ]──────────────────────────   \n");
	printf("Total Clientes Atendidos Bar: %d \n", Total_Clientes_Atendidos_Bar);
	printf("Total Clientes Andaram Montanha Russa Grande: %d \n",Total_Clientes_Andaram_MontanhaRussaG );
	printf("Total Clientes Andaram Montanha Russa Pequena: %d \n",Total_Clientes_Andaram_MontanhaRussaP );
	printf("Total Clientes Entraram Carroussel: %d \n",Total_Clientes_Entraram_Carroussel );
	printf("Total Clientes Entraram Feira: %d \n",Total_Clientes_Entraram_Feira );
	
	printf("Total Clientes Entraram Fila Bar: %d \n", Total_Clientes_Entraram_Fila_Bar);
	printf("Total Clientes Entraram Fila Montanha Russa Grande: %d \n",Total_Clientes_Entraram_Fila_MontanhaRussaG );
	printf("Total Clientes Entraram Fila Montanha Russa Pequeno: %d \n",Total_Clientes_Entraram_Fila_MontanhaRussaP );
	printf("Total Clientes Entraram Fila Carroussel: %d \n",Total_Clientes_Entraram_Fila_Carroussel );
	printf("Total Clientes Entraram Fila Feira: %d \n",Total_Clientes_Entraram_Filas_Bilheteiras );

	printf("Total Clientes Desistiram Fila Bar: %d \n", Total_Clientes_Desistiram_Fila_Bar);
	printf("Total Clientes Desistiram Fila Montanha Russa Grande: %d \n",Total_Clientes_Desistiram_Fila_MontanhaRussaG );
	printf("Total Clientes Desistiram Fila Montanha Russa Pequena: %d \n",Total_Clientes_Desistiram_Fila_MontanhaRussaP );
	printf("Total Clientes Desistiram Fila Carroussel: %d \n",Total_Clientes_Desistiram_Fila_Carroussel );
	printf("Total Clientes Desistiram Fila Feira: %d \n",Total_Clientes_Desistiram_Filas_Bilheteiras );

	/*printf("Tempo Medio Antes Ser Atendido Bar: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Ser_Atendido_Bar));
	printf("Tempo Medio Antes Andar Montanha Russa Grande: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Andar_MontanhaRussaG));
	printf("Tempo Medio Antes Andar Montanha Russa Pequena: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Andar_MontanhaRussaP));
	printf("Tempo Medio Antes Entrar Carroussel: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Entrar_Carroussel));
	printf("Tempo Medio Antes Entrar Feira: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Entrar_Feira));

	printf("Tempo Medio Antes Desistir Fila Bar: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Desistir_Fila_Bar));
	printf("Tempo Medio Antes Desistir Fila Montanha Russa Grande: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaG));
	printf("Tempo Medio Antes Desistir Fila Montanha Russa Pequena: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaP));
	printf("Tempo Medio Antes Desistir Fila Carroussel: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Desistir_Fila_Carroussel));
	printf("Tempo Medio Antes Desistir Fila Bilheteiras: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Antes_Desistir_Filas_Bilheteiras));

	printf("Tempo Maximo Antes Ser Atendido Bar: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar));
	printf("Tempo Maximo Antes Andar Montanha Russa Grande: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaG));
	printf("Tempo Maximo Antes Andar Montanha Russa Pequena: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaP));
	printf("Tempo Maximo Antes Entrar Carroussel: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Entrar_Carroussel));
	printf("Tempo Maximo Antes Entrar Feira: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Entrar_Feira));

	printf("Tempo Maximo Antes Desistir Fila Bar: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar));
	printf("Tempo Maximo Antes Desistir Fila Montanha Russa Grande: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaG));
	printf("Tempo Maximo Antes Desistir Fila Montanha Russa Pequena: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaP));
	printf("Tempo Maximo Antes Desistir Fila Carroussel: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel));
	printf("Tempo Maximo Antes Desistir Fila Bilheteiras: %s \n",Converte_Tempo_Para_Horas(Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras));

	printf("Tempo Medio Estadia Parque: %s \n",Converte_Tempo_Para_Horas(Tempo_Medio_Estadia_Feira));*/

printf("Tempo Medio Antes Ser Atendido Bar: %s \n",Tempo_Medio_Antes_Ser_Atendido_Bar);
	printf("Tempo Medio Antes Andar Montanha Russa Grande: %s \n",Tempo_Medio_Antes_Andar_MontanhaRussaG);
	printf("Tempo Medio Antes Andar Montanha Russa Pequena: %s \n",Tempo_Medio_Antes_Andar_MontanhaRussaP);
	printf("Tempo Medio Antes Entrar Carroussel: %s \n",Tempo_Medio_Antes_Entrar_Carroussel);
	printf("Tempo Medio Antes Entrar Feira: %s \n",Tempo_Medio_Antes_Entrar_Feira);

	printf("Tempo Medio Antes Desistir Fila Bar: %s \n",Tempo_Medio_Antes_Desistir_Fila_Bar);
	printf("Tempo Medio Antes Desistir Fila Montanha Russa Grande: %s \n",Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaG);
	printf("Tempo Medio Antes Desistir Fila Montanha Russa Pequena: %s \n",Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaP);
	printf("Tempo Medio Antes Desistir Fila Carroussel: %s \n",Tempo_Medio_Antes_Desistir_Fila_Carroussel);
	printf("Tempo Medio Antes Desistir Fila Bilheteiras: %s \n",Tempo_Medio_Antes_Desistir_Filas_Bilheteiras);

	printf("Tempo Maximo Antes Ser Atendido Bar: %s \n",Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar);
	printf("Tempo Maximo Antes Andar Montanha Russa Grande: %s \n",Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaG);
	printf("Tempo Maximo Antes Andar Montanha Russa Pequena: %s \n",Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaP);
	printf("Tempo Maximo Antes Entrar Carroussel: %s \n",Tempo_Maximo_Espera_Antes_Entrar_Carroussel);
	printf("Tempo Maximo Antes Entrar Feira: %s \n",Tempo_Maximo_Espera_Antes_Entrar_Feira);

	printf("Tempo Maximo Antes Desistir Fila Bar: %s \n",Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar);
	printf("Tempo Maximo Antes Desistir Fila Montanha Russa Grande: %s \n",Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaG);
	printf("Tempo Maximo Antes Desistir Fila Montanha Russa Pequena: %s \n",Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaP);
	printf("Tempo Maximo Antes Desistir Fila Carroussel: %s \n",Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel);
	printf("Tempo Maximo Antes Desistir Fila Bilheteiras: %s \n",Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras);

	printf("Tempo Medio Estadia Feira: %s \n",Tempo_Medio_Estadia_Feira);
 printf("   ───────────────────────────────────────────────────────────────────────── \n");	


}


//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------



//Funcao Para Escrever as Estatisticas da Feira no Ficheiro Estatisticas (So se Invoca no Final da Simulacao. Esta Funcao Nao Precisa de Uma Funcao "Preparatoria" Porque So Vai Ser Invocada Uma Vez No Final.):
void Escreve_Estatisticas(int Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar,int Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaG, int Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaP,int Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel,int Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras,int Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar,int Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaG,int Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaP,int Tempo_Maximo_Espera_Antes_Entrar_Carroussel,int Tempo_Maximo_Espera_Antes_Entrar_Feira,int Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido,int Soma_Tempos_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG,int Soma_Tempos_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP,int Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel,int Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira,int Soma_Tempos_Espera_Antes_Desistir_Fila_Bar,int Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaG,int Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaP,int Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel,int Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras,int Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar,int Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel,int Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras,int Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar,int Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel,int Total_Clientes_Mobilidade_Reduzida_Entraram_Feira,int Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar,int Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel,int Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras,int Total_Clientes_Criancas_Entraram_Fila_Bar,int Total_Clientes_Criancas_Entraram_Fila_MontanhaRussaP,int Total_Clientes_Criancas_Entraram_Fila_Carroussel,int Total_Clientes_Criancas_Entraram_Filas_Bilheteiras,int Total_Clientes_Criancas_Atendidos_Bar,int Total_Clientes_Criancas_Andaram_MontanhaRussaP,int Total_Clientes_Criancas_Entraram_Carroussel,int Total_Clientes_Criancas_Entraram_Feira,int Total_Clientes_Criancas_Desistiram_Fila_Bar,int Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussaP,int Total_Clientes_Criancas_Desistiram_Fila_Carroussel,int Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras,int Total_Clientes_Publico_Geral_Entraram_Fila_Bar,int Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaG,int Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaP,int Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel,int Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras,int Total_Clientes_Publico_Geral_Atendidos_Bar,int Total_Clientes_Publico_Geral_Andaram_MontanhaRussaG,int Total_Clientes_Publico_Geral_Andaram_MontanhaRussaP,int Total_Clientes_Publico_Geral_Entraram_Carroussel,int Total_Clientes_Publico_Geral_Entraram_Feira,int Total_Clientes_Publico_Geral_Desistiram_Fila_Bar,int Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaG,int Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaP,int Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel,int Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras,int Soma_Tempos_Estadia_Feira,int Total_Clientes_Sairam_Feira)
{

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------
//1º Fazer Soma de Totais de Clientes dos Varios Tipos
//2º Fazer Medias de Tempos
//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

//Soma Totais:
//Total de Clientes que Foram Atendidos/Entraram/Andaram nas Zonas
/*int Total_Clientes_Atendidos_Bar=Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar+Total_Clientes_Criancas_Atendidos_Bar+Total_Clientes_Publico_Geral_Atendidos_Bar;
int Total_Clientes_Andaram_MontanhaRussaG=Total_Clientes_Publico_Geral_Andaram_MontanhaRussaG;
int Total_Clientes_Andaram_MontanhaRussaP=Total_Clientes_Publico_Geral_Andaram_MontanhaRussaG;
int Total_Clientes_Entraram_Piscina=Total_Clientes_Mobilidade_Reduzida_Entraram_Piscina+Total_Clientes_Criancas_Entraram_Piscina+Total_Clientes_Publico_Geral_Entraram_Piscina;
int Total_Clientes_Entraram_Parque=Total_Clientes_Mobilidade_Reduzida_Entraram_Parque+Total_Clientes_Criancas_Entraram_Parque+Total_Clientes_Publico_Geral_Entraram_Parque;

//Total de Clientes que Entraram nas Filas
int Total_Clientes_Entraram_Fila_Bar=Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar+Total_Clientes_Criancas_Entraram_Fila_Bar+Total_Clientes_Publico_Geral_Entraram_Fila_Bar;
int Total_Clientes_Entraram_Fila_Escorrega=Total_Clientes_Criancas_Entraram_Fila_Escorrega+Total_Clientes_Publico_Geral_Entraram_Fila_Escorrega;
int Total_Clientes_Entraram_Fila_Piscina=Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Piscina+Total_Clientes_Criancas_Entraram_Fila_Piscina+Total_Clientes_Publico_Geral_Entraram_Fila_Piscina;
int Total_Clientes_Entraram_Filas_Bilheteiras=Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras+Total_Clientes_Criancas_Entraram_Filas_Bilheteiras+Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras;

//Total de Clientes que Desistiram
int Total_Clientes_Desistiram_Fila_Bar=Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar+Total_Clientes_Criancas_Desistiram_Fila_Bar+Total_Clientes_Publico_Geral_Desistiram_Fila_Bar;
int Total_Clientes_Desistiram_Fila_Escorrega=Total_Clientes_Criancas_Desistiram_Fila_Escorrega+Total_Clientes_Publico_Geral_Desistiram_Fila_Escorrega;
int Total_Clientes_Desistiram_Fila_Piscina=Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Piscina+Total_Clientes_Criancas_Desistiram_Fila_Piscina+Total_Clientes_Publico_Geral_Desistiram_Fila_Piscina;
int Total_Clientes_Desistiram_Filas_Bilheteiras=Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras+Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras+Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras;

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

//Tempos Medios:
//Tempos Medios nas Filas Antes Clientes Serem Atendidos/Entrarem/Andarem nas Zonas
int Tempo_Medio_Antes_Ser_Atendido_Bar=Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido/Total_Clientes_Atendidos_Bar;
int Tempo_Medio_Antes_Andar_Escorrega=Soma_Tempos_Espera_Fila_Escorrega_Antes_Andar_Escorrega/Total_Clientes_Andaram_Escorrega;
int Tempo_Medio_Antes_Entrar_Piscina=Soma_Tempos_Espera_Fila_Piscina_Antes_Entrar_Piscina/Total_Clientes_Entraram_Piscina;
int Tempo_Medio_Antes_Entrar_Parque=Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Parque/Total_Clientes_Entraram_Parque;

//Tempos Medios nas Filas Antes Clientes Desistirem das Filas das Zonas
int Tempo_Medio_Antes_Desistir_Fila_Bar=Soma_Tempos_Espera_Antes_Desistir_Fila_Bar/Total_Clientes_Desistiram_Fila_Bar;
int Tempo_Medio_Antes_Desistir_Fila_Escorrega=Soma_Tempos_Espera_Antes_Desistir_Fila_Escorrega/Total_Clientes_Desistiram_Fila_Escorrega;
int Tempo_Medio_Antes_Desistir_Fila_Piscina=Soma_Tempos_Espera_Antes_Desistir_Fila_Piscina/Total_Clientes_Desistiram_Fila_Piscina;
int Tempo_Medio_Antes_Desistir_Filas_Bilheteiras=Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras/Total_Clientes_Desistiram_Filas_Bilheteiras;



if(Total_Clientes_Atendidos_Bar==0) {Tempo_Medio_Antes_Ser_Atendido_Bar=0;} else {Tempo_Medio_Antes_Ser_Atendido_Bar=Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido/Total_Clientes_Atendidos_Bar;};
if(Total_Clientes_Andaram_Escorrega==0) {Tempo_Medio_Antes_Andar_Escorrega=0;} else {Tempo_Medio_Antes_Andar_Escorrega=Soma_Tempos_Espera_Fila_Escorrega_Antes_Andar_Escorrega/Total_Clientes_Andaram_Escorrega;};
if(Total_Clientes_Entraram_Piscina==0) {Tempo_Medio_Antes_Entrar_Piscina=0;} else {Tempo_Medio_Antes_Entrar_Piscina=Soma_Tempos_Espera_Fila_Piscina_Antes_Entrar_Piscina/Total_Clientes_Entraram_Piscina;};
if(Total_Clientes_Entraram_Parque==0) {Tempo_Medio_Antes_Entrar_Parque=0;} else {Tempo_Medio_Antes_Entrar_Parque=Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Parque/Total_Clientes_Entraram_Parque;};

if(Total_Clientes_Desistiram_Fila_Bar==0) {Tempo_Medio_Antes_Desistir_Fila_Bar=0;} else {Tempo_Medio_Antes_Desistir_Fila_Bar=Soma_Tempos_Espera_Antes_Desistir_Fila_Bar/Total_Clientes_Desistiram_Fila_Bar;};
if(Total_Clientes_Desistiram_Fila_Escorrega==0) {Tempo_Medio_Antes_Desistir_Fila_Escorrega=0;} else {Tempo_Medio_Antes_Desistir_Fila_Escorrega=Soma_Tempos_Espera_Antes_Desistir_Fila_Escorrega/Total_Clientes_Desistiram_Fila_Escorrega;};
if(Total_Clientes_Desistiram_Fila_Piscina==0) {Tempo_Medio_Antes_Desistir_Fila_Piscina=0;} else {Tempo_Medio_Antes_Desistir_Fila_Piscina=Soma_Tempos_Espera_Antes_Desistir_Fila_Piscina/Total_Clientes_Desistiram_Fila_Piscina;};
if(Total_Clientes_Desistiram_Filas_Bilheteiras==0) {Tempo_Medio_Antes_Desistir_Filas_Bilheteiras=0;} else {Tempo_Medio_Antes_Desistir_Filas_Bilheteiras=Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras/Total_Clientes_Desistiram_Filas_Bilheteiras;};
*/


//Soma Totais:
//Total de Clientes que Foram Atendidos/Entraram/Andaram nas Zonas
int Total_Clientes_Atendidos_Bar=Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar+Total_Clientes_Criancas_Atendidos_Bar+Total_Clientes_Publico_Geral_Atendidos_Bar;
int Total_Clientes_Andaram_MontanhaRussaG=Total_Clientes_Publico_Geral_Andaram_MontanhaRussaG;
int Total_Clientes_Andaram_MontanhaRussaP=Total_Clientes_Publico_Geral_Andaram_MontanhaRussaP+Total_Clientes_Criancas_Andaram_MontanhaRussaP;
int Total_Clientes_Entraram_Carroussel=Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel+Total_Clientes_Criancas_Entraram_Carroussel+Total_Clientes_Publico_Geral_Entraram_Carroussel;
int Total_Clientes_Entraram_Feira=Total_Clientes_Mobilidade_Reduzida_Entraram_Feira+Total_Clientes_Criancas_Entraram_Feira+Total_Clientes_Publico_Geral_Entraram_Feira;

//Total de Clientes que Entraram nas Filas
int Total_Clientes_Entraram_Fila_Bar=Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar+Total_Clientes_Criancas_Entraram_Fila_Bar+Total_Clientes_Publico_Geral_Entraram_Fila_Bar;
int Total_Clientes_Entraram_Fila_MontanhaRussaG=Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaG;
int Total_Clientes_Entraram_Fila_MontanhaRussaP=Total_Clientes_Criancas_Entraram_Fila_MontanhaRussaP+Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaP;
int Total_Clientes_Entraram_Fila_Carroussel=Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel+Total_Clientes_Criancas_Entraram_Fila_Carroussel+Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel;
int Total_Clientes_Entraram_Filas_Bilheteiras=Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras+Total_Clientes_Criancas_Entraram_Filas_Bilheteiras+Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras;

//Total de Clientes que Desistiram
int Total_Clientes_Desistiram_Fila_Bar=Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar+Total_Clientes_Criancas_Desistiram_Fila_Bar+Total_Clientes_Publico_Geral_Desistiram_Fila_Bar;
int Total_Clientes_Desistiram_Fila_MontanhaRussaG=Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaG;
int Total_Clientes_Desistiram_Fila_MontanhaRussaP=Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussaP+Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaP;
int Total_Clientes_Desistiram_Fila_Carroussel=Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel+Total_Clientes_Criancas_Desistiram_Fila_Carroussel+Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel;
int Total_Clientes_Desistiram_Filas_Bilheteiras=Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras+Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras+Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras;

//----------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------

//Tempos Medios:
//Tempos Medios nas Filas Antes Clientes Serem Atendidos/Entrarem/Andarem nas Zonas
int Tempo_Medio_Antes_Ser_Atendido_Bar=Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido/Total_Clientes_Atendidos_Bar;
int Tempo_Medio_Antes_Andar_MontanhaRussaG=Soma_Tempos_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG/Total_Clientes_Andaram_MontanhaRussaG;
int Tempo_Medio_Antes_Andar_MontanhaRussaP=Soma_Tempos_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP/Total_Clientes_Andaram_MontanhaRussaP;
int Tempo_Medio_Antes_Entrar_Carroussel=Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel/Total_Clientes_Entraram_Carroussel;
int Tempo_Medio_Antes_Entrar_Feira=Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira/Total_Clientes_Entraram_Feira;

//Tempos Medios nas Filas Antes Clientes Desistirem das Filas das Zonas
int Tempo_Medio_Antes_Desistir_Fila_Bar=Soma_Tempos_Espera_Antes_Desistir_Fila_Bar/Total_Clientes_Desistiram_Fila_Bar;
int Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaG=Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaG/Total_Clientes_Desistiram_Fila_MontanhaRussaG;
int Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaP=Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaP/Total_Clientes_Desistiram_Fila_MontanhaRussaP;
int Tempo_Medio_Antes_Desistir_Fila_Carroussel=Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel/Total_Clientes_Desistiram_Fila_Carroussel;
int Tempo_Medio_Antes_Desistir_Filas_Bilheteiras=Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras/Total_Clientes_Desistiram_Filas_Bilheteiras;



if(Total_Clientes_Atendidos_Bar==0) {Tempo_Medio_Antes_Ser_Atendido_Bar=0;} else {Tempo_Medio_Antes_Ser_Atendido_Bar=Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido/Total_Clientes_Atendidos_Bar;};
if(Total_Clientes_Andaram_MontanhaRussaG==0) {Tempo_Medio_Antes_Andar_MontanhaRussaG=0;} else {Tempo_Medio_Antes_Andar_MontanhaRussaG=Soma_Tempos_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG/Total_Clientes_Andaram_MontanhaRussaG;};
if(Total_Clientes_Andaram_MontanhaRussaP==0) {Tempo_Medio_Antes_Andar_MontanhaRussaP=0;} else {Tempo_Medio_Antes_Andar_MontanhaRussaP=Soma_Tempos_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP/Total_Clientes_Andaram_MontanhaRussaP;};
if(Total_Clientes_Entraram_Carroussel==0) {Tempo_Medio_Antes_Entrar_Carroussel=0;} else {Tempo_Medio_Antes_Entrar_Carroussel=Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel/Total_Clientes_Entraram_Carroussel;};
if(Total_Clientes_Entraram_Feira==0) {Tempo_Medio_Antes_Entrar_Feira=0;} else {Tempo_Medio_Antes_Entrar_Feira=Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira/Total_Clientes_Entraram_Feira;};
if(Total_Clientes_Desistiram_Fila_Bar==0) {Tempo_Medio_Antes_Desistir_Fila_Bar=0;} else {Tempo_Medio_Antes_Desistir_Fila_Bar=Soma_Tempos_Espera_Antes_Desistir_Fila_Bar/Total_Clientes_Desistiram_Fila_Bar;};
if(Total_Clientes_Desistiram_Fila_MontanhaRussaG==0) {Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaG=0;} else {Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaG=Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaG/Total_Clientes_Desistiram_Fila_MontanhaRussaG;};
if(Total_Clientes_Desistiram_Fila_MontanhaRussaP==0) {Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaP=0;} else {Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaP=Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaP/Total_Clientes_Desistiram_Fila_MontanhaRussaP;};
if(Total_Clientes_Desistiram_Fila_Carroussel==0) {Tempo_Medio_Antes_Desistir_Fila_Carroussel=0;} else {Tempo_Medio_Antes_Desistir_Fila_Carroussel=Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel/Total_Clientes_Desistiram_Fila_Carroussel;};
if(Total_Clientes_Desistiram_Filas_Bilheteiras==0) {Tempo_Medio_Antes_Desistir_Filas_Bilheteiras=0;} else {Tempo_Medio_Antes_Desistir_Filas_Bilheteiras=Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras/Total_Clientes_Desistiram_Filas_Bilheteiras;};

//Tempo Medio Estadia no Parque
int Tempo_Medio_Estadia_Feira=0;
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
	fprintf(fp, "|--------- Clientes Andaram na Montanha Russa Grande -------| \n");
	fprintf(fp, "|-----------------------------------------------------------| \n");
	fprintf(fp, "|-----------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Andaram Montanha Russa Grande: %d \n",Total_Clientes_Andaram_MontanhaRussaG );
	fprintf(fp, "Total Clientes Publico Geral Andaram Montanha Russa Grande: %d \n",Total_Clientes_Publico_Geral_Andaram_MontanhaRussaG );
	fprintf(fp, "\n");
	fprintf(fp, "|--------Clientes Andaram na Montanha Russa Pequena --------| \n");
	fprintf(fp, "|-----------------------------------------------------------| \n");
	fprintf(fp, "|-----------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Andaram Montanha Russa Pequena: %d \n",Total_Clientes_Andaram_MontanhaRussaP );
	fprintf(fp, "Total Clientes Crianças Andaram Montanha Russa Pequena: %d \n",Total_Clientes_Criancas_Andaram_MontanhaRussaP );
	fprintf(fp, "Total Clientes Publico Geral Andaram Montanha Russa Pequena: %d \n",Total_Clientes_Publico_Geral_Andaram_MontanhaRussaP );
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
	fprintf(fp, "|----------- Clientes Entraram na Feira -----------------| \n");
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
	fprintf(fp, "|------- Clientes Entraram na Fila da Montanha Russa Grande----------| \n");
	fprintf(fp, "|--------------------------------------------------------------------| \n");
	fprintf(fp, "|--------------------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Entraram Fila Montanha Russa Grande: %d \n",Total_Clientes_Entraram_Fila_MontanhaRussaG );
	fprintf(fp, "\n");
	fprintf(fp, "|------ Clientes Entraram na Fila da Montanha Russa Pequena----------| \n");
	fprintf(fp, "|--------------------------------------------------------------------| \n");
	fprintf(fp, "|--------------------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Entraram Fila Montanha Russa Pequena: %d \n",Total_Clientes_Entraram_Fila_MontanhaRussaP );
	fprintf(fp, "Total Clientes Crianças Entraram Fila Montanha Russa Pequena: %d \n",Total_Clientes_Criancas_Entraram_Fila_MontanhaRussaP );
	fprintf(fp, "Total Clientes Publico Geral Entraram Fila Montanha Russa Pequena: %d \n",Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaP );
	fprintf(fp, "\n");
	fprintf(fp, "|------------------------------------------------------------------| \n");
	fprintf(fp, "|------------------------------------------------------------------| \n");
	fprintf(fp, "|----------- Clientes Entraram na Fila da Carroussel -----------------| \n");
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
	fprintf(fp, "|------ Clientes Desistiram da Fila da Montanha Russa Grande ----------| \n");
	fprintf(fp, "|----------------------------------------------------------------------| \n");
	fprintf(fp, "|----------------------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Total Clientes Desistiram Fila Montanha Russa Grande: %d \n",Total_Clientes_Desistiram_Fila_MontanhaRussaG );
	fprintf(fp, "\n");
	fprintf(fp, "|--------------------------------------------------------------------| \n");
	fprintf(fp, "|--------------------------------------------------------------------| \n");
	fprintf(fp, "|----------- Clientes Desistiram da Fila do Carroussel -----------------| \n");
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
	fprintf(fp, "Tempo Medio Antes Ser Atendido Bar: %s \n",Tempo_Medio_Antes_Ser_Atendido_Bar);
	fprintf(fp, "Tempo Medio Antes Andar Montanha Russa Grande: %s \n",Tempo_Medio_Antes_Andar_MontanhaRussaG);
	fprintf(fp, "Tempo Medio Antes Andar Montanha Russa Pequena: %s \n",Tempo_Medio_Antes_Andar_MontanhaRussaP);
	fprintf(fp, "Tempo Medio Antes Entrar Carroussel: %s \n",Tempo_Medio_Antes_Entrar_Carroussel);
	fprintf(fp, "Tempo Medio Antes Entrar Feira: %s \n",Tempo_Medio_Antes_Entrar_Feira);

	fprintf(fp, "Tempo Medio Antes Desistir Fila Bar: %s \n",Tempo_Medio_Antes_Desistir_Fila_Bar);
	fprintf(fp, "Tempo Medio Antes Desistir Fila Montanha Russa Grande: %s \n",Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaG);
	fprintf(fp, "Tempo Medio Antes Desistir Fila Montanha Russa Pequena: %s \n",Tempo_Medio_Antes_Desistir_Fila_MontanhaRussaP);
	fprintf(fp, "Tempo Medio Antes Desistir Fila Carroussel: %s \n",Tempo_Medio_Antes_Desistir_Fila_Carroussel);
	fprintf(fp, "Tempo Medio Antes Desistir Fila Bilheteiras: %s \n",Tempo_Medio_Antes_Desistir_Filas_Bilheteiras);
	fprintf(fp, "Tempo Medio Estadia Feira: %s \n",Tempo_Medio_Estadia_Feira);

	fprintf(fp, "|-------------------------------------------------------------------------| \n");
	fprintf(fp, "|-------------------------------------------------------------------------| \n");
	fprintf(fp, "|--------------------------- Tempos Maximos ------------------------------| \n");
	fprintf(fp, "|-------------------------------------------------------------------------| \n");
	fprintf(fp, "|-------------------------------------------------------------------------| \n");
	fprintf(fp, "\n");
	fprintf(fp, "Tempo Maximo Antes Ser Atendido Bar: %s \n",Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar);
	fprintf(fp, "Tempo Maximo Antes Andar Montanha Russa Grande: %s \n",Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaG);
	fprintf(fp, "Tempo Maximo Antes Andar Montanha Russa Pequena: %s \n",Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaP);
	fprintf(fp, "Tempo Maximo Antes Entrar Carroussel: %s \n",Tempo_Maximo_Espera_Antes_Entrar_Carroussel);
	fprintf(fp, "Tempo Maximo Antes Entrar Feira: %s \n",Tempo_Maximo_Espera_Antes_Entrar_Feira);

	fprintf(fp, "Tempo Maximo Antes Desistir Fila Bar: %s \n",Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar);
	fprintf(fp, "Tempo Maximo Antes Desistir Fila Montanha Russa Grande: %s \n",Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaG);
	fprintf(fp, "Tempo Maximo Antes Desistir Fila Montanha Russa Pequena: %s \n",Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaP);
	fprintf(fp, "Tempo Maximo Antes Desistir Fila Carroussel: %s \n",Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel);
	fprintf(fp, "Tempo Maximo Antes Desistir Fila Bilheteiras: %s \n",Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras);


	fclose(fp);
}


//Funcao que Mostra o Log de Eventos em Tempo Real
void Mostra_No_Monitor_Eventos_Tempo_Real(int Evento, int ID_Cliente, int Horas_Evento)
{
//☺=Chegou Fila
//☹=Desistiu Fila
//😃=Entrou/Atendido/Andou

	switch(Evento)
	{
		case 1:
		printf("☺ Cliente Nº %i Chegou a Fila das Bilheteiras As: %s \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 2:
		printf("😃 Cliente Nº %i Entrou na Feira As: %s \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 3:
		printf("☹ Cliente Nº %i Desistiu da Fila das Bilheteiras As: %s \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 4:
		printf("☺ Cliente Nº %i Chegou a Fila do Bar As: %s \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 5:
		printf("😃 Cliente Nº %i Foi Atendido no Bar As: %s e Saiu do Bar \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 6:
		printf("😃 Cliente Nº %i Foi Atendido no Bar As: %s e Ficou no Bar \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 7:
		printf("☹ Cliente Nº %i Desistiu da Fila do Bar As: %s \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 8:
		printf("☹ Cliente Nº %i Saiu do Bar As: %s \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 9:
		printf("☺ Cliente Nº %i Chegou a Fila do Carroussel As: %s \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 10:
		printf("😃 Cliente Nº %i Entrou na Carroussel As: %s \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 11:
		printf("☹ Cliente Nº %i Desistiu da Fila da Carroussel As: %s \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 12:
		printf("☺ Cliente Nº %i Chegou a Fila da Montanha Russa Grande As: %s \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 13:
		printf("😃 Cliente Nº %i Andou na Montanha Russa Grande As: %s \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 14:
		printf("☹ Cliente Nº %i Desistiu da Fila da Montanha Russa Grande As: %s \n",ID_Cliente+1,Horas_Evento);

		break;
			//---------------------------------------------------------------------------------------------------------
		case 15:
		printf("☺ Cliente Nº %i Chegou a Fila da Montanha Russa Pequena As: %s \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 16:
		printf("😃 Cliente Nº %i Andou na Montanha Russa Pequena As: %s \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 17:
		printf("☹ Cliente Nº %i Desistiu da Fila da Montanha Russa Pequena As: %s \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 18:
		printf("☹ Cliente Nº %i Saiu do Parque As: %s \n",ID_Cliente+1,Horas_Evento);

		break;
	//---------------------------------------------------------------------------------------------------------
		case 50:
		printf("Simulacao Terminada! \n");

		break;
	//---------------------------------------------------------------------------------------------------------
		default:
		printf("Erro: Caso Nao Definido \n");


	}

}



//Funcao Que Recebe Mensagem da Socket e Passa Cada Campo da Mensagem Para Uma Variavel Propria
int * Recebe_Mensagem(int sockfd)
{
	int Mensagem_Recebida=0;
	int ID_Cliente=0;
	int Tipo_Cliente=0;
	int Estado=0; 
	int Hora_Actual=0;
	int Tempo_Espera_Maximo_Filas=0;
	int Tempo_Estadia_Feira=0; 
	int Hora_Entrada_Fila_Bar=0; 
	int Hora_Desistencia_Fila_Bar=0;
	int Hora_Atendimento_Bar=0;
	int Hora_Entrada_Fila_Carroussel=0;
	int Hora_Desistencia_Fila_Carroussel=0;
	int Hora_Entrada_Carroussel=0;
	int Hora_Entrada_Fila_MontanhaRussaG=0;
	int Hora_Desistencia_Fila_MontanhaRussaG=0;
	int Hora_Andou_MontanhaRussaG=0;
	int Hora_Entrada_Fila_MontanhaRussaP=0;
	int Hora_Desistencia_Fila_MontanhaRussaP=0;
	int Hora_Andou_MontanhaRussaP=0;
	int Hora_Entrada_Filas_Bilheteiras=0;
	int Hora_Desistencia_Filas_Bilheteiras=0;
	int Hora_Entrada_Feira=0;
	int Hora_Saida_Feira=0;
	int Tempo_Maximo_Espera_Antes_Entrar_Feira=0;

	char recvline[MAX_LINE+1];
	static int Mensagem[22]; //Conversao dos Valores da Mensagem Para Array a Ser Enviado Para Interpretacao


	Mensagem_Recebida = read(sockfd, recvline, MAX_LINE);

	if (Mensagem_Recebida < 0) perror("Erro ao ler mensagem no servidor\n");
	recvline[Mensagem_Recebida] = 0;
	
	sscanf(recvline,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &ID_Cliente, &Tipo_Cliente, &Estado, &Hora_Actual, &Tempo_Espera_Maximo_Filas, &Tempo_Estadia_Feira, &Hora_Entrada_Fila_Bar, &Hora_Desistencia_Fila_Bar, &Hora_Atendimento_Bar, &Hora_Entrada_Fila_Carroussel, &Hora_Desistencia_Fila_Carroussel, &Hora_Entrada_Carroussel, &Hora_Entrada_Fila_MontanhaRussaG, &Hora_Desistencia_Fila_MontanhaRussaG, &Hora_Andou_MontanhaRussaG, &Hora_Entrada_Fila_MontanhaRussaP, &Hora_Desistencia_Fila_MontanhaRussaP, &Hora_Andou_MontanhaRussaP,&Hora_Entrada_Filas_Bilheteiras, &Hora_Desistencia_Filas_Bilheteiras, &Hora_Entrada_Feira,&Hora_Saida_Feira);

	Mensagem[0] = ID_Cliente;
	Mensagem[1] = Tipo_Cliente;
	Mensagem[2] = Estado;
	Mensagem[3] = Hora_Actual;
	Mensagem[4] = Tempo_Espera_Maximo_Filas;
	Mensagem[5] = Tempo_Estadia_Feira;
	Mensagem[6] = Hora_Entrada_Fila_Bar;
	Mensagem[7] = Hora_Desistencia_Fila_Bar;
	Mensagem[8] = Hora_Atendimento_Bar;
	Mensagem[9] = Hora_Entrada_Fila_Carroussel;
	Mensagem[10] = Hora_Desistencia_Fila_Carroussel;
	Mensagem[11] = Hora_Entrada_Carroussel;
	Mensagem[12] = Hora_Entrada_Fila_MontanhaRussaG;
	Mensagem[13] = Hora_Desistencia_Fila_MontanhaRussaG;
	Mensagem[14] = Hora_Andou_MontanhaRussaG;
	Mensagem[15] = Hora_Entrada_Fila_MontanhaRussaP;
	Mensagem[16] = Hora_Desistencia_Fila_MontanhaRussaP;
	Mensagem[17] = Hora_Andou_MontanhaRussaP;
	Mensagem[18] = Hora_Entrada_Filas_Bilheteiras;
	Mensagem[19] = Hora_Desistencia_Filas_Bilheteiras;
	Mensagem[20] = Hora_Entrada_Feira;
	Mensagem[21] = Hora_Saida_Feira;
	//Mensagem[22] = Tempo_Maximo_Espera_Antes_Entrar_Feira;

	
	return Mensagem;
}

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
void Escreve_Ficheiro_Eventos(int Evento, int ID_Cliente, int Horas_Evento)
{
	//ID's dos Clientes Começam no 0, Logo Temos de Por ID_Cliente+1

	FILE *fp;
	fp=fopen("relatorio_eventos.c","a"); //"a" = Append (Ou Seja, a Linha é Adicionada ao Ficheiro Sem Eliminar Nada)
	
	switch(Evento)
	{
		case 1:
		fprintf(fp, "->Cliente Nº %i Chegou a  Fila das Bilheteiras As: %s \n", ID_Cliente+1, Horas_Evento); 
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 2:
		fprintf(fp, "->Cliente Nº %i Entrou no Feira As: %s \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 3:
		fprintf(fp, "->Cliente Nº %i Desistiu da Fila das Bilheteiras As: %s \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 4:
		fprintf(fp, "->Cliente Nº %i Chegou a Fila do Bar As: %s \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 5:
		fprintf(fp, "->Cliente Nº %i Foi Atendido no Bar As: %s e Saiu do Bar \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 6:
		fprintf(fp, "->Cliente Nº %i Foi Atendido no Bar As: %s E Ficou no Bar \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 7:
		fprintf(fp, "->Cliente Nº %i Desistiu da Fila do Bar As: %s \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 8:
		fprintf(fp, "->Cliente Nº %i Saiu do Bar As: %s \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 9:
		fprintf(fp, "->Cliente Nº %i Chegou a Fila do Carroussel As: %s \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 10:
		fprintf(fp, "->Cliente Nº %i Entrou no Carroussel As: %s \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 11:
		fprintf(fp, "->Cliente Nº %i Desistiu da Fila do Carroussel As: %s \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 12:
		fprintf(fp, "->Cliente Nº %i Chegou a Fila da Montanha Russa Grande As: %s \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 13:
		fprintf(fp, "->Cliente Nº %i Andou na Montanha Russa Grande As: %s \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 14:
		fprintf(fp, "->Cliente Nº %i Desistiu da Fila da Montanha Russa Grande As: %s \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
		//---------------------------------------------------------------------------------------------------------------------------
		case 15:
		fprintf(fp, "->Cliente Nº %i Chegou a Fila da Montanha Russa Pequena As: %s \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 16:
		fprintf(fp, "->Cliente Nº %i Andou na Montanha Russa Pequena As: %s \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 17:
		fprintf(fp, "->Cliente Nº %i Desistiu da Fila da Montanha Russa Pequena As: %s \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 18:
		fprintf(fp, "->Cliente Nº %i Saiu da Feira As: %s \n", ID_Cliente+1, Horas_Evento);  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		case 50:
		fprintf(fp, "->Simulacao Terminada \n");  
		fprintf(fp, "-----------------------------------------------------\n");
		break;
//---------------------------------------------------------------------------------------------------------------------------
		default:
		fprintf(fp, "-Erro: Evento Nao Definido\n"); 
		fprintf(fp, "-----------------------------------------------------\n");

	}
	fclose(fp);
}




//Funcao de Leitura e Interpretacao da Mensagem Recebida Pela Socket
void Interpreta_Mensagem()
{
	int* Buffer=Recebe_Mensagem(newsockfd);
	int ID_Cliente=Buffer[0]; //Numero Processo
	int Tipo_Cliente=Buffer[1]; //Tipo Cliente= {Mobilidade Reduzida, Crianças, Publico Geral}
	int Estado=Buffer[2]; //Ver Tabela de Estados
	int Hora_Actual=Buffer[3]; //Hora de um Acontecimento (Depende do Estado em que Esta e Esteve)
	int Tempo_Espera_Maximo_Filas=Buffer[4]; //Tempo Maximo que o Cliente Vai Esperar Numa Fila (Este Valor é Actualizado Sempre Que Entra Numa Zona)
	int Tempo_Estadia_Feira=Buffer[5]; //Tempo que o Cliente Vai Ficar na Feira (Processo Termina Apos Este Tempo Acabar)
	int Hora_Entrada_Fila_Bar=Buffer[6];
	int Hora_Desistencia_Fila_Bar=Buffer[7];
	int Hora_Atendimento_Bar=Buffer[8];
	int Hora_Entrada_Fila_Carroussel=Buffer[9];
	int Hora_Desistencia_Fila_Carroussel=Buffer[10];
	int Hora_Entrada_Carroussel=Buffer[11];
	int Hora_Entrada_Fila_MontanhaRussaG=Buffer[12];
	int Hora_Desistencia_Fila_MontanhaRussaG=Buffer[13];
	int Hora_Andou_MontanhaRussaG=Buffer[14];
	int Hora_Entrada_Fila_MontanhaRussaP=Buffer[15];
	int Hora_Desistencia_Fila_MontanhaRussaP=Buffer[16];
	int Hora_Andou_MontanhaRussaP=Buffer[17];
	int Hora_Entrada_Filas_Bilheteiras=Buffer[18];
	int Hora_Desistencia_Filas_Bilheteiras=Buffer[19];
	int Hora_Entrada_Feira=Buffer[20];
	int Hora_Saida_Feira=Buffer[21];
	//int Tempo_Maximo_Espera_Antes_Entrar_Feira [22];

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
		case 8: //Sai Bar
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
Tempo_Espera_Fila_Carroussel_Antes_Entrar_Carrousssel=Hora_Entrada_Carroussel-Hora_Entrada_Fila_Carroussel;
if(Tempo_Maximo_Espera_Antes_Entrar_Carroussel==0)
{
	Tempo_Maximo_Espera_Antes_Entrar_Carroussel=Tempo_Espera_Fila_Carroussel_Antes_Entrar_Carrousssel;
}
else if(Tempo_Espera_Fila_Carroussel_Antes_Entrar_Carrousssel>Tempo_Maximo_Espera_Antes_Entrar_Carroussel)
{
	Tempo_Maximo_Espera_Antes_Entrar_Carroussel=Tempo_Espera_Fila_Carroussel_Antes_Entrar_Carrousssel;
}

//Variaveis Para Tempos Médios:
Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel=Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel+Tempo_Espera_Fila_Carroussel_Antes_Entrar_Carrousssel;

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
		case 12: //Entra Fila Montanha Russa Grande
		Total_Clientes_Acutalmente_Na_Fila_MontanhaRussaG++;

	if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaG++;
		}
		break;
	//---------------------------------------------------------------------------------------
		case 13: //Sai Fila Montanha Russa Grande e Anda na Montanha Russa Grande
		Total_Clientes_Acutalmente_Na_Fila_MontanhaRussaG--;

		if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Andaram_MontanhaRussaG++;
		}

//Tratamento dos Tempos Maximos:
Tempo_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG=Hora_Andou_MontanhaRussaG-Hora_Entrada_Fila_MontanhaRussaG;
if(Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaG==0)
{
	Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaG=Tempo_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG;
}
else if(Tempo_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG>Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaG)
{
	Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaG=Tempo_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG;
}

//Variaveis Para Tempos Médios:
Soma_Tempos_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG=Soma_Tempos_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG+Tempo_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG;

		break;
	//---------------------------------------------------------------------------------------
		case 14: //Desiste Fila Montanha Russa Grande
		Total_Clientes_Acutalmente_Na_Fila_MontanhaRussaG--;

		if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaG++;
		}

//Tratamento dos Tempos Maximos:
Tempo_Espera_Fila_MontanhaRussaG_Antes_Desistir=Hora_Desistencia_Fila_MontanhaRussaG-Hora_Entrada_Fila_MontanhaRussaG;
if(Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaG==0)
{
	Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaG=Tempo_Espera_Fila_MontanhaRussaG_Antes_Desistir;
}
else if(Tempo_Espera_Fila_MontanhaRussaG_Antes_Desistir>Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaG)
{
	Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaG=Tempo_Espera_Fila_MontanhaRussaG_Antes_Desistir;
}

//Variaveis Para Tempos Médios:
Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaG=Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaG+Tempo_Espera_Fila_MontanhaRussaG_Antes_Desistir;

		break;
	//---------------------------------------------------------------------------------------
		case 15: //Entra Fila Montanha Russa Pequena
		Total_Clientes_Acutalmente_Na_Fila_MontanhaRussaP++;

		if(Tipo_Cliente==2) //Criancas
		{
			Total_Clientes_Criancas_Entraram_Fila_MontanhaRussaP++;
		}
		else if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaP++;
		}
		break;
	//---------------------------------------------------------------------------------------
		case 16: //Sai Fila Montanha Russa Pequena e Anda na Montanha Russa Pequena
		Total_Clientes_Acutalmente_Na_Fila_MontanhaRussaP--;

		if(Tipo_Cliente==2) //Criancas
		{
			Total_Clientes_Criancas_Andaram_MontanhaRussaP++;
		}
		else if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Andaram_MontanhaRussaP++;
		}

//Tratamento dos Tempos Maximos:
Tempo_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP=Hora_Andou_MontanhaRussaP-Hora_Entrada_Fila_MontanhaRussaP;
if(Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaP==0)
{
	Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaP=Tempo_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP;
}
else if(Tempo_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP>Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaP)
{
	Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaP=Tempo_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP;
}

//Variaveis Para Tempos Médios:
Soma_Tempos_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP=Soma_Tempos_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP+Tempo_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP;

		break;
	//---------------------------------------------------------------------------------------
		case 17: //Desiste Fila Montanha Russa Pequena
		Total_Clientes_Acutalmente_Na_Fila_MontanhaRussaP--;

		if(Tipo_Cliente==2) //Criancas
		{
			Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussaP++;
		}
		else if(Tipo_Cliente==3) //Publico Geral
		{
			Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaP++;
		}

//Tratamento dos Tempos Maximos:
Tempo_Espera_Fila_MontanhaRussaP_Antes_Desistir=Hora_Desistencia_Fila_MontanhaRussaP-Hora_Entrada_Fila_MontanhaRussaP;
if(Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaP==0)
{
	Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaP=Tempo_Espera_Fila_MontanhaRussaP_Antes_Desistir;
}
else if(Tempo_Espera_Fila_MontanhaRussaP_Antes_Desistir>Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaP)
{
	Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaP=Tempo_Espera_Fila_MontanhaRussaP_Antes_Desistir;
}

//Variaveis Para Tempos Médios:
Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaP=Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaP+Tempo_Espera_Fila_MontanhaRussaP_Antes_Desistir;

		break;
	//---------------------------------------------------------------------------------------
		case 18: //Sai Feira
		Total_Clientes_Acutalmente_Na_Feira--;
		
		Total_Clientes_Sairam_Feira++;

//Variaveis Para Tempos Medios:
Soma_Tempos_Estadia_Feira=Soma_Tempos_Estadia_Feira+(Hora_Saida_Feira - Hora_Entrada_Feira);



		break;
	//---------------------------------------------------------------------------------------
		case 50: //Fim Simulação

		printf("Final da Simulação");
		Simulacao_Concluida=true;
		Simulacao_Em_Curso=false;
		//pthread_exit(NULL); if (DEBUG)  printf("Fim da simulacao! "); 

		Escreve_Estatisticas(Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar, Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaG,Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaP, Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel, Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras, Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar, Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaG,Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaP, Tempo_Maximo_Espera_Antes_Entrar_Carroussel, Tempo_Maximo_Espera_Antes_Entrar_Feira, Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido, Soma_Tempos_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG,Soma_Tempos_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP, Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel, Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira, Soma_Tempos_Espera_Antes_Desistir_Fila_Bar, Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaG, Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaP, Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel, Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras, Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar, Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel, Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras, Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar, Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel, Total_Clientes_Mobilidade_Reduzida_Entraram_Feira, Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar, Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel, Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras, Total_Clientes_Criancas_Entraram_Fila_Bar, Total_Clientes_Criancas_Entraram_Fila_MontanhaRussaP, Total_Clientes_Criancas_Entraram_Fila_Carroussel, Total_Clientes_Criancas_Entraram_Filas_Bilheteiras, Total_Clientes_Criancas_Atendidos_Bar, Total_Clientes_Criancas_Andaram_MontanhaRussaP, Total_Clientes_Criancas_Entraram_Carroussel, Total_Clientes_Criancas_Entraram_Feira, Total_Clientes_Criancas_Desistiram_Fila_Bar, Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussaP, Total_Clientes_Criancas_Desistiram_Fila_Carroussel, Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras, Total_Clientes_Publico_Geral_Entraram_Fila_Bar, Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaG, Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaP, Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel, Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras, Total_Clientes_Publico_Geral_Atendidos_Bar, Total_Clientes_Publico_Geral_Andaram_MontanhaRussaG, Total_Clientes_Publico_Geral_Andaram_MontanhaRussaP, Total_Clientes_Publico_Geral_Entraram_Carroussel, Total_Clientes_Publico_Geral_Entraram_Feira, Total_Clientes_Publico_Geral_Desistiram_Fila_Bar, Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaG, Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaP, Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel, Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras, Soma_Tempos_Estadia_Feira, Total_Clientes_Sairam_Feira); 

		break;
	//---------------------------------------------------------------------------------------
		default: printf("Erro: Estado do Processo Errado. \n");
		break;


	}//Fim Switch

	//Adiciona ao Ficheiro Eventos o Novo Evento que Aconteceu
	Escreve_Ficheiro_Eventos(Estado, ID_Cliente, Hora_Actual); //Funcao no Envia_Para_Ficheiros.c

	if(Escolha_Menu==2)
	{
	//Escreve_Cabecalho(Escolha_Menu);

	Mostra_Estatisticas_Monitor(Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar, Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaG, Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaP, Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel, Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras, Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar, Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaG,Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaP, Tempo_Maximo_Espera_Antes_Entrar_Carroussel, Tempo_Maximo_Espera_Antes_Entrar_Feira, Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido, Soma_Tempos_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG, Soma_Tempos_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP, Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel, Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira, Soma_Tempos_Espera_Antes_Desistir_Fila_Bar, Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaG, Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaP, Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel, Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras, Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar, Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel, Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras, Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar, Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel, Total_Clientes_Mobilidade_Reduzida_Entraram_Feira, Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar, Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel, Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras, Total_Clientes_Criancas_Entraram_Fila_Bar, Total_Clientes_Criancas_Entraram_Fila_MontanhaRussaP, Total_Clientes_Criancas_Entraram_Fila_Carroussel, Total_Clientes_Criancas_Entraram_Filas_Bilheteiras, Total_Clientes_Criancas_Atendidos_Bar, Total_Clientes_Criancas_Andaram_MontanhaRussaP, Total_Clientes_Criancas_Entraram_Carroussel, Total_Clientes_Criancas_Entraram_Feira, Total_Clientes_Criancas_Desistiram_Fila_Bar, Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussaP, Total_Clientes_Criancas_Desistiram_Fila_Carroussel, Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras, Total_Clientes_Publico_Geral_Entraram_Fila_Bar, Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaG, Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaP, Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel, Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras, Total_Clientes_Publico_Geral_Atendidos_Bar, Total_Clientes_Publico_Geral_Andaram_MontanhaRussaG, Total_Clientes_Publico_Geral_Andaram_MontanhaRussaP, Total_Clientes_Publico_Geral_Entraram_Carroussel, Total_Clientes_Publico_Geral_Entraram_Feira, Total_Clientes_Publico_Geral_Desistiram_Fila_Bar, Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaG, Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaP, Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel, Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras, Soma_Tempos_Estadia_Feira, Total_Clientes_Sairam_Feira); 
	}
	else if(Escolha_Menu==3) //Isto é, Se o Menu Actual For o 3 (Mostrar Eventos em Tempo Real), Sempre Que Houver um Evento, Mostra no Ecra
	{
//Escreve_Cabecalho(Escolha_Menu);
		Mostra_No_Monitor_Eventos_Tempo_Real(Estado, ID_Cliente, Hora_Actual); //Funcao no Funcoes_Auxiliares_Monitor.c
	}
	else if(Escolha_Menu==4) //Isto é, Se o Menu Actual For o 4 (Mostrar Zonas em Tempo Real), Sempre Que Houver uma Alteracao nas Zonas, Vai Ser Mostrada no Ecra
	{
//Escreve_Cabecalho(Escolha_Menu);
		//Mostra_Zonas_Tempo_Real(Tamanho_Maximo_Bar, Tamanho_Maximo_Piscina, Tamanho_Maximo_Parque, Total_Clientes_Acutalmente_No_Bar, Total_Clientes_Acutalmente_Na_Piscina, Total_Clientes_Acutalmente_No_Parque);
Mostra_Zonas_Tempo_Real(50, 100, 500, 100, 100, Total_Clientes_Acutalmente_No_Bar, Total_Clientes_Acutalmente_No_Carroussel, Total_Clientes_Acutalmente_Na_Feira, Total_Clientes_Actualmente_Na_MontanhaRussaG, Total_Clientes_Actualmente_Na_MontanhaRussaP);
			//Funcao no Estatisticas_Tempo_Real
	}

}

//Funcao que Mostra Informacao do Grupo
void Mostra_Informacao_Grupo()
{
	printf("                   Grupo 17 - Sistemas Operativos \n");
	printf("-Eurico Teixeira  \n");
	printf("Nº 22083014 \n");
	printf(" \n");
	printf("Paulo Gil Rodrigues de Freitas \n");
	printf("Nº 2012413 \n");
	printf(" \n");
	printf("Josué Ferreira \n");
	printf("Nº 2066513 \n");
	printf(" \n");

	//Volta_Menu(); //Funcao Para Voltar ao Menu Principal
}


// Cria Cabeçalho do Interface do Monitor
void Escreve_Cabecalho(int Escolha_Menu)
{
//system("clear");
printf("        ┌──────────────────────────────────────────────────────────────┐\n");
printf("        │   Sistemas Operativos 2017/2018 ─ Simulacao Montanha Russa   │\n");
printf("        └──────────────────────────────────────────────────────────────┘\n");
printf("┌──────────────────────┌────────────────────────────────────────────────────────────┐\n");
printf("│                      │  ■ 1-Inicio Simulacao    ■ 2-Estatisticas Tempo Real       │\n");

switch(Escolha_Menu)
{
	case 0: printf("│      Menu Principal       │  ■ 3-Eventos Tempo Real  ■ 4-Estado Zonas Tempo Real       │"); break;
	case 1: printf("│     Inicio Simulacao      │  ■ 3-Eventos Tempo Real  ■ 4-Estado Zonas Tempo Real       │\n"); break;
	case 2: printf("│  Estatisticas Tempo Real  │  ■ 3-Eventos Tempo Real  ■ 4-Estado Zonas Tempo Real       │\n"); break;		
	case 3: printf("│     Eventos Tempo Real    │  ■ 3-Eventos Tempo Real  ■ 4-Estado Zonas Tempo Real       │\n"); break;
	case 4: printf("│  Estado Zonas Tempo Real  │  ■ 3-Eventos Tempo Real  ■ 4-Estado Zonas Tempo Real       │\n"); break;
	case 5: printf("│           Sobre           │  ■ 3-Eventos Tempo Real  ■ 4-Estado Zonas Tempo Real       │\n"); break;
	case 6: printf("│           Ajuda           │  ■ 3-Eventos Tempo Real  ■ 4-Estado Zonas Tempo Real       │\n"); break;
	case 7: printf("│            Sair           │  ■ 3-Eventos Tempo Real  ■ 4-Estado Zonas Tempo Real       │\n"); break;
	default: printf("│   Erro: Opcao Errada     │  ■ 3-Eventos Tempo Real  ■ 4-Estado Zonas Tempo Real       │\n"); break;
}

printf("│                │  ■ 5-Sobre        ■ 6-Ajuda          ■ 7-Sair   │\n");
printf("└────────────────┘─────────────────────────────────────────────────┘\n");
}


//Menu Principal:
void Menu_Principal()
{
	Escreve_Cabecalho(Escolha_Menu);
	printf("$");
	scanf("%d",&Escolha_Menu);

	switch(Escolha_Menu)
	{
		case 1: //Iniciar Simulacao
		if(Simulacao_Em_Curso==false) //Se Nao Houver Nenhuma Simulacao Activa, Vai Comecar Uma Simulacao Nova
		{
			//Escreve_Cabecalho(Escolha_Menu);
			Simulacao_Em_Curso=true;
			Variavel_Global_Comeca_Simulacao=1;
			//Codigo Para Comecar a Simulacao

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
			Escreve_Cabecalho(Escolha_Menu);

			//Funcao Para Mostrar as Estatisticas da Feira no Monitor:
			Mostra_Estatisticas_Monitor (Tempo_Maximo_Espera_Antes_Desistir_Fila_Bar, Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaG,Tempo_Maximo_Espera_Antes_Desistir_Fila_MontanhaRussaP, Tempo_Maximo_Espera_Antes_Desistir_Fila_Carroussel, Tempo_Maximo_Espera_Antes_Desistir_Filas_Bilheteiras, Tempo_Maximo_Espera_Antes_Ser_Atendido_Bar, Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaG,Tempo_Maximo_Espera_Antes_Andar_MontanhaRussaP, Tempo_Maximo_Espera_Antes_Entrar_Carroussel, Tempo_Maximo_Espera_Antes_Entrar_Feira, Soma_Tempos_Espera_Fila_Bar_Antes_Ser_Atendido, Soma_Tempos_Espera_Fila_MontanhaRussaG_Antes_Andar_MontanhaRussaG,Soma_Tempos_Espera_Fila_MontanhaRussaP_Antes_Andar_MontanhaRussaP, Soma_Tempos_Espera_Fila_Carroussel_Antes_Entrar_Carroussel, Soma_Tempos_Espera_Filas_Bilheteiras_Antes_Entrar_Feira, Soma_Tempos_Espera_Antes_Desistir_Fila_Bar, Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaG, Soma_Tempos_Espera_Antes_Desistir_Fila_MontanhaRussaP, Soma_Tempos_Espera_Antes_Desistir_Fila_Carroussel, Soma_Tempos_Espera_Antes_Desistir_Filas_Bilheteiras, Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Bar, Total_Clientes_Mobilidade_Reduzida_Entraram_Fila_Carroussel, Total_Clientes_Mobilidade_Reduzida_Entraram_Filas_Bilheteiras, Total_Clientes_Mobilidade_Reduzida_Atendidos_Bar, Total_Clientes_Mobilidade_Reduzida_Entraram_Carroussel, Total_Clientes_Mobilidade_Reduzida_Entraram_Feira, Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Bar, Total_Clientes_Mobilidade_Reduzida_Desistiram_Fila_Carroussel, Total_Clientes_Mobilidade_Reduzida_Desistiram_Filas_Bilheteiras, Total_Clientes_Criancas_Entraram_Fila_Bar, Total_Clientes_Criancas_Entraram_Fila_MontanhaRussaP, Total_Clientes_Criancas_Entraram_Fila_Carroussel, Total_Clientes_Criancas_Entraram_Filas_Bilheteiras, Total_Clientes_Criancas_Atendidos_Bar, Total_Clientes_Criancas_Andaram_MontanhaRussaP, Total_Clientes_Criancas_Entraram_Carroussel, Total_Clientes_Criancas_Entraram_Feira, Total_Clientes_Criancas_Desistiram_Fila_Bar, Total_Clientes_Criancas_Desistiram_Fila_MontanhaRussaP, Total_Clientes_Criancas_Desistiram_Fila_Carroussel, Total_Clientes_Criancas_Desistiram_Filas_Bilheteiras, Total_Clientes_Publico_Geral_Entraram_Fila_Bar, Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaG, Total_Clientes_Publico_Geral_Entraram_Fila_MontanhaRussaP, Total_Clientes_Publico_Geral_Entraram_Fila_Carroussel, Total_Clientes_Publico_Geral_Entraram_Filas_Bilheteiras, Total_Clientes_Publico_Geral_Atendidos_Bar, Total_Clientes_Publico_Geral_Andaram_MontanhaRussaG, Total_Clientes_Publico_Geral_Andaram_MontanhaRussaP, Total_Clientes_Publico_Geral_Entraram_Carroussel, Total_Clientes_Publico_Geral_Entraram_Feira, Total_Clientes_Publico_Geral_Desistiram_Fila_Bar, Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaG, Total_Clientes_Publico_Geral_Desistiram_Fila_MontanhaRussaP, Total_Clientes_Publico_Geral_Desistiram_Fila_Carroussel, Total_Clientes_Publico_Geral_Desistiram_Filas_Bilheteiras, Soma_Tempos_Estadia_Feira, Total_Clientes_Sairam_Feira); 
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
			//Escreve_Cabecalho(Escolha_Menu);

			//Mostra Eventos Que Vao Acontecendo no Ecra
			printf("Teste");
			//Mostra_No_Monitor_Eventos_Tempo_Real(Evento, ID_Cliente, Hora_Actual);
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
			Escreve_Cabecalho(Escolha_Menu);

			//Mostra Ocupacao das Zonas em Tempo Real
			//Mostra_Zonas_Tempo_Real(Tamanho_Maximo_Bar,Tamanho_Maximo_Piscina,Tamanho_Maximo_Parque,Total_Clientes_Acutalmente_No_Bar,Total_Clientes_Acutalmente_Na_Piscina,Total_Clientes_Acutalmente_No_Parque);
			//Funcao no Estatisticas_Tempo_Real
		}
		else
		{
			printf("Erro: Nenhuma Simulacao Activa Neste Momento \n");
		}
		break;
//---------------------------------------------------------------------------------------
		case 5: //Sobre

		Escreve_Cabecalho(Escolha_Menu);

		Mostra_Informacao_Grupo();
		break;
//---------------------------------------------------------------------------------------
		case 6: //Ajuda

		Escreve_Cabecalho(Escolha_Menu);

		Mostra_Funcionamento();
		break;		
//---------------------------------------------------------------------------------------
		case 7: //Fechar

		Variavel_Global_Sai_Aplicacao=1;

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




/*int main(int argc, char *argv[])
{
	Prepara_Ficheiro_Eventos(); //Funcao no Envia_Para_Ficheiros.c

	//Faz Ligacao à Socket Quando Simulador é Iniciado
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
		perror("Erro ao fazer connect no cliente\n");
	}

	while(Escolha_Menu!=7)
	{
		Menu_Principal();
	}


	return 0;
}*/











