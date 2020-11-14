#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

struct Configuration
{
    int tempoMedioChegada;
    int tempoTesteNormal;
    int tempoTesteRapido;
    int tempoEsperaCentro1;
    int tempoEsperaCentro2;
    int tamanhoFilaCentro1;
    int tamanhoFilaCentro2;
    int tamanhoHospital;
    float probabilidadePositivo;
    float probabilidadeTesteNormalFalsoPositivo;
    float probabilidadeTesteRapidoFalsoPositivo;
    int tempoSimulacao;
};

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        struct Configuration configuracao;
        FILE *configFile;
        configFile = fopen(argv[1], "r");
        if (configFile == NULL)
        {
            perror("ERRO!\nFicheiro de configuração não existe.");
            exit(EXIT_FAILURE);
        }
        else if(strcmp(argv[1],"simulador.conf")!=0) //se strcmp retorn 0, então os string são iguais
        {
            perror("ERRO!\nNão é o ficheiro de configuração.");
            exit(EXIT_FAILURE);
        }
        // obtain file size:
        fseek(configFile, 0, SEEK_END);
        long fileSize = ftell(configFile);
        rewind(configFile);
        char buffer[fileSize];
        fread(buffer, 1, fileSize, configFile);
        fclose(configFile);

        int i = 0;
        char *p = strtok(buffer, "\n");
        char *lines[25];
        while (p != NULL)
        {
            lines[i++] = p;
            p = strtok(NULL, "\n");
        }
        char *end;
        char *array[2];
        char *values[12];
        for (int index = 0; index < 12; index++)
        {
            char *aux = strtok(lines[index], ":");
            i = 0;
            while (aux != NULL)
            {
                array[i++] = aux;
                aux = strtok(NULL, ":");
            }
            values[index]=array[1];
        }

        //guardar os valores do ficheiro na estrutura de configuracao
        configuracao.tempoMedioChegada = (int)strtol(values[0], &end, 10);
        configuracao.tempoTesteNormal = strtol(values[1], &end, 10);
        configuracao.tempoTesteRapido = strtol(values[2], &end, 10);
        configuracao.tempoEsperaCentro1 = strtol(values[3], &end, 10);
        configuracao.tempoEsperaCentro2 = strtol(values[4], &end, 10);
        configuracao.tamanhoFilaCentro1 = strtol(values[5], &end, 10);
        configuracao.tamanhoFilaCentro2 = strtol(values[6], &end, 10);
        configuracao.tamanhoHospital = strtol(values[7], &end, 10);
        configuracao.probabilidadePositivo = strtof(values[8], &end);
        configuracao.probabilidadeTesteNormalFalsoPositivo = strtof(values[9], &end);
        configuracao.probabilidadeTesteRapidoFalsoPositivo = strtof(values[10], &end);
        configuracao.tempoSimulacao = strtol(values[11], &end, 10);

        printf("Tempo medio de chegada: %d\n", configuracao.tempoMedioChegada);
        printf("Tempo ate o resultado do teste normal: %d\n", configuracao.tempoTesteNormal);
        printf("Tempo ate o resultado do teste rapido: %d\n", configuracao.tempoTesteRapido);
        printf("Tempo de espera no centro de testes 1: %d\n", configuracao.tempoEsperaCentro1);
        printf("Tempo de espera no centro de testes 2: %d\n", configuracao.tempoEsperaCentro2);
        printf("Tamanho maximo da fila no centro de testes 1: %d\n", configuracao.tamanhoFilaCentro1);
        printf("Tamanho maximo da fila no centro de testes 2: %d\n", configuracao.tamanhoFilaCentro2);
        printf("Tamanho maximo do hospital: %d\n", configuracao.tamanhoHospital);
        printf("Probabilidade de um teste regressar positivo: %f\n", configuracao.probabilidadePositivo);
        printf("Probabilidade do teste normal dar falso positivo: %f\n", configuracao.probabilidadeTesteNormalFalsoPositivo);
        printf("Probabilidade do teste rapido dar falso positivo: %f\n", configuracao.probabilidadeTesteRapidoFalsoPositivo);
        printf("Duracao da simulacao: %d\n", configuracao.tempoSimulacao);
        return 0;
    }
    perror("ERRO!\n(Só) É preciso passar como argumento o ficheiro de configuração.");
    exit(EXIT_FAILURE);
}