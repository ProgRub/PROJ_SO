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
    int tempoSimulacao;
};

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        struct Configuration config;
        FILE *configFile;
        configFile = fopen(argv[1], "r");
        if (configFile == NULL)
        {
            perror("ERRO!\nFicheiro de configuração não existe.");
            exit(EXIT_FAILURE);
        }
        else if(strcmp(argv[1],"simulador.conf")!=0)
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
        // printf("%s\n\n\n", buffer);
        int i = 0;
        char *p = strtok(buffer, "\n");
        char *lines[25];
        while (p != NULL)
        {
            lines[i++] = p;
            p = strtok(NULL, "\n");
        }
        char *end;
        // printf("%d",(int)strtol(array[0],&end,10));
        char *array[2];
        char *values[10];
        for (int index = 0; index < 10; index++)
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
        config.tempoMedioChegada = (int)strtol(values[0], &end, 10);
        config.tempoTesteNormal = strtol(values[1], &end, 10);
        config.tempoTesteRapido = strtol(values[2], &end, 10);
        config.tempoEsperaCentro1 = strtol(values[3], &end, 10);
        config.tempoEsperaCentro2 = strtol(values[4], &end, 10);
        config.tamanhoFilaCentro1 = strtol(values[5], &end, 10);
        config.tamanhoFilaCentro2 = strtol(values[6], &end, 10);
        config.tamanhoHospital = strtol(values[7], &end, 10);
        config.probabilidadePositivo = strtof(values[8], &end);
        config.tempoSimulacao = strtol(values[9], &end, 10);
        // printf("%d\n", config.tempoMedioChegada);
        // printf("%d\n", config.tempoTesteNormal);
        // printf("%d\n", config.tempoTesteRapido);
        // printf("%d\n", config.tempoEsperaCentro1);
        // printf("%d\n", config.tempoEsperaCentro2);
        // printf("%d\n", config.tamanhoFilaCentro1);
        // printf("%d\n", config.tamanhoFilaCentro2);
        // printf("%d\n", config.tamanhoHospital);
        // printf("%f\n", config.probabilidadePositivo);
        // printf("%d\n", config.tempoSimulacao);
        // printf("%s\n", array[i]);
        fclose(configFile);
        return 0;
    }
    perror("ERRO!\n(Só) É preciso passar como argumento o ficheiro de configuração.");
    exit(EXIT_FAILURE);
}