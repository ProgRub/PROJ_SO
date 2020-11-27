// Grupo constituído por:
// 2046018 Rúben José Gouveia Rodrigues
// 2045218 Filipe Orlando Namora Gomes
// 2028616 José Alejandro Ferreira Gouveia

#include "config.h"

int socketfd;
int fimSimulacao = FALSE;

void leituraSocket(int sockfd)
{
    socketfd = sockfd;
    int numero = 0;
    char buffer[TamLinha];
    while (!fimSimulacao)
    {                                              //Enquanto a simulacao nao acabar
        numero = read(socketfd, buffer, TamLinha); //Le a mensagem do socket e guarda no buffer
        if (numero == 0)
        { //Quando chega ao fim
            break;
        }
        else if (numero < 0)
        {
            printf("Erro: Nao leu do socket \n");
        }
        else
        {
            trataMensagem(buffer);
        }
    }
}

void trataMensagem(char mensagem[])
{
}

void criaServidor()
{
    //sockfd -> criacao para a primeira comunicacao
    //novoSocket -> criacao para a segunda comunicacao
    int sockfd, novoSocket, tamanhoCliente, tamanhoServidor;
    struct sockaddr_un end_cli, serv_addr;

    //Verifica a criacao do socket
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        printf("Erro ao criar o Socket\n");
    }

    //Incializa os valores do buffer a zero
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, UNIXSTR_PATH);
    tamanhoServidor = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
    unlink(UNIXSTR_PATH);

    //Liga o socket a um endereco
    if (bind(sockfd, (struct sockaddr *)&serv_addr, tamanhoServidor) < 0)
    {
        printf("Erro a ligar o socket a um endereco\n");
    }

    //Espera a conexao com o simulador
    printf("Esperando pelo simulador...\n");
    listen(sockfd, 1);

    //Criacao de um novo scoket
    tamanhoCliente = sizeof(end_cli);
    novoSocket = accept(sockfd, (struct sockaddr *)&end_cli, &tamanhoCliente);
    // printf("%d\n",novoSocket);
    if (novoSocket < 0)
    {
        printf("Erro na aceitacao \n");
    } //Verifica o erro na aceitacao da ligacao

    //Criacao de um novo processo
    int pid;
    if ((pid = fork()) < 0)
    {
        printf("Erro ao criar o processo filho \n"); //Erro na criacao do processo filho
    }
    else if (pid == 0)
    { //Processo filho irá tratar das sucessivas leituras e fecha o socket do processo pai
        close(sockfd);
        leituraSocket(novoSocket);
    }
    close(novoSocket);
}

int main(int argc, char *argv[])
{
    printf("########### Bem vindo ########### \n");
    printf("1: Comecar simulacao          \n");
    // printf ( "2: Limpar ficheiros da simulacao \n" );
    printf("################################# \n");
    int opcao = 0;
    while (!fimSimulacao)
    {
        while (opcao != 1)
        {
            printf("Opcao: ");
            scanf("%d", &opcao); //Le valor introduzido
            if (opcao != 1)
            {
                printf("Isso nao e uma simulacao.\n");
                opcao = 0;
            }
        }
        criaServidor();
    }
    return 0;
    // criaServidor();
}

void escreveEmFicheiro(char *mensagem)
{
    FILE *registoFile;
    registoFile = fopen("Relatorio.txt", "w");
    // char *mensagem="Estado atual => Simulacao a decorrer!\nUtilizadores: 203\nCasos positivos: 18\nCasos em estudo: 11\nTempo medio de espera: 13 min.";
    fprintf(registoFile, "%s", mensagem);
    fclose(registoFile);
}