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

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <sys/un.h>

// #define UNIXSTR_PATH "/tmp/2046018"
// #define UNIXDG_PATH  "/tmp/s.unixdfddffdfdgx"
// #define UNIXDG_TMP   "/tmp/dgXXXXXfdjhfdjhfdXX"


// /* Cliente do tipo socket stream.
//    L▒ linhas do teclado e envia-as para o servidor */

// #include "unix.h"

// main(void)
// {
//         int sockfd, servlen;
//         struct sockaddr_un serv_addr;

//         /* Cria socket stream */

//         if ((sockfd= socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
//                 err_dump("client: can't open stream socket");

//         /* Primeiro uma limpeza preventiva!
//            Dados para o socket stream: tipo + nome do ficheiro.
//          O ficheiro identifica o servidor */

//         bzero((char *) &serv_addr, sizeof(serv_addr));
//         serv_addr.sun_family = AF_UNIX;
//         strcpy(serv_addr.sun_path, UNIXSTR_PATH);
//         servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

//         /* Tenta estabelecer uma liga▒▒o. S^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^$
//          sido lan▒ado primeiro (o servidor tem de criar o ficheiro e associar
//          o socket ao ficheiro) */

//         if (connect(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0)
//                 err_dump("client: can't connect to server");

//         /* Envia as linhas lidas do teclado para o socket */

//         str_cli(stdin, sockfd);

//         /* Fecha o socket e termina */

//         close(sockfd);
//         exit(0);
// }


/* Servidor do tipo socket stream.
   Recebe linhas do cliente e reenvia-as para o cliente */

// #include "unix.h"

// main(void)
// {
//         int sockfd, newsockfd, clilen, childpid, servlen;
//         struct sockaddr_un cli_addr, serv_addr;

//         /* Cria socket stream */

//         if ((sockfd = socket(AF_UNIX,SOCK_STREAM,0)) < 0)
//                 err_dump("server: can't open stream socket");

//         /* Primeiro uma limpeza preventiva!
//            Dados para o socket stream: tipo + nome do ficheiro.
//          O ficheiro serve para que os clientes possam identificar o servidor */

//         bzero((char *)&serv_addr, sizeof(serv_addr));
//         serv_addr.sun_family = AF_UNIX;
//         strcpy(serv_addr.sun_path, UNIXSTR_PATH);

//       /* O servidor ▒ quem cria o ficheiro que identifica o socket.
//          Elimina o ficheiro, para o caso de algo ter ficado pendurado.
//          Em seguida associa o socket ao ficheiro.
//          A dimens▒o a indicar ao bind n▒o ▒ a da estrutura, pois depende
//          do nome do ficheiro */

//         servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
//         unlink(UNIXSTR_PATH);
//         if (bind(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0)
//                 err_dump("server, can't bind local address");

//         /* Servidor pronto a aceitar 5 clientes para o socket stream */

//         listen(sockfd, 5);

//         for (;;) {

//                 /* N▒o esquecer que quando o servidor aceita um cliente cria um
//                    socket para comunicar com ele. O primeiro socket (sockfd) fica
//                    ▒ espera de mais clientes */

//                 clilen = sizeof(cli_addr);
//                 newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
//                                    &clilen);
//                 if (newsockfd < 0)
//                         err_dump("server: accept error");

//                 /* Lan▒a processo filho para lidar com o cliente */

//                 if ((childpid = fork()) < 0)
//                         err_dump("server: fork error");

//                 else if (childpid == 0) {

//                         /* Processo filho que vai atender o cliente.
//                            Fechar sockfd ▒ sanit▒rio, j▒ que n▒o ▒
//                            utilizado pelo processo filho.
//                            Os dados recebidos do cliente s▒o reenviados
//                            para o cliente */

//                         close(sockfd);
//                         str_echo(newsockfd);
//                         exit(0);
//                 }

//                 /* Processo pai.
//                    Fechar newsockfd ▒ sanit▒rio, j▒ que n▒o ▒
//                    utilizado pelo processo pai */

//                 close(newsockfd);
//         }
// }


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