// Grupo constituído por:
// 2046018 Rúben José Gouveia Rodrigues
// 2045218 Filipe Orlando Namora Gomes
// 2028616 José Alejandro Ferreira Gouveia

#include "config.h"

int sockfd = 0;


int main(int argc, char *argv[])
{
    sockfd = criaSocket();
}

int criaSocket(){
    //Variaveis
    struct sockaddr_un end_serv;
    int tamanhoServidor;

    //Cria o socket
    int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);                                          
    if(sockfd < 0) {                                                                         
        printf("Erro: socket nao foi criado \n");
    }

    //Colocar o socket a zero
    bzero((char *) &end_serv, sizeof(end_serv));
    
    //Dominio do socket
    end_serv.sun_family = AF_UNIX;
    strcpy(end_serv.sun_path, UNIXSTR_PATH);
    tamanhoServidor = strlen(end_serv.sun_path) + sizeof( end_serv.sun_family);

    // Estabelece a ligacao com o socket
    int varprint = 0;
    while( connect( sockfd, (struct sockaddr *) &end_serv, tamanhoServidor) < 0) {
        if (varprint==0){
            printf("Espera pelo monitor.\n"); 
            varprint = 1;
        }
    }
    printf("Simulador pronto.\n"); 
    return sockfd;
}

/*
void leitura(argc, __argv){

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
        configuracao.tempoMedioChegada = (int)strtol(values[0], NULL, 10);
        configuracao.tempoTesteNormal = strtol(values[1], NULL, 10);
        configuracao.tempoTesteRapido = strtol(values[2], NULL, 10);
        configuracao.tempoEsperaCentro1 = strtol(values[3], NULL, 10);
        configuracao.tempoEsperaCentro2 = strtol(values[4], NULL, 10);
        configuracao.tamanhoFilaCentro1 = strtol(values[5], NULL, 10);
        configuracao.tamanhoFilaCentro2 = strtol(values[6], NULL, 10);
        configuracao.tamanhoHospital = strtol(values[7], NULL, 10);
        configuracao.probabilidadePositivo = strtof(values[8], NULL);
        configuracao.probabilidadeTesteNormalFalsoPositivo = strtof(values[9], NULL);
        configuracao.probabilidadeTesteRapidoFalsoPositivo = strtof(values[10], NULL);
        configuracao.tempoSimulacao = strtol(values[11], NULL, 10);

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
*/
