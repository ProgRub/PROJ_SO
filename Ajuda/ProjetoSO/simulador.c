#include "config.h"

//###########################################//
//          Variaveis Globais Usadas         //
//###########################################//

//
char mensagem[50];

//Sockets
int sockfd = 0;


//Probabilidades
int probDesistenciaDepoisDeTerBilhete;
int probDesistirDepoisDeEstarMetro;
int probSerVip;
int probDesistenciaFilaVIP;
int probDesistenciaFilaNormal;

//Outras
int numeroDePessoasMaximo;
int tempoViagem;  //Tempo de viagem do comboio de uma estação a outra
int tempoEsperaComboioEstacao;  //Tempo que o comboio espera que os passageiros entrem

//Para ser o atendimento e compra do bilhete
int Pessoa_a_ser_atendida[5] = {0,0,0,0,0}; //0->Balcao estacao 0; 1->Balcao estacao 1; 2->Balcao estacao 2; 3->Balcao estacao 3; 4->Balcao estacao 4;
int Fila_para_ser_atendida [5][2]={{0,0},{0,0},{0,0},{0,0},{0,0}}; //Numero de pessoas na fila do balcao n, {0->VIP 1->Normal}
int Numero_VIP_frente_desistir;  //Quantidade de pessoas que fazem a pessoa VIP desistir de comprar o bilhete
int Numero_normal_frente_desistir;  //Quantidade de pessoas que fazem a pessoa Normal desistir de comprar o bilhete


//Metro
int Lotacao_maxima_metro; //Quantidade maxima de pessoas por metro
int Quantidade_pessoas_espera[5]={0,0,0,0,0}; //0->estacao 0 e metro 0; 1->estacao 1 e metro 1; 2->estacao 2 e metro 0; 3->estacao 3 e metro 1; 4->estacao 4 e metro 1;
int Quantidade_pessoas_espera_metro1_estacao2 = 0;


// Metros
struct metro metros[2];


//Enum para o tipo de pessoas
enum Type {VIP, Normal};


//Semaforos
sem_t trincoAuxiliarApagar;////////////////////////////////////////
sem_t TrincoEnviaMensagem;
sem_t TrincoPessoaFila;
sem_t TrincoCriaPessoa;
sem_t TrincoPessoaChegouFila[5];
sem_t TrincoPessoaEstaSerAtendida[5];
sem_t TrincoAtendimentoVIP[5];
sem_t TrincoAtendimentoNormal[5];
sem_t EntraComboioXChegouEstacaoY[2][3];
sem_t SaiComboioXChegouEstacaoY[2][3];
sem_t TrincoQuantidadePessoasEstacao[5]; //0->estacao 0 e metro 0; 1->estacao 1 e metro 1; 2->estacao 2 e metro 0; 3->estacao 3 e metro 1; 4->estacao 4 e metro 1;
sem_t TrincoPessoasComboio[2];  //0->metro 0; 1->metro 1
sem_t SemMetroEsperaPessoasSair[2];  //0->metro 0; 1->metro 1

//Tarefas
pthread_t IDtarefas[500];
pthread_t IDTarefaMetro[2];

/*###################################################*/
/*               Escrever no ficheiro                */
/*###################################################*/
void escreverSimuladorFicheiro(char textoFicheiro []){
    FILE *fp; // Cria um apontador para um ficheiro
    fp = fopen ("simulador.txt", "a");  // Abre o ficheiro em modo de acrescentar texto

    if (fp == NULL) { // Verifica se o ficheiro existe
        printf("Não foi possível abrir o ficheiro %s.", "simulador.txt");
    }else{
        fprintf(fp, "%s", textoFicheiro); // Escreve no ficheiro o texto
        fclose(fp); // Encerra a conexão com o ficheiro
    }
}


/*###################################################*/
/*               Funcoes auxiliares                  */
/*###################################################*/
bool probabilidade(int p){
    if (rand()%100 < p){
        return true;
    }else{
        return false;
    }
}

int PosicaoEstacaoVetor(int metro, int estacaoDestino){
    int e, valor;
    bool erro = true;
    //printf("metro [%d,%d,%d]",metros[metro].trajeto[0],metros[metro].trajeto[1],metros[metro].trajeto[2]);
    for (e=0; e<3; e++){
        if (metros[metro].trajeto[e]==estacaoDestino){
            valor = e;
            erro = false;
        }
    }
    if (erro){
        //printf("Erro na posicao do comboio por causa de estacao destino %d para o metro %d \n", estacaoDestino, metro);
        return -1;
    }else{
        return valor;
    }
}

int metroPessoaTemApanhar (int estacao, int DestinoPessoa){
    if (estacao == 2){
        if (DestinoPessoa == 1 || DestinoPessoa == 3){
            return 1;
        }else{
            return 0;
        }
    }else{
        if (estacao == 0 || estacao == 4){
            return 0;
        }else{
            return 1;
        }
    }

}


