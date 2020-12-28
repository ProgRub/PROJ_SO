// Grupo constituído por:
// 2046018 Rúben José Gouveia Rodrigues
// 2045218 Filipe Orlando Namora Gomes
// 2028616 José Alejandro Ferreira Gouveia

#include "config.h"

//VARIAVEIS GLOBAIS

int socketfd = 0; //socket
int idPessoa = 0;
struct Configuration configuracao; //configuracao da simulacao
long tempoDecorrido = 0;
int minutosDecorridos = 0;
struct centroTeste1 centroTestes1;
struct centroTeste2 centroTestes2;
int *tempoCooldownPontosTestagemCentro1;
int *tempoCooldownPontosTestagemCentro2;

//TRINCOS E SEMAFOROS
pthread_mutex_t mutexCriarPessoa;
pthread_mutex_t mutexFilaEspera;
pthread_mutex_t mutexEnviarMensagem;
pthread_mutex_t mutexVariaveisSimulacao;
sem_t semaforoMedicos;
sem_t semaforoDoentes;

//tarefas
pthread_t IDtarefa[TAMANHO_ARRAY_TAREFAS]; //pessoas e médicos
// phtread_t tarefaCentroTeste1;
// pthread_t tarefaCentroTeste2;

/*---------------------------------------
            SOCKETS
-----------------------------------------*/

int criaSocket()
{
    struct sockaddr_un end_serv;
    int tamanhoServidor;

    //Criar o socket
    socketfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socketfd < 0)
    {
        printf("Erro: socket nao foi criado \n");
    }

    //Zerar o socket
    bzero((char *)&end_serv, sizeof(end_serv));

    //Familia do socket
    end_serv.sun_family = AF_UNIX;
    strcpy(end_serv.sun_path, UNIXSTR_PATH);
    tamanhoServidor = strlen(end_serv.sun_path) + sizeof(end_serv.sun_family);

    // Estabelecer a ligacao com o socket
    int varprint = 0;
    while (connect(socketfd, (struct sockaddr *)&end_serv, tamanhoServidor) < 0)
    {
        if (varprint == 0)
        {
            printf("Espera pelo monitor...\n");
            varprint = 1;
        }
    }
    printf("Simulador pronto. Esperando pelo início da simulação...\n");
    return socketfd;
}

void enviarMensagem(char *mensagemAEnviar) //envia mensagem po monitor
{
    pthread_mutex_lock(&mutexEnviarMensagem);
    int numero;
    char mensagem[TAMANHO_LINHA];
    if (strcpy(mensagem, mensagemAEnviar) != 0)
    {
        numero = strlen(mensagem) + 1;
        if (write(socketfd, mensagem, numero) != numero)
        {
            printf("Erro no write!\n");
        }
    }

    usleep(750);
    pthread_mutex_unlock(&mutexEnviarMensagem);
}

/*---------------------------------------
            FUNCOES AUXILIARES
-----------------------------------------*/

int numeroAleatorio(int numeroMaximo, int numeroMinimo)
{
    return rand() % (numeroMaximo + 1 - numeroMinimo) + numeroMinimo;
}
int probabilidade(float prob)
{
    return numeroAleatorio(100, 0) < (prob * 100);
}
long long current_timestamp()
{
    struct timeval te;
    gettimeofday(&te, NULL);                                         // get current time
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}
void fazerTeste(struct pessoa *pessoa)
{
    float probabilidadeInconclusivo = 0.0;
    if (pessoa->tipoTeste == TESTE_NORMAL)
    {
        probabilidadeInconclusivo = configuracao.probabilidadeTesteNormalInconclusivo;
    }
    else
    {
        probabilidadeInconclusivo = configuracao.probabilidadeTesteRapidoInconclusivo;
    }
    if (probabilidade(probabilidadeInconclusivo))
    { //Teste é inconclusivo
        pessoa->estadoTeste = INCONCLUSIVO;
    }
    else
    {
        if (!pessoa->medico)
        { //se a pessoa a ser testada não é médica
            if (probabilidade(configuracao.probabilidadePopulacaoPositivo))
            { //pessoa testou positivo
                pessoa->estadoTeste = POSITIVO;
            }
            else
            {
                pessoa->estadoTeste = NEGATIVO;
            }
        }
        else
        { //se a pessoa a ser testada é médica
            if (probabilidade(configuracao.probabilidadeMedicoPositivo))
            { //médico testou positivo
                pessoa->estadoTeste = POSITIVO;
            }
            else
            {
                pessoa->estadoTeste = NEGATIVO;
            }
        }
    }
}

