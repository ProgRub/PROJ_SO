#include "config.h"

//Variavel global que indica o estado da simulacao
int fimSimulacao = 0;

struct metro metros[2];
struct bilheteira bilheteiras[5];
struct estacao estacoes[5];

bool metroEstaViagem [2]; //0-> metro zero; 1-> metro 1
int numeroPessoas = 0;


/*###################################################*/
/*          Escreve no ficheiro e no monitor         */
/*###################################################*/
void escreverMonitorFicheiro(char textoFicheiro []){
    FILE *fp; // Cria um apontador para um ficheiro
    fp = fopen ("monitor.txt", "a");  // Abre o ficheiro em modo de acrescentar texto

    if (fp == NULL) { // Verifica se o ficheiro existe
        printf("Não foi possível abrir o ficheiro %s.", "monitor.txt");
    }else{
        fprintf(fp, "%s", textoFicheiro); // Escreve no ficheiro o texto
        printf(textoFicheiro);
        fclose(fp); // Encerra a conexão com o ficheiro
    }
}


void imprimeInfo(){
    char textoEscrever[500];
    //Metros
    escreverMonitorFicheiro("*********************************\n");
    escreverMonitorFicheiro("     * * * M E T R O S * * *     \n");
    escreverMonitorFicheiro("*********************************\n");
    for(int e = 0; e<2; e++){
        sprintf(textoEscrever, "Metro: %d\n", e);
        escreverMonitorFicheiro(textoEscrever);
        sprintf(textoEscrever, "Número de Passageiros atual: %i\n",metros[e].numeroPassageirosAtual);
        escreverMonitorFicheiro(textoEscrever);
        if (metroEstaViagem[e] ){
            escreverMonitorFicheiro("Estação atual: -\n");
        }else{
            sprintf(textoEscrever, "Estação atual: %i\n" ,metros[e].idEstacaoAtual);
            escreverMonitorFicheiro(textoEscrever);
        }
        sprintf(textoEscrever, "Está em viagem: %i \n", metroEstaViagem[e]);
        escreverMonitorFicheiro(textoEscrever);
        escreverMonitorFicheiro("\n");
    }

    //estacoes
   /* escreverMonitorFicheiro("***************************\n");
    escreverMonitorFicheiro(" * * * E S T A Ç Õ E S * * *\n");
    escreverMonitorFicheiro("***************************\n");
    for(int e = 0; e<5; e++){
        escreverMonitorFicheiro("Estação: %d\n", e);
        escreverMonitorFicheiro("Pessoas à Espera: %i\n",estacoes[e].numeroPassageirosEspera);
        escreverMonitorFicheiro("Pessoas que entraram na Estação: %i\n",estacoes[e].numeroPassageirosEntraramEstacao);
        escreverMonitorFicheiro("Pessoas que sairam na Estação: %i\n",estacoes[e].numeroPssageirosSairamComboio);
        escreverMonitorFicheiro("Pessoas que chegaram ao seu Destino: %i\n",estacoes[e].numeroPssageirosSairamEstacao);
        escreverMonitorFicheiro("Pessoas que desistiram de realizar a viagem: %i\n",estacoes[e].numeroPassageirosDesistiram);
        escreverMonitorFicheiro("\n");
    }*/

    //Bilheteiras
	escreverMonitorFicheiro("*********************************\n");
	escreverMonitorFicheiro(" * * * B I L H E T E I R A  * * *\n");
    escreverMonitorFicheiro("*********************************\n");
    for(int e = 0; e<2; e++){
        sprintf(textoEscrever, "Bilheteira da Estação %i\n",e);
        escreverMonitorFicheiro(textoEscrever);
        sprintf(textoEscrever, "Pessoas normais na fila: %i\n",bilheteiras[e].pessoasNormalFila);
        escreverMonitorFicheiro(textoEscrever);
        sprintf(textoEscrever, "Pessoas VIP na fila: %i\n", bilheteiras[e].pessoasVIPFila);
        escreverMonitorFicheiro(textoEscrever);
        sprintf(textoEscrever, "Tempo médio de espera: %f\n",bilheteiras[e].tempoMedioEspera);
        escreverMonitorFicheiro(textoEscrever);
        sprintf(textoEscrever, "Tempo médio de compra: %f\n",bilheteiras[e].tempoMedioCompra);
        escreverMonitorFicheiro(textoEscrever);
        sprintf(textoEscrever, "Pessoas VIP que desistiram : %i\n",bilheteiras[e].pessoasDesistentesVIP);
        escreverMonitorFicheiro(textoEscrever);
        sprintf(textoEscrever, "Pessoas Normal que desistiram : %i\n",bilheteiras[e].pessoasDesistentesNormal);
        escreverMonitorFicheiro(textoEscrever);
        sprintf(textoEscrever, "Pessoas VIP atendidas : %i\n",bilheteiras[e].pessoasAtendidasVIP);
        escreverMonitorFicheiro(textoEscrever);
        sprintf(textoEscrever, "Pessoas Normal atendidas : %i\n",bilheteiras[e].pessoasAtendidasNormal);
        escreverMonitorFicheiro(textoEscrever);
        escreverMonitorFicheiro("\n");
    }
}