/*###################################################*/
/*               Carregar Dados Simulacao            */
/*###################################################*/
void carregaDados(){
    bool erro = true;
    char nomeFicheiro[] = "server.config";
    int value [10];

    while (erro){
        erro = false;

        FILE * ficheiro;
        ficheiro = fopen(nomeFicheiro, "r");

        if (ficheiro == NULL) {
            printf("Não foi possível abrir o ficheiro %s.", nomeFicheiro);
        }else{
            char buffer[150];
            int j =0;
            while(fgets(buffer, 150, ficheiro)){
                for(int l = 0; l<4; l++){
                    if (buffer[l] == ':'){
                        break;
                    }else{
                        if (isalpha(buffer[l])){
                            printf("ERRO! Inseriu um valor com o carater '%c' na linha %i \n", buffer[l],j+1);
                            erro = true;
                        }
                    }
                }
                char *token = strtok(buffer, ":");
                value[j]=atoi(token);
                j++;
            }
            if(value[0] < 0){
                printf("Erro! A capacidade dos comboios é invalida. Introduza um valor positivo\n");
                erro = true;
            }
            else if(value[1] <1){
                printf("Erro! O numero de pessoas VIP que estão a frente é invalido. Introduza um maior que 0\n");
                erro = true;
            }
            else if( value[2] <1){
                printf("Erro! O numero de pessoas Normais que estão a frente é invalido. Introduza um maior que 0\n");
                erro = true;
            }
            else if(value[3] <0 || value[3]  > 100){
                printf("Erro! A probabilidade de desistencia VIP na fila é invalida. Introduza um valor entre 0 e 100 inclusivo\n");
                erro = true;
            }
            else if(value[4] <0 || value[4]  > 100){
                printf("Erro! A probabilidade de desistencia Normal na fila é invalida. Introduza um valor entre 0 e 100 inclusivo\n");
                erro = true;
            }
            else if(value[5] <0 || value[5] > 100){
                printf("Erro! A probabilidade de desistencia VIP na estacao é invalida.Introduza um valor entre 0 e 100 inclusivo\n");
                erro = true;
            }
            else if(value[6] <0 || value[6]  > 100){
                printf("Erro! A probabilidade de desistencia Normal na estacao é invalida. Introduza um valor entre 0 e 100 inclusivo\n");
                erro = true;
            }
            else if(value[7] <0 || value[7] > 100){
                printf("Erro! A probabilidade de ser VIP é invalida.Introduza um valor entre 0 e 100 inclusivo\n");
                erro = true;
            }
            else if( value[8] <1){
                printf("Erro! Para a simulação ocorrer tem de existir no minimo 1 pessoa.\n");
                erro = true;
            }
            else if(value[9] <= 0){
                printf("Erro! O tempo de viagem é invalido.\n");
                erro = true;
            }
            else if(value[10]<=0){
                printf("Erro! O tempo que o metro espera na estação é invalido.\n");
                erro = true;
            }
        }
        if (erro){
            int selecao = 0;
            printf ( "Faça as alteraçoes necessarias! Prima 1 para continuar: \n" );               //Pede ao utilizador para introduzir uma opcao
            while(selecao != 1) {
                scanf ( "%d" , &selecao );                          //Le valor introduzido pelo utilizador
            }
        }
        fclose(ficheiro);
    }
    Lotacao_maxima_metro = value[0];
    Numero_VIP_frente_desistir = value[1];
    Numero_normal_frente_desistir = value[2];
    probDesistenciaFilaVIP = value[3];
    probDesistenciaFilaNormal = value[4];
    probDesistenciaDepoisDeTerBilhete = value[5];
    probDesistirDepoisDeEstarMetro = value[6];
    probSerVip = value[7];
    numeroDePessoasMaximo = value[8];
    tempoViagem = value[9];
    tempoEsperaComboioEstacao = value[10];
}


/*###################################################*/
/*                   Envia Mensagem                  */
/*###################################################*/
void EnviaMensagem( char *fp, int sockfd) {
    //Fecha trinco
    sem_wait(&TrincoEnviaMensagem);


    int numero;
    char mensagem[TamLinha];
    if( strcpy(mensagem, fp) != 0 ) {
        numero = strlen(mensagem) + 1;
        if( write( sockfd, mensagem, numero) != numero) {
            printf ( "Erro cliente no write! \n" );
        }else{
            //printf ( "Mensagem Enviada! \n" );
        }
    }

    usleep(2000);
    //Abre trinco
    sem_post(&TrincoEnviaMensagem);
}


/*###################################################*/
/*               Criacao da pessoa                   */
/*###################################################*/
struct pessoa criaPessoa(){
    sem_wait(&TrincoCriaPessoa);

    //Valores aleatorios
    int valoresIn[2]={0,1};
    int valorAleatorioEstacaoInicial = valoresIn[rand() % 2 ];
    int valoresFn [5]={0,1,2,3,4};
    int valorAleatorioEstacaoDEstino = valoresFn[rand() % 5 ];

    //Cria a pessoa
    struct pessoa p;
    p.estacao = valorAleatorioEstacaoInicial;
    p.destino = valorAleatorioEstacaoDEstino;
    p.temBilhete = false;
    p.VIP = probabilidade(probSerVip);
    if ( PosicaoEstacaoVetor(metroPessoaTemApanhar(p.estacao, 0),  p.destino)== -1){
        p.temQueMudarDeEstacao= true;
    }else{
        p.temQueMudarDeEstacao = false;
    }
    p.estaComboio = false;
    p.desistiu = false;

    //Caso os valores de destino e da estacao atual sejam iguais altera-os
    if (p.estacao == p.destino){
        p.destino = 2;
    }

    usleep(100000);

    //Manda mensagem a dizer que criou pessoa
    escreverSimuladorFicheiro("Uma pessoa foi criada\n");
    printf("Uma pessoa foi criada\n");

    //Abre o trinco
    sem_post(&TrincoCriaPessoa);
    return p;
}