/*---------------------------------------
            PESSOA
-----------------------------------------*/

struct pessoa criaPessoa()
{
    pthread_mutex_lock(&mutexCriarPessoa);

    struct pessoa p;
    p.id = idPessoa;
    p.medico = FALSE;
    p.centroTeste = numeroAleatorio(2, 1);
    p.estadoTeste = NAO_TESTOU;
    p.tipoTeste = TESTE_NORMAL;
    p.idoso = probabilidade(configuracao.probabilidadeSerIdoso);
    if (p.centroTeste == 1)
    {
        p.numeroPessoasAFrenteParaDesistir = numeroAleatorio(configuracao.tamanhoFilaCentro1, configuracao.tamanhoFilaCentro1 / 4);
    }
    else
    {
        p.numeroPessoasAFrenteParaDesistir = numeroAleatorio(configuracao.tamanhoFilaCentro2, configuracao.tamanhoFilaCentro2 / 4);
    }
    p.tempoMaximoEspera = numeroAleatorio(configuracao.tempoMaximoEsperaMedio, configuracao.tempoMaximoEsperaMedio / 4);
    p.estado = ESPERA;

    printf("Criado Pessoa %d: \n", idPessoa);
    idPessoa++;
    pthread_mutex_unlock(&mutexCriarPessoa);
    return p;
}
struct pessoa criaMedico()
{
    pthread_mutex_lock(&mutexCriarPessoa);
    struct pessoa m;
    m.id = idPessoa;
    m.medico = TRUE;
    m.idoso = FALSE;
    m.tipoTeste = TESTE_NORMAL;
    m.centroTeste = CENTRO_PRIORITARIO;
    m.estadoTeste = NAO_TESTOU;

    printf("Criado Medico %d: \n", idPessoa);
    idPessoa++;
    pthread_mutex_unlock(&mutexCriarPessoa);
    return m;
}

void Pessoa(void *ptr)
{
    struct pessoa pessoa = criaPessoa();
    FilaEspera(&pessoa);

    int tipoTeste = -1;
    int tempoEsperaTeste = 0;
    if (centroTestes1.numeroPessoasEspera > configuracao.tamanhoFilaCentro1 - 5)
    {
        tempoEsperaTeste = configuracao.tempoTesteRapido * HORA;
    }
    else
    {
        tempoEsperaTeste = configuracao.tempoTesteNormal * HORA;
    }
    usleep(tempoEsperaTeste);
    fazerTeste(&pessoa);
}

