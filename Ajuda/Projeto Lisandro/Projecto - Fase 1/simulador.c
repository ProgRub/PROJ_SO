// 1ª Fase deverá conseguir carregar o ficheiro de configuração "configuracao_simulacao"

// Declaração das Variáveis Globais
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include <unistd.h>

int num_cliente, tempo_medio, tempo_simulacao, capacidade_fila, probalidade_desistencia, tempo_serv_prod;

// Função Responsável por ler o Ficheiro de Configuração

void leitura()
{
	FILE* config;

	config = fopen("configuracao_simulacao.txt", "r");

	if(config != NULL){
	
		char linha[50];+
		int valor;    
		char lixo[50];

		while(fgets(linha, sizeof(linha), config) != NULL){
			//valor = strtok(linha, ":");
			sscanf(linha, "%s : %d",lixo, &valor);


			if(strcmp(lixo, "tempo_medio") == 0){
				tempo_medio = valor;
			}
			if(strcmp(lixo, "temp_sim") == 0) {
				
				tempo_simulacao = valor;
			}
			if(strcmp(lixo, "tempo_serv_prod") == 0) {
				tempo_serv_prod = valor;
			}
			if(strcmp(lixo, "CAPACIDADE_FILA") == 0) {
				
				capacidade_fila = valor;
			}
	
			if(strcmp(lixo, "prob_des") == 0) {
				
				probalidade_desistencia = valor;
			}
			
		}
	} else {
		printf("Erro ao Abrir o Ficheiro. Tente Novamente.\n");
	}
	fclose(config);
	
}

int main()
{
	leitura();
	printf("Tempo de Medio de Espera: %d\n",tempo_medio);
	printf("Tempo de Servico em Cada Produto: %d\n",tempo_serv_prod);
	printf("Probablidade de Desistencia: %d\n",probalidade_desistencia);	
	printf("Capacidade da Fila: %d\n",capacidade_fila);
	printf("Tempo de Simulação: %d\n",tempo_simulacao);

	return 0;
}