/*###################################################*/
/*A pessoa esta na fila e compra quer comprar bilhete*/
/*###################################################*/
void FilaEspera(struct pessoa *p){
    char textoEscrever[500];
    sem_wait(&TrincoPessoaChegouFila[p->estacao]);


    //Inicializacao dos tempos de controlo
    clock_t tempo_Inicial_compra, tempo_espera_final, tempo_espera_inicial;
    bool logoAtendida = false;

    //Caso não haja ninguem a ser atendido
    if (Pessoa_a_ser_atendida[p->estacao]==0){
        Pessoa_a_ser_atendida[p->estacao] = 1;

        //Começa a compra
        logoAtendida = true;
        tempo_Inicial_compra = clock();

        sem_post(&TrincoPessoaChegouFila[p->estacao]);
    }else{
        //Tempo de espera inicial
        tempo_espera_inicial = clock();

        //Caso a pessoa que chegou seja VIP
        if (p->VIP == true){
            sem_wait(&TrincoPessoaFila);

            //A pessoa vip pensa em desistir porque a estacao tem um numero maximo maior
            if (!probabilidade(probDesistenciaFilaVIP) && Fila_para_ser_atendida[p->estacao][0]< Numero_VIP_frente_desistir){
                //A pessoa vip nao desiste e espera pelo atendimento
                Fila_para_ser_atendida[p->estacao][0] ++;
                sem_post(&TrincoPessoaFila);

                sprintf(mensagem, "B%i-0-%i", p->estacao,p->VIP);
                EnviaMensagem(mensagem, sockfd);
                sprintf(textoEscrever, "Uma pessoa VIP colocou-se na fila na estação %i.\n" , p->estacao);
                escreverSimuladorFicheiro(textoEscrever);
                printf("Uma pessoa VIP colocou-se na fila na estação %i.\n" , p->estacao);

                sem_post(&TrincoPessoaChegouFila[p->estacao]);
                sem_wait(&TrincoAtendimentoVIP[p->estacao]);
            }else{
                sem_post(&TrincoPessoaFila);
                //A pessoa Vip desiste
                p->desistiu = true;

                sprintf(mensagem, "B%i-1-%i", p->estacao,p->VIP);
                EnviaMensagem(mensagem, sockfd);
                sprintf(textoEscrever, "Uma pessoa VIP desistiu na estacao %i.\n" ,p->estacao);
                escreverSimuladorFicheiro(textoEscrever);
                printf("Uma pessoa VIP desistiu na estacao %i.\n" ,p->estacao);

                sem_post(&TrincoPessoaChegouFila[p->estacao]);
                return;
            }
        }else{

            sem_wait(&TrincoPessoaFila);

            //A pessoa normal pensa em desistir
            if (!probabilidade(probDesistenciaFilaNormal) && (Fila_para_ser_atendida [p->estacao][1] + Fila_para_ser_atendida[p->estacao][0]  < Numero_normal_frente_desistir )){
                //A pessoa normal nao desiste
                Fila_para_ser_atendida[p->estacao][1] ++;
                sem_post(&TrincoPessoaFila);

                sprintf(mensagem, "B%i-0-%i", p->estacao,p->VIP);
                EnviaMensagem(mensagem, sockfd);
                sprintf(textoEscrever, "Uma pessoa Normal colocou-se na fila na estação %i.\n" , p->estacao);
                escreverSimuladorFicheiro(textoEscrever);
                printf("Uma pessoa Normal colocou-se na fila na estação %i.\n" , p->estacao);

                sem_post(&TrincoPessoaChegouFila[p->estacao]);
                sem_wait(&TrincoAtendimentoNormal[p->estacao]);
            }else{
                sem_post(&TrincoPessoaFila);

                //A pessoa normal desiste
                p->desistiu = true;

                sprintf(mensagem, "B%i-1-%i", p->estacao,p->VIP);
                EnviaMensagem(mensagem, sockfd);
                sprintf(textoEscrever, "Uma pessoa Normal desistiu na estacao %i.\n", p->estacao);
                escreverSimuladorFicheiro(textoEscrever);
                printf("Uma pessoa Normal desistiu na estacao %i.\n", p->estacao);

                sem_post(&TrincoPessoaChegouFila[p->estacao]);
                return;
            }
        }

        tempo_Inicial_compra = clock();
        tempo_espera_final = clock();
    }

    //A pessoa esta a ser atendida
    sem_wait(&TrincoPessoaEstaSerAtendida[p->estacao]);

    //Tempo de espera na fila
    float tempoEsperaFila;
    if (logoAtendida){
        tempoEsperaFila = 0;
    }else{
        tempoEsperaFila = ((float) (tempo_espera_final - tempo_espera_inicial) / CLOCKS_PER_SEC)*10000;
    }

    sprintf(mensagem, "B%i-3-%f", p->estacao, tempoEsperaFila);
    EnviaMensagem(mensagem, sockfd);
    sprintf(textoEscrever, "Uma pessoa está a ser atendida na estacao %i e esperou %f\n",  p->estacao, tempoEsperaFila);
    escreverSimuladorFicheiro(textoEscrever);
    printf(textoEscrever);

    //Se a pessoa e VIP
    sem_wait(&TrincoPessoaFila);
    if (!logoAtendida){
        if (p->VIP ){
            Fila_para_ser_atendida[p->estacao][0]--;
        }else {
            Fila_para_ser_atendida[p->estacao][1]--;
        }
        sprintf(mensagem, "B%i-5-%i", p->estacao,p->VIP);
        EnviaMensagem(mensagem, sockfd);
    }else{
        sprintf(mensagem, "B%i-2-%i", p->estacao,p->VIP);
    }
    sem_post(&TrincoPessoaFila);


    p->temBilhete = true;

    //Tempo de compra
    clock_t tempo_Final_Compra = clock();
    float tempoEsperaCompra = ((float) (tempo_Final_Compra - tempo_Inicial_compra) / CLOCKS_PER_SEC)*10000;
    sprintf(mensagem, "B%i-4-%f", p->estacao, tempoEsperaCompra);

    EnviaMensagem(mensagem, sockfd);
    sprintf(textoEscrever, "Uma pessoa comprou bilhete na estação %i e demorou %f a comprar o bilhete\n", p->estacao, tempoEsperaCompra);
    escreverSimuladorFicheiro(textoEscrever);
    printf(textoEscrever);

    usleep(600000);

    //A pessoa vai acabar de ser atendida
    sem_wait(&TrincoPessoaFila);

    //Caso hajam pessoas VIP
    if(Fila_para_ser_atendida[p->estacao][0]>0){
        sem_post(&TrincoAtendimentoVIP[p->estacao]);

    //Caso hajam pessoas Normais
    }else if (Fila_para_ser_atendida[p->estacao][1]>0){
        sem_post(&TrincoAtendimentoNormal[p->estacao]);

    //Caso não haja mais pessoas
    }else{
        Pessoa_a_ser_atendida[p->estacao] = 0;
    }
    sem_post(&TrincoPessoaFila);

    //A pessoa acabou de ser atendida
    sem_post(&TrincoPessoaEstaSerAtendida[p->estacao]);
}


