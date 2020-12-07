// Grupo constituído por:
// 2046018 Rúben José Gouveia Rodrigues
// 2045218 Filipe Orlando Namora Gomes
// 2028616 José Alejandro Ferreira Gouveia

#include "config.h"

//VARIAVEIS GLOBAIS

int socketfd = 0; //socket
int idPessoa = 0;
struct Configuration configuracao; //configuracao da simulacao

struct timeval tv;

// int numeroPessoas = 0;
// int numeroCasosPositivos = 0;
// int numeroCasosAtivos = 0;
// int numeroDoentesNoHospital = 0;

//TRINCOS E SEMAFOROS
pthread_mutex_t mutexCriarPessoa;
pthread_mutex_t mutexEnviarMensagem;
sem_t filaEsperaCentro1;
sem_t filaEsperaCentro2;
sem_t semaforoMedicos;
sem_t semaforoDoentes;

//tarefas 
pthread_t IDtarefa[TAMANHO_ARRAY_TAREFAS]; //pessos e enfermeiros 
pthread_t IDtarefaCentro[2];

/*---------------------------------------
            SOCKETS
-----------------------------------------*/

int criaSocket()
{
    //Variaveis
    struct sockaddr_un end_serv;
    int tamanhoServidor;

    //Cria o socket
    socketfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socketfd < 0)
    {
        printf("Erro: socket nao foi criado \n");
    }

    //Colocar o socket a zero
    bzero((char *)&end_serv, sizeof(end_serv));

    //Dominio do socket
    end_serv.sun_family = AF_UNIX;
    strcpy(end_serv.sun_path, UNIXSTR_PATH);
    tamanhoServidor = strlen(end_serv.sun_path) + sizeof(end_serv.sun_family);

    // Estabelece a ligacao com o socket
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

    usleep(300);
    pthread_mutex_unlock(&mutexEnviarMensagem);
}

/*---------------------------------------
            FUNCOES AUXILIARES
-----------------------------------------*/

int probabilidade(float prob)
{
    return (rand() % 100) < (prob * 100);
}
long long current_timestamp()
{
    struct timeval te;
    gettimeofday(&te, NULL);                                         // get current time
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}
/*---------------------------------------
            PESSOA
-----------------------------------------*/

struct pessoa criaPessoa()
{
    int valores[2] = {0, 1};

    pthread_mutex_lock(&mutexCriarPessoa);
    int valorRandomCentroTeste = rand() % 2;
    int valorRandomIdoso = rand() % 2;

    struct pessoa p;
    p.id = idPessoa;
    p.medico = FALSE;
    p.centroTeste = valorRandomCentroTeste;
    p.estadoTeste = NAO_TESTOU;
    p.idoso = probabilidade(configuracao.probabilidadeSerIdoso);

    p.estado = ESPERA;

    printf("Criado Pessoa %d: \n", IDtarefa[idPessoa]);
    char mensagem[TAMANHO_LINHA];
    sprintf(mensagem, "%d-%d-%d-%d", IDtarefa[idPessoa], 0, 0, p.centroTeste);
    enviarMensagem(mensagem);
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
    m.centroTeste = CENTRO_PRIORITARIO;
    m.estadoTeste = NAO_TESTOU;

    printf("Criado Medico %d: \n", IDtarefa[idPessoa]);
    char mensagem[TAMANHO_LINHA];
    sprintf(mensagem, "%d-%d-%d", IDtarefa[idPessoa], 0, 12);
    enviarMensagem(mensagem);
    idPessoa++;
    pthread_mutex_unlock(&mutexCriarPessoa);
    return m;
}

void Pessoa(void *ptr)
{
    struct pessoa p = criaPessoa();
}
void Medico(void *ptr)
{
    struct pessoa p = criaMedico();
}

void Centro(void *ptr)
{
}

/*---------------------------------------
            SIMULACAO
-----------------------------------------*/

