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
struct centroTeste1 centroTeste1;
struct centroTeste2 centroTeste2;

//TRINCOS E SEMAFOROS
pthread_mutex_t mutexCriarPessoa;
pthread_mutex_t mutexFilaEspera;
pthread_mutex_t mutexEnviarMensagem;
sem_t semaforoMedicos;
sem_t semaforoDoentes;

//tarefas
pthread_t IDtarefa[TAMANHO_ARRAY_TAREFAS]; //pessos e enfermeiros

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
    pthread_mutex_lock(&mutexCriarPessoa);

    struct pessoa p;
    p.id = idPessoa;
    p.medico = FALSE;
    p.centroTeste = (rand() % 2) + 1;
    p.estadoTeste = NAO_TESTOU;
    p.idoso = probabilidade(configuracao.probabilidadeSerIdoso);

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
}

void FilaEspera(struct pessoa *pessoa)
{
    pthread_mutex_lock(&mutexFilaEspera);
    long timestamp = tempoDecorrido;
    if (pessoa->centroTeste == 1)
    {
        if (centroTeste1.numeroPessoasEspera < configuracao.tamanhoFilaCentro1)
        {
            char mensagem[TAMANHO_LINHA];
            sprintf(mensagem, "%d-%d-%d-%d", idPessoa, timestamp, 0, 1);
            enviarMensagem(mensagem);
            centroTeste1.numeroPessoasEspera++;
        }
    }
    else
    {
        if ((centroTeste2.numeroPessoasNormalEspera + centroTeste2.numeroPessoasPrioritariasEspera) < configuracao.tamanhoFilaCentro2)
        {
            char mensagem[TAMANHO_LINHA];
            sprintf(mensagem, "%d-%d-%d-%d", idPessoa, timestamp, 0, 2);
            enviarMensagem(mensagem);
            if (pessoa->idoso)
            {
                centroTeste2.numeroPessoasPrioritariasEspera++;
            }
            else
            {
                centroTeste2.numeroPessoasNormalEspera++;
            }
        }
    }
    pthread_mutex_unlock(&mutexFilaEspera);
}
void Medico(void *ptr)
{
    struct pessoa p = criaMedico();
    char mensagem[TAMANHO_LINHA];
    sprintf(mensagem, "%d-%d-%d-%d", idPessoa, tempoDecorrido, 11, "Z");
    enviarMensagem(mensagem);
}

/*---------------------------------------
            SIMULACAO
-----------------------------------------*/

void simulacao(char *filename)
{
    srand(time(NULL));
    iniciarSemaforosETrincos();

    carregarConfiguracao(filename);
    int timeStampAnterior = current_timestamp();
    int auxTimeStamp;
    int tempoLimite = configuracao.tempoSimulacao * DIA;
    enviarMensagem("Z-Z-0-Z"); //Mensagem que indica o comeco da simulacao
    int index = 0;

    for (index; index < configuracao.numeroMedicos; index++)
    {
        if (pthread_create(&IDtarefa[idPessoa], NULL, Medico, NULL))
        {
            printf(" Erro na criação da tarefa \n");
            exit(1);
        }
        usleep(50);
    }
    while (tempoDecorrido != tempoLimite)
    {
        auxTimeStamp = current_timestamp();
        if (auxTimeStamp != timeStampAnterior)
        {
            tempoDecorrido++;
            timeStampAnterior = auxTimeStamp;
            if (tempoDecorrido % (configuracao.tempoMedioChegada * MINUTO) == 0)
            {
                //cria tarefas pessoas
                if (pthread_create(&IDtarefa[idPessoa], NULL, Pessoa, NULL))
                {
                    printf(" Erro na criação da tarefa \n");
                    exit(1);
                }
            }
        }
    }

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
    sem_init(&centroTeste1.filaEspera, 0, configuracao.tamanhoFilaCentro1);
    sem_init(&centroTeste1.pontosTestagem, configuracao.numeroPontosTestagemCentro1, configuracao.numeroPontosTestagemCentro1);
    sem_init(&centroTeste2.filaEsperaNormal, 0, configuracao.tamanhoFilaCentro2);
    sem_init(&centroTeste2.filaEsperaPrioritario, 0, configuracao.tamanhoFilaCentro2);
    sem_init(&centroTeste2.pontosTestagem, configuracao.numeroPontosTestagemCentro2, configuracao.numeroPontosTestagemCentro2);
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
    configuracao.numeroPontosTestagemCentro1 = strtol(values[5], NULL, 10);
    configuracao.numeroPontosTestagemCentro2 = strtol(values[6], NULL, 10);
    configuracao.tamanhoHospital = strtol(values[7], NULL, 10);
    configuracao.numeroMedicos = configuracao.tamanhoHospital - 20;
    configuracao.probabilidadeSerIdoso = strtof(values[8], NULL);
    configuracao.probabilidadeMedicoPositivo = strtof(values[9], NULL);
    configuracao.probabilidadePopulacaoPositivo = strtof(values[10], NULL);
    configuracao.probabilidadeTesteNormalInconclusivo = strtof(values[11], NULL);
    configuracao.probabilidadeTesteRapidoInconclusivo = strtof(values[12], NULL);
    configuracao.probabilidadeNaoIdosoPrecisaHospital = strtof(values[13], NULL);
    configuracao.probabilidadeIdosoMorrer = strtof(values[14], NULL);
    configuracao.probabilidadeNaoIdosoMorrer = strtof(values[15], NULL);
    configuracao.tempoCurar = strtol(values[16], NULL, 10);
    configuracao.tempoSimulacao = strtol(values[17], NULL, 10);
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