/*###################################################*/
/*          A pessoa esta a espera do metro          */
/*###################################################*/
void EsperaMetro(struct pessoa *p){
    char textoEscrever[500];
    //Caso a pessoa queira desistir
    if (probabilidade(probDesistenciaDepoisDeTerBilhete)){
        //Mandar mensagem
        //Enviar Mensagem
        printf("A pessoa desistiu depois de ter bilhete\n");

    }else{//A pessoa não desiste
        //Numero de pessoas a espera do metro e incrementado
        sem_wait(&TrincoQuantidadePessoasEstacao[p->estacao]);
        if (p->estacao==2 && p->destino%2 != 0){
            Quantidade_pessoas_espera_metro1_estacao2++;
        }else{
            Quantidade_pessoas_espera[p->estacao]++;
        }
        sem_post(&TrincoQuantidadePessoasEstacao[p->estacao]);

        //pessoa foi criada na estacao 2
        if (p->estacao == 0){
            // tem que apanha o metro 0
            if (p->destino % 2 == 0){
                //espera que o comboio 0 chegue à estação 0
                sem_wait(&EntraComboioXChegouEstacaoY[0][0]);

                //Mandar mensagem
                printf("a pessoa entrou no metro \n");
                p->estaComboio = true;

                sem_wait(&TrincoPessoasComboio[0]);
                metros[0].numeroPassageirosAtual++;

                //Caso a pessoa queira desistir da viagem e a proxima viagem do comboio é diferente do destino
                if (probabilidade(probDesistirDepoisDeEstarMetro) && metros[0].trajeto[metros[0].idEstacaoSeguinte]!= p->destino  ){

                    //Adiciona a pessoa a lista de pessoas que vao sair na mesma saida (proxima estacao do metro)
                    metros[0].pessoasEsperaSaida[1]++;

                    //Retira a pessoa da lista da estacao
                    sem_wait(&TrincoQuantidadePessoasEstacao[0]);
                    Quantidade_pessoas_espera[0]--;
                    sem_post(&TrincoQuantidadePessoasEstacao[0]);

                    //Abre novamente o trinco
                    sem_post(&TrincoPessoasComboio[0]);

                    //Espera que o metro chegue a estacao
                    sem_wait(&SaiComboioXChegouEstacaoY[0][1]);

                    //Retira a pessoa
                    p->estaComboio = false;

                    //Retira a pessoa do metro
                    sem_wait(&TrincoPessoasComboio[0]);
                    metros[0].numeroPassageirosAtual--;
                    metros[0].pessoasEsperaSaida[1]--;


                    printf("A pessoa desistiu quando estava no metro\n");
                    //Mandar mensagem

                    if (metros[0].pessoasEsperaSaida[1]==0){
                        sem_post(&TrincoPessoasComboio[0]);
                        sem_post(&SemMetroEsperaPessoasSair[0]);
                    }else{
                        sem_post(&TrincoPessoasComboio[0]);
                    }

                }else{
                    int posicao = PosicaoEstacaoVetor(0, p->destino);

                    //Adiciona a pessoa a lista de pessoas que vao sair na mesma saida (destino da pessoa)
                    metros[0].pessoasEsperaSaida[posicao]++;

                    //Retira a pessoa da lista da estacao
                    sem_wait(&TrincoQuantidadePessoasEstacao[0]);
                    Quantidade_pessoas_espera[0]--;
                    sem_post(&TrincoQuantidadePessoasEstacao[0]);

                    //Abre novamente o trinco
                    sem_post(&TrincoPessoasComboio[0]);

                    //Espera que o metro chegue a estacao
                    sem_wait(&SaiComboioXChegouEstacaoY[0][posicao]);

                    //Retira a pessoa do metro
                    sem_wait(&TrincoPessoasComboio[0]);
                    p->estaComboio = false;
                    metros[0].numeroPassageirosAtual--;
                    metros[0].pessoasEsperaSaida[posicao]--;


                    //Manda mensagem
                    printf("A pessoa chegou ao destino %d \n", p->destino);
                    if (p->destino == 2){
                        if (metros[0].pessoasEsperaSaida[1]==0){
                            sem_post(&TrincoPessoasComboio[0]);
                            sem_post(&SemMetroEsperaPessoasSair[0]);
                        }else{
                            sem_post(&TrincoPessoasComboio[0]);
                        }
                    }else{
                        if (metros[0].pessoasEsperaSaida[2]==0){
                            sem_post(&TrincoPessoasComboio[0]);
                            sem_post(&SemMetroEsperaPessoasSair[0]);
                        }else{
                            sem_post(&TrincoPessoasComboio[0]);
                        }
                    }

                }
            }else{// tem que apanha o metro 1
                //espera que o comboio 0 chegue à estação 0
                sem_wait(&EntraComboioXChegouEstacaoY[0][0]);

                //Mandar mensagem
                printf("a pessoa entrou no metro \n");
                p->estaComboio = true;

                //Fecha o trinco para a pessoa entrar no metro
                sem_wait(&TrincoPessoasComboio[0]);

                //Adiciona a pessoa a lista de pessoas que vao sair na mesma saida (2)
                metros[0].pessoasEsperaSaida[1]++;
                metros[0].numeroPassageirosAtual++;

                //Retira a pessoa da lista da estacao
                sem_wait(&TrincoQuantidadePessoasEstacao[0]);
                Quantidade_pessoas_espera[0]--;
                sem_post(&TrincoQuantidadePessoasEstacao[0]);

                //Abre o trinco para da pessoa entrar no metro
                sem_post(&TrincoPessoasComboio[0]);

                //Espera que o metro chegue a estacao 2
                sem_wait(&SaiComboioXChegouEstacaoY[0][1]);

                //Retira a pessoa do metro
                sem_wait(&TrincoPessoasComboio[0]);
                metros[0].numeroPassageirosAtual--;
                metros[0].pessoasEsperaSaida[1]--;
                p->estacao = 2;
                p->estaComboio = false;

                if (metros[0].pessoasEsperaSaida[1]==0){
                    sem_post(&TrincoPessoasComboio[0]);
                    sem_post(&SemMetroEsperaPessoasSair[0]);
                }else{
                    sem_post(&TrincoPessoasComboio[0]);
                }

                if (probabilidade(probDesistirDepoisDeEstarMetro)) {
                    printf("A pessoa desistiu quando tinha que mudar de estacao\n");
                    //a pessoa desiste
                    return;
                }

                //Adiciona a pessoa da lista da estacao
                sem_wait(&TrincoQuantidadePessoasEstacao[2]);
                Quantidade_pessoas_espera_metro1_estacao2++;
                sem_post(&TrincoQuantidadePessoasEstacao[2]);

                //Abre o trinco das pessoas a entrarem
                sem_post(&TrincoPessoasComboio[0]);



                //espera que o comboio 1 chegue à estação 1
                sem_wait(&EntraComboioXChegouEstacaoY[1][1]);

                //Mandar mensagem
                //printf("a pessoa entrou no metro \n");
                p->estaComboio = true;

                //Adiciona a pessoa ao metro
                sem_wait(&TrincoPessoasComboio[1]);
                metros[1].numeroPassageirosAtual++;

                //Adiciona a pessoa a lista de pessoas que vao sair na mesma saida (destino da pessoa)
                int posicao = PosicaoEstacaoVetor(1, p->destino);
                metros[1].pessoasEsperaSaida[posicao]++;

                //Retira a pessoa da lista da estacao
                sem_wait(&TrincoQuantidadePessoasEstacao[1]);
                Quantidade_pessoas_espera_metro1_estacao2--;
                sem_post(&TrincoQuantidadePessoasEstacao[1]);

                //Acabou de adicionar a pessoa
                sem_post(&TrincoPessoasComboio[1]);

                //Espera que o metro chegue a estacao
                sem_wait(&SaiComboioXChegouEstacaoY[1][posicao]);

                //Retira a pessoa do metro
                sem_wait(&TrincoPessoasComboio[1]);
                p->estaComboio = false;
                metros[1].numeroPassageirosAtual--;
                metros[1].pessoasEsperaSaida[posicao]--;
                sem_post(&TrincoPessoasComboio[1]);

                //Manda mensagem
                printf("A pessoa chegou ao destino %d \n", p->destino);
                if (p->destino == 1){
                    if (metros[1].pessoasEsperaSaida[0]==0){
                        sem_post(&TrincoPessoasComboio[1]);
                        sem_post(&SemMetroEsperaPessoasSair[1]);
                    }else{
                        sem_post(&TrincoPessoasComboio[1]);
                    }
                }else{
                    if (metros[1].pessoasEsperaSaida[2]==0){
                        sem_post(&TrincoPessoasComboio[1]);
                        sem_post(&SemMetroEsperaPessoasSair[1]);
                    }else{
                        sem_post(&TrincoPessoasComboio[1]);
                    }
                }

            }
        }else{//A pessoa foi criada noutra estacao 1
            // tem que apanha o metro 1
            if (p->destino == 2 || p->destino == 3){
                //espera que o comboio 1 chegue à estação 1
                sem_wait(&EntraComboioXChegouEstacaoY[1][0]);

                //Mandar mensagem
                printf("a pessoa entrou no metro \n");
                p->estaComboio = true;

                sem_wait(&TrincoPessoasComboio[1]);
                metros[1].numeroPassageirosAtual++;

                //Caso a pessoa queira desistir da viagem e a proxima viagem do comboio é diferente do destino
                if (probabilidade(probDesistirDepoisDeEstarMetro) && metros[1].trajeto[metros[1].idEstacaoSeguinte]!= p->destino  ){

                    //Adiciona a pessoa a lista de pessoas que vao sair na mesma saida (proxima estacao do metro)
                    metros[1].pessoasEsperaSaida[1]++;

                    //Retira a pessoa da lista da estacao
                    sem_wait(&TrincoQuantidadePessoasEstacao[1]);
                    Quantidade_pessoas_espera[1]--;
                    sem_post(&TrincoQuantidadePessoasEstacao[1]);

                    //Pessoa acabou de entrar
                    sem_post(&TrincoPessoasComboio[1]);

                    //Espera que o metro chegue a estacao
                    sem_wait(&SaiComboioXChegouEstacaoY[1][1]);

                    //Retira a pessoa
                    p->estaComboio = false;

                    //Retira a pessoa do metro
                    sem_wait(&TrincoPessoasComboio[1]);
                    metros[1].numeroPassageirosAtual--;
                    metros[1].pessoasEsperaSaida[1]--;
                    sem_post(&TrincoPessoasComboio[1]);

                    printf("A pessoa desistiu quando estava no metro\n");
                    //Mandar mensagem
                    if (metros[1].pessoasEsperaSaida[1]==0){
                        sem_post(&TrincoPessoasComboio[1]);
                        sem_post(&SemMetroEsperaPessoasSair[1]);
                    }else{
                        sem_post(&TrincoPessoasComboio[1]);
                    }

                }else{
                    int posicao = PosicaoEstacaoVetor(1, p->destino);

                    //Adiciona a pessoa a lista de pessoas que vao sair na mesma saida (destino da pessoa)
                    metros[1].pessoasEsperaSaida[posicao]++;

                    //Retira a pessoa da lista da estacao
                    sem_wait(&TrincoQuantidadePessoasEstacao[1]);
                    Quantidade_pessoas_espera[1]--;
                    sem_post(&TrincoQuantidadePessoasEstacao[1]);

                    //Pessoa acabou de entrar
                    sem_post(&TrincoPessoasComboio[1]);

                    //Espera que o metro chegue a estacao
                    sem_wait(&SaiComboioXChegouEstacaoY[1][posicao]);

                    //Retira a pessoa do metro
                    sem_wait(&TrincoPessoasComboio[1]);
                    p->estaComboio = false;
                    metros[1].numeroPassageirosAtual--;
                    metros[1].pessoasEsperaSaida[posicao]--;
                    sem_post(&TrincoPessoasComboio[1]);

                    //Manda mensagem

                    printf("A pessoa chegou ao destino %d \n", p->destino);
                    if (p->destino == 2){
                        if (metros[1].pessoasEsperaSaida[1]==0){
                            sem_post(&TrincoPessoasComboio[1]);
                            sem_post(&SemMetroEsperaPessoasSair[1]);
                        }else{
                            sem_post(&TrincoPessoasComboio[1]);
                        }
                    }else{
                        if (metros[1].pessoasEsperaSaida[2]==0){
                            sem_post(&TrincoPessoasComboio[1]);
                            sem_post(&SemMetroEsperaPessoasSair[1]);
                        }else{
                            sem_post(&TrincoPessoasComboio[1]);
                        }
                    }
                }
            }else{// tem que apanha o metro 0
                //espera que o comboio 1 chegue à estação 1
                sem_wait(&EntraComboioXChegouEstacaoY[1][0]);

                //Mandar mensagem
                printf("a pessoa entrou no metro \n");
                p->estaComboio = true;

                sem_wait(&TrincoPessoasComboio[1]);
                metros[1].numeroPassageirosAtual++;

                //Retira a pessoa da lista da estacao
                sem_wait(&TrincoQuantidadePessoasEstacao[1]);
                Quantidade_pessoas_espera[1]--;
                sem_post(&TrincoQuantidadePessoasEstacao[1]);

                //Adiciona a pessoa a lista de pessoas que vao sair na mesma saida (2)
                metros[1].pessoasEsperaSaida[1]++;
                sem_post(&TrincoPessoasComboio[1]);

                //Espera que o metro chegue a estacao 2
                sem_wait(&SaiComboioXChegouEstacaoY[1][1]);

                //Retira a pessoa do metro
                sem_wait(&TrincoPessoasComboio[1]);
                metros[1].numeroPassageirosAtual--;
                metros[1].pessoasEsperaSaida[1]--;

                p->estacao = 2;
                p->estaComboio = false;

                if (metros[1].pessoasEsperaSaida[1]==0){
                    sem_post(&TrincoPessoasComboio[1]);
                    sem_post(&SemMetroEsperaPessoasSair[1]);
                }else{
                    sem_post(&TrincoPessoasComboio[1]);
                }

                if (probabilidade(probDesistirDepoisDeEstarMetro)) {
                    printf("A pessoa desistiu quando tinha que mudar de estacao\n");
                    //a pessoa desiste
                    return;
                }


                //Adiciona a pessoa da lista da estacao
                sem_wait(&TrincoQuantidadePessoasEstacao[2]);
                Quantidade_pessoas_espera[2]++;
                sem_post(&TrincoQuantidadePessoasEstacao[2]);

                //A pessoa acabou de chegar a estacao
                sem_post(&TrincoPessoasComboio[1]);

                //espera que o comboio 0 chegue à estação 1
                sem_wait(&EntraComboioXChegouEstacaoY[0][1]);

                //Mandar mensagem
                printf("a pessoa entrou no metro \n");
                p->estaComboio = true;

                sem_wait(&TrincoPessoasComboio[0]);
                metros[0].numeroPassageirosAtual++;

                int posicao = PosicaoEstacaoVetor(0, p->destino);

                //Retira a pessoa da lista da estacao
                sem_wait(&TrincoQuantidadePessoasEstacao[2]);
                Quantidade_pessoas_espera[2]--;
                sem_post(&TrincoQuantidadePessoasEstacao[2]);

                //Adiciona a pessoa a lista de pessoas que vao sair na mesma saida (destino da pessoa)
                metros[0].pessoasEsperaSaida[posicao]++;
                sem_post(&TrincoPessoasComboio[0]);

                //Espera que o metro chegue a estacao
                sem_wait(&SaiComboioXChegouEstacaoY[0][posicao]);

                //Retira a pessoa do metro
                sem_wait(&TrincoPessoasComboio[0]);
                p->estaComboio = false;
                metros[0].numeroPassageirosAtual--;
                metros[0].pessoasEsperaSaida[posicao]--;
                sem_post(&TrincoPessoasComboio[0]);

                //Manda mensagem
                //printf("A pessoa chegou ao destino %d \n", p->destino);
                if (p->destino == 4){
                    if (metros[0].pessoasEsperaSaida[2]==0){
                        sem_post(&TrincoPessoasComboio[0]);
                        sem_post(&SemMetroEsperaPessoasSair[0]);
                    }else{
                        sem_post(&TrincoPessoasComboio[0]);
                    }
                }else{
                    if (metros[0].pessoasEsperaSaida[0]==0){
                        sem_post(&TrincoPessoasComboio[0]);
                        sem_post(&SemMetroEsperaPessoasSair[0]);
                    }else{
                        sem_post(&TrincoPessoasComboio[0]);
                    }
                }

            }
        }
    }
}