void FilaEspera(struct pessoa *pessoa)
{
    pthread_mutex_lock(&mutexFilaEspera);
    char mensagem[TAMANHO_LINHA];
    long timestamp = minutosDecorridos;
    if (pessoa->centroTeste == 1)
    {
        if (centroTestes1.numeroPessoasEspera < configuracao.tamanhoFilaCentro1)
        {
            printf("A pessoa com o id %d chegou a fila.\n", pessoa->id);
            sprintf(mensagem, "%d-%d-%d-%d", idPessoa, timestamp, 0, 1);
            enviarMensagem(mensagem);
            if (pessoa->numeroPessoasAFrenteParaDesistir < centroTestes1.numeroPessoasEspera)
            {
                printf("A pessoa com o id %d desistiu fila 1.\n", pessoa->id);
                pthread_mutex_unlock(&mutexFilaEspera);
                sprintf(mensagem, "%d-%d-%d-%d", idPessoa, timestamp, 2, 1);
                enviarMensagem(mensagem);
                return;
            }
            pessoa->tempoChegadaFilaEspera = timestamp;
            centroTestes1.numeroPessoasEspera++;
            if (centroTestes1.numeroPessoasEspera > configuracao.tamanhoFilaCentro1 - 5)
            {
                pessoa->tipoTeste = TESTE_RAPIDO;
            }
            pthread_mutex_unlock(&mutexFilaEspera);
            //printf("A pessoa com o id %d chegou1 a fila.\n", pessoa->id);
            sem_wait(&centroTestes1.filaEspera);
            printf("A pessoa com o id %d vai ser testada quando houver um posto livre no centro 1.\n", pessoa->id);
            sem_wait(&centroTestes1.pontosTestagem);
            printf("A pessoa com o id %d foi testada.\n", pessoa->id);
            pthread_mutex_lock(&mutexVariaveisSimulacao);
            if (minutosDecorridos - pessoa->tempoChegadaFilaEspera > pessoa->tempoMaximoEspera)
            { //passou muito tempo à espera, a pessoa desiste
                pthread_mutex_unlock(&mutexVariaveisSimulacao);
                sprintf(mensagem, "%d-%d-%d-%d", idPessoa, timestamp, 2, 1);
                enviarMensagem(mensagem);
                pthread_mutex_lock(&mutexVariaveisSimulacao);
            }
            else
            { //não desiste, vai ser testada
                pthread_mutex_unlock(&mutexVariaveisSimulacao);
                sprintf(mensagem, "%d-%d-%d-%d", idPessoa, timestamp, 1, 1);
                enviarMensagem(mensagem);
                pthread_mutex_lock(&mutexVariaveisSimulacao);
                int index = 0;
                for (index; index < configuracao.numeroPontosTestagemCentro1; index++)
                {
                    printf("%d\n", *(tempoCooldownPontosTestagemCentro1 + index));
                    if (*(tempoCooldownPontosTestagemCentro1 + index) == -1) //ponto testagem está livre, começa-se o cooldown
                    {
                        *(tempoCooldownPontosTestagemCentro1 + index) = configuracao.tempoCooldownPontosTestagem;
                        printf("%d\n", *(tempoCooldownPontosTestagemCentro1 + index));
                        break;
                    }
                }
            }
            pthread_mutex_unlock(&mutexVariaveisSimulacao);
        }
    }
    else
    {
        if ((centroTestes2.numeroPessoasNormalEspera + centroTestes2.numeroPessoasPrioritariasEspera) < configuracao.tamanhoFilaCentro2)
        {
            sprintf(mensagem, "%d-%d-%d-%d", idPessoa, timestamp, 0, 2);
            enviarMensagem(mensagem);
            if (pessoa->idoso)
            {
                centroTestes2.numeroPessoasPrioritariasEspera++;
                pthread_mutex_unlock(&mutexFilaEspera);
                sem_wait(&centroTestes2.filaEsperaPrioritario);
            }
            else
            {
                centroTestes2.numeroPessoasNormalEspera++;
                pthread_mutex_unlock(&mutexFilaEspera);
                sem_wait(&centroTestes2.filaEsperaNormal);
            }
            printf("A pessoa com o id %d chegou a fila2.\n", pessoa->id);
            sprintf(mensagem, "%d-%d-%d-%d", idPessoa, timestamp, 0, 2);
            enviarMensagem(mensagem);
            if (pessoa->numeroPessoasAFrenteParaDesistir < centroTestes1.numeroPessoasEspera)
            {
                printf("A pessoa com o id %d desistiu fila 2.\n", pessoa->id);
                pthread_mutex_unlock(&mutexFilaEspera);
                sprintf(mensagem, "%d-%d-%d-%d", idPessoa, timestamp, 2, 2);
                enviarMensagem(mensagem);
                return;
            }
            pessoa->tempoChegadaFilaEspera = timestamp;
            centroTestes1.numeroPessoasEspera++;
            if (centroTestes1.numeroPessoasEspera > configuracao.tamanhoFilaCentro2 - 5)
            {
                pessoa->tipoTeste = TESTE_RAPIDO;
            }
            pthread_mutex_unlock(&mutexFilaEspera);
            //printf("A pessoa com o id %d chegou1 a fila2.\n", pessoa->id);
            sem_wait(&centroTestes1.filaEspera);
            printf("A pessoa com o id %d vai ser testada quando houver um posto livre no centro 2.\n", pessoa->id);
            sem_wait(&centroTestes1.pontosTestagem);
            printf("A pessoa com o id %d foi testada.\n", pessoa->id);
            pthread_mutex_lock(&mutexVariaveisSimulacao);
            //MANDAR minutosDecorridos - pessoa->tempoChegadaFilaEspera EM VEZ DE TIMESTAMP
            if (minutosDecorridos - pessoa->tempoChegadaFilaEspera > pessoa->tempoMaximoEspera)
            { //passou muito tempo à espera, a pessoa desiste
                pthread_mutex_unlock(&mutexVariaveisSimulacao);
                sprintf(mensagem, "%d-%d-%d-%d", idPessoa, timestamp, 2, 2);
                enviarMensagem(mensagem);
                pthread_mutex_lock(&mutexVariaveisSimulacao);
            }
            else
            { //não desiste, vai ser testada
                pthread_mutex_unlock(&mutexVariaveisSimulacao);
                sprintf(mensagem, "%d-%d-%d-%d", idPessoa, timestamp, 1, 2);
                enviarMensagem(mensagem);
                pthread_mutex_lock(&mutexVariaveisSimulacao);
                int index = 0;
                for (index; index < configuracao.numeroPontosTestagemCentro2; index++)
                {
                    printf("%d\n", *(tempoCooldownPontosTestagemCentro2 + index));
                    if (*(tempoCooldownPontosTestagemCentro2 + index) == -1) //ponto testagem está livre, começa-se o cooldown
                    {
                        *(tempoCooldownPontosTestagemCentro2 + index) = configuracao.tempoCooldownPontosTestagem;
                        printf("%d\n", *(tempoCooldownPontosTestagemCentro2 + index));
                        break;
                    }
                }
            }
            pthread_mutex_unlock(&mutexVariaveisSimulacao);
        }
    }
    pthread_mutex_unlock(&mutexFilaEspera);
}
void Medico(void *ptr)
{
    struct pessoa p = criaMedico();
    char mensagem[TAMANHO_LINHA];
    sprintf(mensagem, "%d-%d-%d-%d", idPessoa, minutosDecorridos, 11, "Z");
    enviarMensagem(mensagem);
}