void simulacao(char *filename)
{
    srand(time(NULL));
    iniciarSemaforosETrincos();

    carregarConfiguracao(filename);
    // printf("Tempo medio de chegada: %d\n", configuracao.tempoMedioChegada);
    // printf("Tempo ate o resultado do teste normal: %d\n", configuracao.tempoTesteNormal);
    // printf("Tempo ate o resultado do teste rapido: %d\n", configuracao.tempoTesteRapido);
    // printf("Tempo de espera no centro de testes 1: %d\n", configuracao.tempoEsperaCentro1);
    // printf("Tempo de espera no centro de testes 2: %d\n", configuracao.tempoEsperaCentro2);
    // printf("Tamanho maximo da fila no centro de testes 1: %d\n", configuracao.tamanhoFilaCentro1);
    // printf("Tamanho maximo da fila no centro de testes 2: %d\n", configuracao.tamanhoFilaCentro2);
    // printf("Tamanho maximo do hospital: %d\n", configuracao.tamanhoHospital);
    // printf("Probabilidade de um teste regressar positivo: %f\n", configuracao.probabilidadePositivo);
    // printf("Probabilidade do teste normal dar falso positivo: %f\n", configuracao.probabilidadeTesteNormalFalsoPositivo);
    // printf("Probabilidade do teste rapido dar falso positivo: %f\n", configuracao.probabilidadeTesteRapidoFalsoPositivo);
    // printf("Duracao da simulacao: %d\n", configuracao.tempoSimulacao);

    long tempoDecorrido = 0;
    int timeStampAnterior = current_timestamp();
    int auxTimeStamp;
    int tempoLimite = configuracao.tempoSimulacao * 1000;
    enviarMensagem("Z-Z-0"); //Mensagem que indica o comeco da simulacao
    int index = 0;

    //cria tarefas centro = 2
    /*
    for (int i = 0; i < 2; i++)
    {
        if (pthread_create (&IDtarefaCentro[i], NULL, Centro, NULL))
        {
            printf(" Erro na criação da tarefa \n");
            exit(1);
        }
        else
        {
            printf("Criada a tarefa %d\n", IDtarefaCentro[i]);
        }
    }*/

    for (index; index < configuracao.numeroMedicos; index++)
    {
        if (pthread_create(&IDtarefa[index], NULL, Medico, NULL))
        {
            printf(" Erro na criação da tarefa \n");
            exit(1);
        }
    }
    while (tempoDecorrido != tempoLimite)
    {
        auxTimeStamp = current_timestamp();
        if (auxTimeStamp != timeStampAnterior)
        {
            tempoDecorrido++;
            timeStampAnterior = auxTimeStamp;
            // printf("CHEGOU\n");
        }
        // printf("tempo: %d\n", tempoDecorrido);

        if (tempoDecorrido % configuracao.tempoMedioChegada == 0)
        {
                //cria tarefas pessoas
            if (pthread_create(&IDtarefa[index], NULL, Pessoa, NULL))
             {
                printf(" Erro na criação da tarefa \n");
                exit(1);
            }
        }
    }

    for (int i = 0; i < TAMANHO_ARRAY_TAREFAS; i++)
    {
        pthread_join(IDtarefa[i], NULL);
    }

    //pthread_join(IDtarefaCentro[0], NULL);
    //pthread_join(IDtarefaCentro[1], NULL);

    enviarMensagem("Z-Z-1"); //Mensagem que indica o fim da simulacao
}

void iniciarSemaforosETrincos()
{
    if (pthread_mutex_init(&mutexCriarPessoa, NULL) != 0)
    {
        printf("Inicializacao do trinco falhou.\n");
    }
    sem_init(&filaEsperaCentro1, 0, configuracao.tamanhoFilaCentro1);
    sem_init(&filaEsperaCentro2, 0, configuracao.tamanhoFilaCentro2);
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
    configuracao.tamanhoFilaCentro1 = strtol(values[3], NULL, 10);
    configuracao.tamanhoFilaCentro2 = strtol(values[4], NULL, 10);
    configuracao.tamanhoHospital = strtol(values[5], NULL, 10);
    configuracao.numeroMedicos = strtof(values[6], NULL);
    configuracao.probabilidadeSerIdoso = strtof(values[7], NULL);
    configuracao.probabilidadeMedicoPositivo = strtof(values[8], NULL);
    configuracao.probabilidadePopulacaoPositivo = strtof(values[9], NULL);
    configuracao.probabilidadeTesteNormalInconclusivo = strtof(values[10], NULL);
    configuracao.probabilidadeTesteRapidoInconclusivo = strtof(values[11], NULL);
    configuracao.probabilidadeNaoIdosoPrecisaHospital = strtof(values[12], NULL);
    configuracao.probabilidadeIdosoMorrer = strtof(values[13], NULL);
    configuracao.probabilidadeNaoIdosoMorrer = strtof(values[14], NULL);
    configuracao.tempoCurar = strtol(values[15], NULL, 10);
    configuracao.tempoSimulacao = strtol(values[16], NULL, 10);
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