/*###################################################*/
/*               Criacao do metro                    */
/*###################################################*/
void criarMetro(int numeroMetro){
    struct metro *m = &(metros[numeroMetro]);
    m->id = numeroMetro;
    m->numeroPassageirosAtual = 0;
    if (numeroMetro == 0){
        m->trajeto[0] = 0;
        m->trajeto[1] = 2;
        m->trajeto[2] = 4;
    }else{
        m->trajeto[0] = 1;
        m->trajeto[1] = 2;
        m->trajeto[2] = 3;
    }
    m->idEstacaoAtual = 0;
    m->idEstacaoSeguinte = 1;
    m->direcao = 1;
    m->pessoasEsperaSaida[0] = 0;
    m->pessoasEsperaSaida[1] = 0;
    m->pessoasEsperaSaida[2] = 0;
}


/*###################################################*/
/*            Movimentacao do metro                  */
/*###################################################*/
void movimentarMetro(struct metro *m){
    if(m->direcao == 1){
        if(m->idEstacaoAtual == 2){
            //Envia mensagem de começo de viagem
            sprintf(mensagem, "M%i-2-0", m->id );
            EnviaMensagem(mensagem, sockfd);

            m->direcao = 0;
            m->idEstacaoAtual = 1;
            m->idEstacaoSeguinte = 0;

            printf("O metro %i vai sair para a estacao %i\n", m->id, m->trajeto[m->idEstacaoAtual]);

            sleep(tempoViagem); // Tempo de viagem 2 s

            //Envia a mensagem de fim de viagem
            sprintf(mensagem, "M%i-3-0", m->id);
            EnviaMensagem(mensagem, sockfd);

        }else if (m->idEstacaoAtual == 1){
            //Envia mensagem de começo de viagem
            sprintf(mensagem, "M%i-2-0", m->id );
            EnviaMensagem(mensagem, sockfd);

            printf("O metro %i vai sair para a estacao %i\n",m->id, m->trajeto[m->idEstacaoSeguinte]);
            m->idEstacaoAtual = 2;
            m->idEstacaoSeguinte = 1;

            sleep(tempoViagem); // Tempo de viagem 2 s

            //Envia a mensagem de fim de viagem
            sprintf(mensagem, "M%i-3-0", m->id);
            EnviaMensagem(mensagem, sockfd);

        }else{
            //Envia mensagem de começo de viagem
            sprintf(mensagem, "M%i-2-0", m->id );
            EnviaMensagem(mensagem, sockfd);


            printf("O metro %i vai sair para a estacao %i\n",m->id, m->trajeto[m->idEstacaoSeguinte]);

            m->idEstacaoAtual = 1;
            m->idEstacaoSeguinte = 2;

            sleep(tempoViagem); // Tempo de viagem 2 s

            //Envia a mensagem de fim de viagem
            sprintf(mensagem, "M%i-3-0", m->id);
            EnviaMensagem(mensagem, sockfd);
        }
    }else{
        if(m->idEstacaoAtual == 0){
            //Envia mensagem de começo de viagem
            sprintf(mensagem, "M%i-2-0", m->id );
            EnviaMensagem(mensagem, sockfd);

            m->direcao = 1;
            m->idEstacaoAtual = 1;
            m->idEstacaoSeguinte = 2;

            printf("O metro %i vai sair para a estacao %i\n",m->id, m->trajeto[m->idEstacaoAtual]);

            sleep(tempoViagem); // Tempo de viagem 2 s

            //Envia a mensagem de fim de viagem
            sprintf(mensagem, "M%i-3-0", m->id);
            EnviaMensagem(mensagem, sockfd);

        }else if (m->idEstacaoAtual == 1){
            //Envia mensagem de começo de viagem
            sprintf(mensagem, "M%i-2-0", m->id );
            EnviaMensagem(mensagem, sockfd);

            printf("O metro %i vai sair para a estacao %i\n",m->id, m->trajeto[m->idEstacaoSeguinte]);

            m->idEstacaoAtual = 0;
            m->idEstacaoSeguinte = 1;

            sleep(tempoViagem); // Tempo de viagem 2 s

            //Envia a mensagem de fim de viagem
            sprintf(mensagem, "M%i-3-0", m->id);
            EnviaMensagem(mensagem, sockfd);

        }else{
            //Envia mensagem de começo de viagem
            sprintf(mensagem, "M%i-2-0", m->id );
            EnviaMensagem(mensagem, sockfd);

            printf("O metro %i vai sair para a estacao %i\n",m->id, m->trajeto[m->idEstacaoSeguinte]);

            m->idEstacaoAtual = 1;
            m->idEstacaoSeguinte = 0;

            sleep(tempoViagem);

            //Envia a mensagem de fim de viagem
            sprintf(mensagem, "M%i-3-0", m->id);
            EnviaMensagem(mensagem, sockfd);
        }
    }
}


