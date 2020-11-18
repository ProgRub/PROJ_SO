//o monitor escreve no ficheiro resultados.txt
#include "util.h"

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

int main(){
	escreve();
	printf("O ficheiro resultados.txt foi criado!\n");
	
	return 0;
}