/*---------------------------------------
            CENTROS TESTE
-----------------------------------------*/
// void criarCentro1()
// {
// }

// void criarCentro2()
// {
// }

// void CentroTestes1()
// {
//     while (TRUE)
//     {
//         sem_wait(&semaforoCentroTestes1TestarPessoa);
//         int index;
//         // int pontoLivre = FALSE;
//         for (index = 0; index < configuracao.numeroPontosTestagemCentro1; index++)
//         {
//             if (*(centroTestes1.tempoCooldownPontosTestagem + index) == 0) //ponto testagem está livre
//             {
//                 sem_post(&centroTestes1.filaEspera);
//                 // sem_wait(centroTestes1.pontosTestagem + index);
//                 // pontoLivre = TRUE;
//                 *(centroTestes1.tempoCooldownPontosTestagem + index) = configuracao.tempoCooldownPontosTestagem;
//                 break;
//             }
//         }
//     }
// }

// void CentroTestes2()
// {
//     while (TRUE)
//     {
//         sem_wait(&semaforoCentroTestes2TestarPessoa);
//     }
// }

/*---------------------------------------
            SIMULACAO
-----------------------------------------*/

void simulacao(char *filename)
{
    srand(time(NULL));
    carregarConfiguracao(filename);
    iniciarSemaforosETrincos();
    int timeStampAnterior = current_timestamp();
    int auxTimeStamp;
    int tempoLimite = configuracao.tempoSimulacao * DIA;
    enviarMensagem("Z-Z-0-Z"); //Mensagem que indica o comeco da simulacao
    int index = 0;

    for (index; index < configuracao.numeroMedicos; index++)
    {
        if (pthread_create(&IDtarefa[idPessoa], NULL, Medico, NULL))
        {
            printf("Erro na criação da tarefa\n");
            exit(1);
        }
        usleep(50);
    }
    while (tempoDecorrido != tempoLimite)
    {
        auxTimeStamp = current_timestamp();
        if (auxTimeStamp != timeStampAnterior)
        {
            // printf("%d %d\n",tempoDecorrido,tempoLimite);
            tempoDecorrido++;
            timeStampAnterior = auxTimeStamp;
            if (tempoDecorrido % MINUTO == 0)
            { //passou um minuto na simulação
                pthread_mutex_lock(&mutexVariaveisSimulacao);
                minutosDecorridos++;
                int index = 0;
                for (index; index < configuracao.numeroPontosTestagemCentro1; index++)
                {
                    if (*(tempoCooldownPontosTestagemCentro1 + index) == 0)
                    {
                        sem_post(&centroTestes1.pontosTestagem);
                        (*(tempoCooldownPontosTestagemCentro1 + index))--;
                    }
                    else if (*(tempoCooldownPontosTestagemCentro1 + index) > 0)
                    {
                        (*(tempoCooldownPontosTestagemCentro1 + index))--;
                    }
                }
                index = 0;
                for (index; index < configuracao.numeroPontosTestagemCentro2; index++)
                {
                    if (*(tempoCooldownPontosTestagemCentro2 + index) > 0)
                    {
                        sem_post(&centroTestes2.pontosTestagem);
                    }
                    else if (*(tempoCooldownPontosTestagemCentro1 + index) > 0)
                    {
                        (*(tempoCooldownPontosTestagemCentro2 + index))--;
                    }
                }
                if (minutosDecorridos % configuracao.tempoMedioChegada == 0)
                {
                    //cria tarefas pessoas
                    pthread_mutex_unlock(&mutexVariaveisSimulacao);
                    if (pthread_create(&IDtarefa[idPessoa], NULL, Pessoa, NULL))
                    {
                        printf("Erro na criação da tarefa\n");
                        exit(1);
                    }
                    pthread_mutex_lock(&mutexVariaveisSimulacao);
                }
                pthread_mutex_unlock(&mutexVariaveisSimulacao);
            }
        }
    }
    // printf("FIM %d %d\n",tempoDecorrido,tempoLimite);

    // for (int i = 0; i < TAMANHO_ARRAY_TAREFAS; i++)
    // {
    //     pthread_join(IDtarefa[i], NULL);
    // }

    enviarMensagem("Z-Z-1-Z"); //Mensagem que indica o fim da simulacao
}