/*###################################################*/
/*                  Tarefa Metro                     */
/*###################################################*/
void * Metro(void * ptr){
    int * numeroMetro = (int *) ptr;
    struct metro *m = &(metros[*numeroMetro]);
    bool acaba = true;
    while (acaba){
        //Envia mensagem da estacao
        sprintf(mensagem, "M%i-1-%i", m->id, m->trajeto[m->idEstacaoAtual] );
        EnviaMensagem(mensagem, sockfd);

        sem_wait(&trincoAuxiliarApagar);
        printf("------------------------------------------------------\n");
        printf("-Pessoas no metro %d à chegada a estacao %d: [%d,%d,%d]\n", *numeroMetro, m->trajeto[m->idEstacaoAtual] ,m->pessoasEsperaSaida[0],m->pessoasEsperaSaida[1],m->pessoasEsperaSaida[2]);
        printf("-Quantidade pessoas estacao: [%d,%d,%d,%d,%d] %d\n", Quantidade_pessoas_espera[0],Quantidade_pessoas_espera[1],Quantidade_pessoas_espera[2],Quantidade_pessoas_espera[3],Quantidade_pessoas_espera[4], Quantidade_pessoas_espera_metro1_estacao2);
        printf("-O metro %d vai deixar %d pessoas na estacao %d!!\n", *numeroMetro, m->pessoasEsperaSaida[m->idEstacaoAtual],m->trajeto[m->idEstacaoAtual]);
        printf("Numero de pessoas para assinalar: %i\n",m->pessoasEsperaSaida[m->idEstacaoAtual]);
        printf("------------------------------------------------------\n");
        sem_post(&trincoAuxiliarApagar);

        //Caso hajam pessoas para sair na estacao
        if (m->pessoasEsperaSaida[m->idEstacaoAtual]>0){
            int valor = m->pessoasEsperaSaida[m->idEstacaoAtual];
            for (int e = 0; e<valor; e++){
                sem_post(&SaiComboioXChegouEstacaoY[*numeroMetro][m->idEstacaoAtual]);
            }

            //Fica à espera que as pessoas saiam do metro
            sem_wait(&SemMetroEsperaPessoasSair[*numeroMetro]);

        }
        /*printf("-Pessoas no metro %d depois da libertacao na estacao %d: [%d,%d,%d]\n", *numeroMetro, m->trajeto[m->idEstacaoAtual] ,m->pessoasEsperaSaida[0],m->pessoasEsperaSaida[1],m->pessoasEsperaSaida[2]);
        printf("------------------------------------------------------\n");*/



        //Obtem a quantidade de pessoas a espera na estacao de apanhar o metro
        sem_wait(&TrincoQuantidadePessoasEstacao[m->trajeto[m->idEstacaoAtual]]);
        int numPessoasEspera = 0;
        if (*numeroMetro == 1 && m->idEstacaoAtual == 1){
            numPessoasEspera = Quantidade_pessoas_espera_metro1_estacao2;
        }else{
            numPessoasEspera = Quantidade_pessoas_espera[m->trajeto[m->idEstacaoAtual]];
        }
        sem_post(&TrincoQuantidadePessoasEstacao[m->trajeto[m->idEstacaoAtual]]);

        //Caso ainda haja espaco no metro
        if (m->numeroPassageirosAtual < Lotacao_maxima_metro && numPessoasEspera>0){
            int e, numLugaresMetro, aux;

            //Conta quantos lugares ainda tem disponivel
            sem_wait(&TrincoPessoasComboio[*numeroMetro]);
            numLugaresMetro = Lotacao_maxima_metro - m->numeroPassageirosAtual;
            sem_post(&TrincoPessoasComboio[*numeroMetro]);

            //Assinala quantas pessoas podem entrar
            if (numLugaresMetro > numPessoasEspera){
                aux = numPessoasEspera;
            }else{
                aux = numLugaresMetro;
            }
            for (e=0; e<aux;e++){
                sem_post(&EntraComboioXChegouEstacaoY[*numeroMetro][m->idEstacaoAtual]); // Abre as portas do metro para entrar
            }
            //printf("O valor de aux: %d", aux);

            //Espera que as pessoas entrem
            int i;
            sem_getvalue(&EntraComboioXChegouEstacaoY[*numeroMetro][m->idEstacaoAtual],&i);
            while (i >0){
                sem_getvalue(&EntraComboioXChegouEstacaoY[*numeroMetro][m->idEstacaoAtual],&i);
                //printf("O valor de i: %d",i);
            }

        }else{
            printf("O metro %d não carregou ninguem na estacao %d\n", *numeroMetro, m->trajeto[m->idEstacaoAtual] );
        }
        //Print(metro vai arrancar)
        //Mensagem

        //printf("Pessoas no metro %d à saida para a estacao %d: [%d,%d,%d]\n", *numeroMetro, m->trajeto[m->idEstacaoSeguinte] ,m->pessoasEsperaSaida[0],m->pessoasEsperaSaida[1],m->pessoasEsperaSaida[2]);

        //Espera 2 segundos numa dada estacao
        sleep(tempoEsperaComboioEstacao);

        //Arranca com o metro para a proxima estacao

        movimentarMetro(m);

        sem_wait(&TrincoQuantidadePessoasEstacao[m->trajeto[m->idEstacaoAtual]]);
        if(Quantidade_pessoas_espera[0] == 0 && Quantidade_pessoas_espera[1] == 0 && Quantidade_pessoas_espera[2] == 0 && Quantidade_pessoas_espera[3]== 0 && Quantidade_pessoas_espera[4] == 0 && Quantidade_pessoas_espera_metro1_estacao2 == 0){
            printf("teste1");
            if(Pessoa_a_ser_atendida[0] == 0 && Pessoa_a_ser_atendida[1] == 0 && Fila_para_ser_atendida[0][0] == 0 && Fila_para_ser_atendida[0][1] == 0 && Fila_para_ser_atendida[1][0] == 0 && Fila_para_ser_atendida[1][1] == 0){
                printf("teste2");
                if (metros[0].pessoasEsperaSaida[0]== 0 && metros[0].pessoasEsperaSaida[1]== 0 && metros[0].pessoasEsperaSaida[2]== 0 && metros[1].pessoasEsperaSaida[0]== 0 && metros[1].pessoasEsperaSaida[1]== 0 && metros[1].pessoasEsperaSaida[2]== 0){
                    printf("Teste3");
                    acaba = false;
                }
            }
        }
        sem_post(&TrincoQuantidadePessoasEstacao[m->trajeto[m->idEstacaoAtual]]);
    }

}


