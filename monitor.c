// Grupo constituído por:
// 2046018 Rúben José Gouveia Rodrigues
// 2045218 Filipe Orlando Namora Gomes
// 2028616 José Alejandro Ferreira Gouveia

#include "config.h"

int main(int argc, char *argv[])
{
    FILE *registoFile;
    registoFile=fopen("Relatorio.txt","w");
	char *mensagem="Estado atual => Simulacao a decorrer!\nUtilizadores: 203\nCasos positivos: 18\nCasos em estudo: 11\nTempo medio de espera: 13 min.";
	fprintf(registoFile,"%s",mensagem);
    fclose(registoFile);

	return 0;
}