void iniciarSemaforosETrincos()
{
    if (pthread_mutex_init(&mutexCriarPessoa, NULL) != 0)
    {
        printf("Inicializacao do trinco falhou.\n");
    }
    if (pthread_mutex_init(&mutexFilaEspera, NULL) != 0)
    {
        printf("Inicializacao do trinco falhou.\n");
    }
    if (pthread_mutex_init(&mutexEnviarMensagem, NULL) != 0)
    {
        printf("Inicializacao do trinco falhou.\n");
    }
    if (pthread_mutex_init(&mutexVariaveisSimulacao, NULL) != 0)
    {
        printf("Inicializacao do trinco falhou.\n");
    }

    // sem_init(&centroTestes2.pontosTestagem, configuracao.numeroPontosTestagemCentro2, configuracao.numeroPontosTestagemCentro2);
    sem_init(&centroTestes1.filaEspera, 0, configuracao.tamanhoFilaCentro1);
    sem_init(&centroTestes1.pontosTestagem, 0, configuracao.numeroPontosTestagemCentro1);
    // sem_init(&centroTestes1.pontosTestagem, configuracao.numeroPontosTestagemCentro1, configuracao.numeroPontosTestagemCentro1);
    // centroTestes1.pontosTestagem = (sem_t *)calloc(configuracao.numeroPontosTestagemCentro1, sizeof(sem_t));
    tempoCooldownPontosTestagemCentro1 = (int *)calloc(configuracao.numeroPontosTestagemCentro1, sizeof(int));
    int index = 0;
    for (index; index < configuracao.numeroPontosTestagemCentro1; index++)
    {
        *(tempoCooldownPontosTestagemCentro1 + index) = -1;
    }

    sem_init(&centroTestes2.filaEsperaNormal, 0, configuracao.tamanhoFilaCentro2);
    sem_init(&centroTestes2.filaEsperaPrioritario, 0, configuracao.tamanhoFilaCentro2);
    sem_init(&centroTestes2.pontosTestagem, 0, configuracao.numeroPontosTestagemCentro2);
    // centroTestes2.pontosTestagem = (sem_t *)calloc(configuracao.numeroPontosTestagemCentro2, sizeof(sem_t));
    tempoCooldownPontosTestagemCentro2 = (int *)calloc(configuracao.numeroPontosTestagemCentro2, sizeof(int));
    index = 0;
    for (index; index < configuracao.numeroPontosTestagemCentro2; index++)
    {
        // sem_init(centroTestes2.pontosTestagem + index, 0, 1);
        *(tempoCooldownPontosTestagemCentro2 + index) = -1;
    }

    sem_init(&semaforoMedicos, 0, configuracao.numeroMedicos);
    sem_init(&semaforoDoentes, 0, configuracao.tamanhoHospital);
}