/*###################################################*/
/*                  Tarefa Pessoa                    */
/*###################################################*/
void Pessoa(void * ptr){
    //Cria pessoa
    struct pessoa p = criaPessoa();

    //Coloca a pessoa em fila de espera
    FilaEspera(&p);

    //Caso a pessoa não tenha desistido na fila
    if (!p.desistiu){
        EsperaMetro(&p);
    }

    escreverSimuladorFicheiro("Acabou");
}


/*###################################################*/
/*           Inicializacoes necessarias              */
/*###################################################*/
void inicializacao(){
    //Ininialização dos semaforos
    sem_init(&TrincoCriaPessoa,0,1);
    sem_init(&trincoAuxiliarApagar,0,1);
    sem_init(&TrincoEnviaMensagem,0,1);
    sem_init(&TrincoPessoaFila,0,1);
    sem_init(&SemMetroEsperaPessoasSair,0,0);
    int i,e;
    for (e=0; e<2;e++){
        sem_init(&TrincoPessoasComboio[e],0,1);
        for (i=0;i<3;i++){
            sem_init(&EntraComboioXChegouEstacaoY[e][i],0,0);
            sem_init(&SaiComboioXChegouEstacaoY[e][i],0,0);
        }
    }
    for (i=0;i<5;i++){
        sem_init(&TrincoPessoaEstaSerAtendida[i], 0 ,1);
        sem_init(&TrincoPessoaChegouFila[i], 0 ,1);
        sem_init(&TrincoAtendimentoVIP[i],0,0);
        sem_init(&TrincoAtendimentoNormal[i],0,0);
        sem_init(&TrincoQuantidadePessoasEstacao[i],0,1);
    }

    //Criacao do metro
    criarMetro(0);
    criarMetro(1);

    //Inicializar Valores
    carregaDados();
}


