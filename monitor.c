// Grupo constituído por:
// 2046018 Rúben José Gouveia Rodrigues
// 2045218 Filipe Orlando Namora Gomes
// 2028616 José Alejandro Ferreira Gouveia

#include "config.h"

int sockfd;


void criaServidor()
{
    //sockfd -> criacao para a primeira comunicacao
    //novoSocket -> criacao para a segunda comunicacao
    //tamCliente -> guarda o tamanho do endereco do cliente
    //tamanhoServidor -> guarda o tamanho do servidor
    int sockfd, novoSocket, tamCliente, tamanhoServidor;
    struct sockaddr_un end_cli , serv_addr;             
	
    //Verifica a criacao do socket
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0 ){
        printf ( "Erro ao criar o Socket \n");
    }
    
    //Incializa os valores do buffer a zero
    bzero((char*) & serv_addr , sizeof(serv_addr));   
    serv_addr.sun_family = AF_UNIX;                       
    strcpy(serv_addr.sun_path, UNIXSTR_PATH);
    tamanhoServidor = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);    
    unlink(UNIXSTR_PATH);

    //Liga o socket a um endereco
    if( bind( sockfd, (struct sockaddr *) & serv_addr, tamanhoServidor) < 0 ){
        printf("Erro a ligar o socket a um endereco\n");
    }

    //Espera a conexao com o simulador
    printf("Esperando pelo simulador: \n");
    listen( sockfd , 1 );

    //Criacao de um novo scoket
    tamCliente = sizeof(end_cli);
    novoSocket = accept (sockfd, (struct sockaddr * ) &end_cli, &tamCliente);
    if (novoSocket < 0) {                                        //Verifica o erro na aceitacao da ligacao
        printf ( "Erro na aceitacao \n" );
    }
    /*
    //Criacao de um novo processo
    int pid;
    if ((pid = fork()) < 0 ) {
        printf ( "Erro ao criar o processo filho \n" );         //Erro na criacao do processo filho
    }else if (pid == 0) {                                       //Processo filho irá tratar das sucessivas leituras e fecha o socket do processo pai
        close(sockfd);
        LeituraSucessiva(novoSocket);
    }*/
    close(novoSocket);



}

int main(int argc, char *argv[])
{
    criaServidor();
}

void escreve()
{
    FILE *registoFile;
    registoFile=fopen("Relatorio.txt","w");
	char *mensagem="Estado atual => Simulacao a decorrer!\nUtilizadores: 203\nCasos positivos: 18\nCasos em estudo: 11\nTempo medio de espera: 13 min.";
	fprintf(registoFile,"%s",mensagem);
    fclose(registoFile);
}