void carregarConfiguracao(char nomeFicheiro[])
{
    FILE *configFile;
    configFile = fopen(nomeFicheiro, "r");
    if (configFile == NULL)
    {
        perror("ERRO!\nFicheiro de configuração não existe.");
        exit(EXIT_FAILURE);
    }
    else if (strcmp(nomeFicheiro, "simulador.conf") != 0) //se strcmp retorn 0, então os string são iguais
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
    char *values[TAMANHO_CONFIGURACAO];
    for (int index = 0; index < TAMANHO_CONFIGURACAO; index++)
    {
        char *aux = strtok(lines[index], ":");
        i = 0;
        while (aux != NULL)
        {
            array[i++] = aux;
            aux = strtok(NULL, ":");
        }
        values[index] = array[1];
    }

    //guardar os valores do ficheiro na estrutura de configuracao
    configuracao.tempoMedioChegada = (int)strtol(values[0], NULL, 10);
    configuracao.tempoTesteNormal = strtol(values[1], NULL, 10);
    configuracao.tempoTesteRapido = strtol(values[2], NULL, 10);
    configuracao.tempoCooldownPontosTestagem = strtol(values[3], NULL, 10);
    configuracao.tempoMaximoEsperaMedio = strtol(values[4], NULL, 10);
    configuracao.tamanhoFilaCentro1 = strtol(values[5], NULL, 10);
    configuracao.tamanhoFilaCentro2 = strtol(values[6], NULL, 10);
    configuracao.numeroPontosTestagemCentro1 = strtol(values[7], NULL, 10);
    configuracao.numeroPontosTestagemCentro2 = strtol(values[8], NULL, 10);
    configuracao.tamanhoHospital = strtol(values[9], NULL, 10);
    configuracao.numeroMedicos = configuracao.tamanhoHospital - 20;
    configuracao.probabilidadeSerIdoso = strtof(values[10], NULL);
    configuracao.probabilidadeMedicoPositivo = strtof(values[11], NULL);
    configuracao.probabilidadePopulacaoPositivo = strtof(values[12], NULL);
    configuracao.probabilidadeTesteNormalInconclusivo = strtof(values[13], NULL);
    configuracao.probabilidadeTesteRapidoInconclusivo = strtof(values[14], NULL);
    configuracao.probabilidadeNaoIdosoPrecisaHospital = strtof(values[15], NULL);
    configuracao.probabilidadeIdosoMorrer = strtof(values[16], NULL);
    configuracao.probabilidadeNaoIdosoMorrer = strtof(values[17], NULL);
    configuracao.tempoCurar = strtol(values[18], NULL, 10);
    configuracao.tempoSimulacao = strtol(values[19], NULL, 10);
    // printf("Tempo medio de chegada: %d\n", configuracao.tempoMedioChegada);
    // printf("Tempo ate o resultado do teste normal: %d\n", configuracao.tempoTesteNormal);
    // printf("Tempo ate o resultado do teste rapido: %d\n", configuracao.tempoTesteRapido);
    // printf("Tempo de cooldown dos pontos de testagem: %d\n", configuracao.tempoCooldownPontosTestagem);
    // printf("Tempo maximo de espera medio: %d\n", configuracao.tempoMaximoEsperaMedio);
    // printf("Tamanho maximo da fila no centro de testes 1: %d\n", configuracao.tamanhoFilaCentro1);
    // printf("Tamanho maximo da fila no centro de testes 2: %d\n", configuracao.tamanhoFilaCentro2);
    // printf("Numero de pontos de testagem no centro de testes 1: %d\n", configuracao.numeroPontosTestagemCentro1);
    // printf("Numero de pontos de testagem no centro de testes 2: %d\n", configuracao.numeroPontosTestagemCentro2);
    // printf("Tamanho maximo do hospital: %d\n", configuracao.tamanhoHospital);
    // printf("Probabilidade de ser idoso: %f\n", configuracao.probabilidadeSerIdoso);
    // printf("Probabilidade do teste do medico dar positivo; %f\n", configuracao.probabilidadeMedicoPositivo);
    // printf("Probabilidade do teste a populacao dar positivo: %f\n", configuracao.probabilidadePopulacaoPositivo);
    // printf("Probabilidade do teste normal dar inconclusivo: %f\n", configuracao.probabilidadeTesteNormalInconclusivo);
    // printf("Probabilidade do teste rapido dar inconclusivo: %f\n", configuracao.probabilidadeTesteRapidoInconclusivo);
    // printf("Probabilidade de um nao idoso precisar de hospital: %f\n", configuracao.probabilidadeNaoIdosoPrecisaHospital);
    // printf("Probabilidade de um idoso morrer: %f\n", configuracao.probabilidadeIdosoMorrer);
    // printf("Probabilidade do um nao idoso morrer: %f\n", configuracao.probabilidadeNaoIdosoMorrer);
    // printf("Tempo para curar: %d\n", configuracao.tempoCurar);
    // printf("Duracao da simulacao: %d\n", configuracao.tempoSimulacao);
}

int main(int argc, char *argv[])
{

    if (argc == 2)
    {
        socketfd = criaSocket();
        simulacao(argv[1]);
        close(socketfd);
        return 0;
    }
    else
    {
        perror("ERRO!\n(Só) É preciso passar como argumento o ficheiro de configuração.");
        exit(EXIT_FAILURE);
    }
}