/*###################################################*/
/*              Criacao do Socket                    */
/*###################################################*/
int criarSocket ( ) {
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


/*###################################################*/
/*                  Simulacao                        */
/*###################################################*/
void simulacao(int sockfd) {
    srand(time(NULL));
    inicializacao();

    //Envia mensagem de começo
    EnviaMensagem("X-X-0", sockfd);

    int zero = 0;
    int um = 1;

    pthread_create ( &IDTarefaMetro[0] , NULL , Metro, &zero);
    pthread_create ( &IDTarefaMetro[1] , NULL , Metro, &um );


    int i;
    for (i=0; i<numeroDePessoasMaximo; i++){
        pthread_create ( &IDtarefas[i] , NULL , Pessoa, NULL);
        //usleep(1000);
    }

    for (i=0; i<numeroDePessoasMaximo; i++){
        pthread_join ( IDtarefas[i] , NULL);
    }

    pthread_join ( IDTarefaMetro[0] , NULL);
    pthread_join ( IDTarefaMetro[1] , NULL);

    //Acaba a simulacao
    EnviaMensagem("X-X-1", sockfd);
}

/*###################################################*/
/*                      Main                         */
/*###################################################*/
int main ( int argc , char const * argv[] ) {
    sockfd = criarSocket();
    simulacao(sockfd);
    close(sockfd);
    return 1;
}