/*###################################################*/
/*                  Trata a mensagem                 */
/*###################################################*/
void trata_mensagem(char palavraSeparar[]){
    //Para funcionar
    char buffer[90];
    strcpy(buffer, palavraSeparar);

    //Onde vai guardar os valores depois da divisao
    char matrix [3][90];
    int i = 0;

    //Começa a divisao
    char *token = strtok(buffer, "-");
    strcpy(matrix[i], token);

    //Ciclo que percorre e vai separando pelos . e copiando para matrix[i]
    while (token != NULL) {
        i++;
        strcpy(matrix[i], token);
        token = strtok(NULL, "-");
        if (token != NULL) {
            strcpy(matrix[i], token);
        }

    }
    if (matrix[0][0]=='X'){
   		if(matrix[2][0] ==  '0'){
	    	printf("Bem-vindo! A Simulação está a decorrer.\n");
	    }else{
	   		printf("A simulação terminou.\n");
            fimSimulacao = 1;
	   	}
   	}else{
        char estrutura = matrix[0][0]; // B1.3.2
        int atributo = atoi(matrix[1]);
        float valor = atof((char *)matrix[2]);
        int instancia = atoi((char *)&matrix[0][1]);
        if(estrutura == 'E'){

        }else if(estrutura== 'B'){
            switch(atributo){
                //Chegou uma pessoa a fila
                case 0:
                    if (valor == 1){
                        bilheteiras[instancia].pessoasVIPFila++;
                    }else if (valor == 0){
                        bilheteiras[instancia].pessoasNormalFila++;
                    }else{
                        printf("Erro!!!!");
                    }
                    break;

                //Uma pessoa desistiu
                case 1:
                    if (valor == 1){
                        bilheteiras[instancia].pessoasDesistentesVIP ++;
                    }else if (valor == 0){
                        bilheteiras[instancia].pessoasDesistentesNormal ++;
                    }else{
                        printf("Erro!!!!");
                    }
                    break;

                //Quando uma pessoa foi atendida e nao esperou na fila
                case 2:
                    if (valor == 1){
                        bilheteiras[instancia].pessoasAtendidasVIP ++;
                    }else if (valor == 0){
                        bilheteiras[instancia].pessoasAtendidasNormal ++;
                    }else{
                        printf("Erro!!!!");
                    }
                    break;

                //Tempo medio de espera na fila
                case 3:
                    bilheteiras[instancia].somaTempoEspera += valor;
                    bilheteiras[instancia].quantidadeTemposEspera++;
                    bilheteiras[instancia].tempoMedioEspera = bilheteiras[instancia].somaTempoEspera / bilheteiras[instancia].quantidadeTemposEspera;
                    break;

                //Tempo medio de compra
                case 4:
                    bilheteiras[instancia].somaTempoCompra += valor;
                    bilheteiras[instancia].quantidadeTemposCompra++;
                    bilheteiras[instancia].tempoMedioCompra = bilheteiras[instancia].somaTempoCompra / bilheteiras[instancia].quantidadeTemposCompra;
                    break;

                //Caso seja atendida uma pessoa que esperou na fila
                case 5:
                    if (valor == 1){
                        bilheteiras[instancia].pessoasAtendidasVIP ++;
                        bilheteiras[instancia].pessoasVIPFila--;
                    }else if (valor == 0){
                        bilheteiras[instancia].pessoasAtendidasNormal ++;
                        bilheteiras[instancia].pessoasNormalFila--;
                    }else{
                        printf("Erro!!!!");
                    }
                    break;

                case 6:

                    break;

                default:
                    printf("Erro!");
                    break;
            }
        }else if(estrutura == 'M'){
            switch (atributo){
                case 0:
                    metros[instancia].numeroPassageirosAtual++;
                    break;
                case 1:
                    metros[instancia].idEstacaoAtual = valor;
                    break;
                case 2:
                    metroEstaViagem[instancia] = true;
                    break;
                case 3:
                    metroEstaViagem[instancia]= false;
                    break;
                default:
                    printf("Erro!!");
                    break;
            }

        }
    }
    imprimeInfo();

}


/*###################################################*/
/*        Leitura Sucessiva das mensagens            */
/*###################################################*/
void LeituraSucessiva (int sockfd) {
    int numero = 0;
    char buffer[TamLinha];                                  //Cria um buffer com o TamLinha definido
    while(!fimSimulacao) {                                  //Enquanto a simulacao nao acabar
        //printf("Chegou \n");
        numero = read(sockfd, buffer, TamLinha);            //Le a mensagem do socket e guarda no buffer
        if(numero==0) {                                     //Quando chega ao fim
            //trata_mensagem(buffer);
            //printf("chegou ao fim");
            break;
        }else if( numero < 0 ) {
            printf ( "Erro: Nao leu do socket \n" );
        }else{
            trata_mensagem(buffer);
            //printf("LeituraSucessiva: \n");
            //printf("%s", buffer);                           //Faz print da mensagem guardada no buffer
        }

    }
}


/*###################################################*/
/*                  Cria o servidor                  */
/*###################################################*/
void criaServidor () {
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

    //Criacao de um novo processo
    int pid;
    if ((pid = fork()) < 0 ) {
        printf ( "Erro ao criar o processo filho \n" );         //Erro na criacao do processo filho
    }else if (pid == 0) {                                       //Processo filho irá tratar das sucessivas leituras e fecha o socket do processo pai
        close(sockfd);
        LeituraSucessiva(novoSocket);
    }
    close(novoSocket);                                          //Fecha o socket filho
}


/*###################################################*/
/*                      Main                         */
/*###################################################*/
int main(int argc, char const * argv[]){
    printf ( "########### Bem vindo ########### \n" );              //Menu
    printf ( "1: Comecar simulacao          \n" );              //Menu
    printf ( "2: Limpar ficheiros da simulacao \n" );              //Menu
    printf ( "################################# \n" );              //Menu
    int selecao = 0;   ;                                                //Variavel que guarda o valor introduzido pelo utilizador
    int acaba = 0;
    while (!acaba) {
        if(fimSimulacao == 1) {                                 //Se a selecao for 1
            acaba = 1;
        }else{
            while(selecao != 1) {
                printf ( "Introduza uma opção: \n" );               //Pede ao utilizador para introduzir uma opcao
                scanf ( "%d" , &selecao );                          //Le valor introduzido pelo utilizador
            }
            criaServidor();                                         //Cria o servidor
        }
    }
    return 0;
}
