// Grupo constituído por:
// 2046018 Rúben José Gouveia Rodrigues
// 2045218 Filipe Orlando Namora Gomes
// 2028616 José Alejandro Ferreira Gouveia

#include "config.h"

// VARIAVEIS GLOBAIS

int socketfd = 0; // socket
int idPessoa = 0;
struct Configuration configuracao; // configuracao da simulacao
long tempoDecorrido = 0;
int minutosDecorridos = 0;
struct centroTeste1 centroTestes1;
struct centroTeste2 centroTestes2;
int *tempoCooldownPontosTestagemCentro1;
int *tempoCooldownPontosTestagemCentro2;
int idososTestadosConsecutivamente = 0;
int numeroPacientesNoHospital = 0;
int casosEmEstudo = 0, casosPositivosAtivos = 0, casosPositivosTotal = 0, casosRecuperados = 0, numeroMortos = 0, pessoasEmIsolamento = 0, medicosDisponiveis = 0, numeroPessoasTestadas = 0, somaTemposEspera = 0,
    tempoMedioEspera = 0;

// TRINCOS E SEMAFOROS
pthread_mutex_t mutexCriarPessoa;
pthread_mutex_t mutexVariaveisCentros;
sem_t semaforoEnviarMensagem;
pthread_mutex_t mutexVariaveisSimulacao;
pthread_mutex_t mutexVariaveisHospital;
sem_t mutexVariaveisMonitor;
sem_t mutexMensagens;
sem_t semaforoMedicos;
sem_t semaforoDoentes;

// tarefas
pthread_t IDtarefa[TAMANHO_ARRAY_TAREFAS]; // pessoas e médicos
struct pessoa *PessoasCriadas[TAMANHO_ARRAY_TAREFAS];
int *IDsMedicosASerUsados;
int *IDsDoentesNoHospital;

/*---------------------------------------
            SOCKETS
-----------------------------------------*/

int criaSocket() {
    struct sockaddr_un end_serv;
    int tamanhoServidor;

    // Criar o socket
    socketfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socketfd < 0) {
        printf("Erro: socket nao foi criado!\n");
    }

    // Zerar o socket
    bzero((char *)&end_serv, sizeof(end_serv));

    // Familia do socket
    end_serv.sun_family = AF_UNIX;
    strcpy(end_serv.sun_path, UNIXSTR_PATH);
    tamanhoServidor = strlen(end_serv.sun_path) + sizeof(end_serv.sun_family);

    // Estabelecer a ligacao com o socket
    int varprint = 0;
    while (connect(socketfd, (struct sockaddr *)&end_serv, tamanhoServidor) < 0) {
        if (varprint == 0) {
            printf("Espera pelo monitor...\n");
            varprint = 1;
        }
    }
    printf("Simulador pronto. Esperando pelo início da simulação...\n");
    return socketfd;
}

void enviarMensagem(char *mensagemAEnviar) // envia mensagem po monitor
{
    int numero;
    char mensagem[TAMANHO_LINHA];
    if (strcpy(mensagem, mensagemAEnviar) != 0) {
        numero = strlen(mensagem) + 1;
        if (write(socketfd, mensagem, numero) != numero) {
            printf("Erro no write!\n");
        }
    }
}

/*---------------------------------------
            FUNCOES AUXILIARES
-----------------------------------------*/

int numeroAleatorio(int numeroMaximo,
                    int numeroMinimo) { // Retorna um numero aleatorio entre
                                        // numero minimo e numero maximo
    return rand() % (numeroMaximo + 1 - numeroMinimo) + numeroMinimo;
}
int probabilidade(float prob) { // Função probabilidade, retorna true ou false
    return numeroAleatorio(100, 0) < (prob * 100);
}
long long current_timestamp() {
    struct timeval te;
    gettimeofday(&te, NULL);                                         // get current time
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
    return milliseconds;
}
void fazerTeste(struct pessoa *pessoa) { // Função atribui resultado do teste
    float probabilidadeInconclusivo = 0.0;
    if (pessoa->tipoTeste == TESTE_NORMAL) { // Verifica qual o tipo de testes (Teste normal ou rapido)
        probabilidadeInconclusivo = configuracao.probabilidadeTesteNormalInconclusivo;
    } else {
        probabilidadeInconclusivo = configuracao.probabilidadeTesteRapidoInconclusivo;
    }
    if (probabilidade(probabilidadeInconclusivo)) {                           // Verifica se o teste for inconclusivo
        pessoa->estadoTeste = INCONCLUSIVO;                                   // Atribui o estadoTeste inconclusivo
    } else {                                                                  // Senão verifica se é medico ou não e faz uma probabilidade para
                                                                              // decidir se o teste é positivo ou negativo
        if (!pessoa->medico) {                                                // se a pessoa a ser testada não é médica
            if (probabilidade(configuracao.probabilidadePopulacaoPositivo)) { // pessoa testou positivo
                pessoa->estadoTeste = POSITIVO;
            } else {
                pessoa->estadoTeste = NEGATIVO;
            }
        } else {                                                           // se a pessoa a ser testada é médica
            if (probabilidade(configuracao.probabilidadeMedicoPositivo)) { // médico testou positivo
                pessoa->estadoTeste = POSITIVO;
            } else {
                pessoa->estadoTeste = NEGATIVO;
            }
        }
    }
}
char *printTipoPessoa(struct pessoa *pessoa) { // Função para escrever o tipo de pessoa Medico
                                               // (Idoso ou normal) ou Pessoa (Idoso ou normal)
    char *imprimir = malloc(35);
    sprintf(imprimir, "%s %s com o id %d", (pessoa->medico ? "O medico" : "A pessoa"), (pessoa->idoso ? (pessoa->medico ? "idoso" : "idosa") : "normal"), pessoa->id);
    return imprimir;
}

/*---------------------------------------
            PESSOA
-----------------------------------------*/

struct pessoa criaPessoa() { // Cria a pessoa

    struct pessoa p;
    p.id = idPessoa;
    p.medico = FALSE;
    p.centroTeste = numeroAleatorio(2, 1);
    p.estadoTeste = NAO_TESTOU;
    p.tipoTeste = TESTE_NORMAL;
    p.idoso = probabilidade(configuracao.probabilidadeSerIdoso);
    if (p.idoso) {
        p.centroTeste = probabilidade(0.75) + 1; // pessoas idosas preferem o centro de testes 2, o prioritário, probabilidade tenderá a dar 1+1=2
    } else {
        p.centroTeste = probabilidade(0.25) + 1; // pessoas normais preferem o centro de testes 1, probabilidade tenderá a dar 0 + 1 = centro de testes 1
    }
    p.desistiu = FALSE;
    if (p.centroTeste == 1) {
        p.numeroPessoasAFrenteParaDesistir = numeroAleatorio(configuracao.tamanhoFilaCentro1, (configuracao.tamanhoFilaCentro1 * 3) / 4);
    } else {
        p.numeroPessoasAFrenteParaDesistir = numeroAleatorio(configuracao.tamanhoFilaCentro2, (configuracao.tamanhoFilaCentro2 * 3) / 4);
    }
    p.tempoMaximoEspera = numeroAleatorio(configuracao.tempoMaximoEsperaMedio, (configuracao.tempoMaximoEsperaMedio * 3) / 4);
    p.estado = ESPERA;
    p.precisaHospital = (p.idoso && probabilidade(configuracao.probabilidadeIdosoPrecisaHospital)) || (!p.idoso && probabilidade(configuracao.probabilidadeNaoIdosoPrecisaHospital));
    sem_wait(&mutexVariaveisMonitor);
    idPessoa++;
    sem_post(&mutexVariaveisMonitor);
    return p;
}
struct pessoa criaMedico() { // Cria medico
    struct pessoa m;
    m.id = idPessoa;
    m.medico = TRUE;
    m.idoso = probabilidade(configuracao.probabilidadeSerIdoso);
    m.tipoTeste = TESTE_RAPIDO;
    m.estadoTeste = NAO_TESTOU;
    m.desistiu = FALSE;
    m.precisaHospital = (m.idoso && probabilidade(configuracao.probabilidadeIdosoPrecisaHospital)) || (!m.idoso && probabilidade(configuracao.probabilidadeNaoIdosoPrecisaHospital));
    sem_wait(&mutexVariaveisMonitor);
    idPessoa++;
    sem_post(&mutexVariaveisMonitor);
    return m;
}

void FilaEspera(struct pessoa *pessoa) {
    char mensagem[TAMANHO_LINHA];
    long timestamp = minutosDecorridos;
    int index, tempoEspera;
    int valorSemaforo; // Fica com o tamanho do semaforo
    char *tipoPessoa;
    if (pessoa->centroTeste == 1) { // CENTRO TESTES 1
        pthread_mutex_lock(&mutexVariaveisCentros);
        int pessoasNaFila = centroTestes1.numeroPessoasEspera;
        pthread_mutex_unlock(&mutexVariaveisCentros);
        if (pessoasNaFila < configuracao.tamanhoFilaCentro1) { // Se o numero de pessoas na fila de espera for menor que o tamanho da fila avança
            tipoPessoa = printTipoPessoa(pessoa);
            printf("%s chegou a fila do centro 1.\n", tipoPessoa);
            free(tipoPessoa);
            if (pessoa->numeroPessoasAFrenteParaDesistir < pessoasNaFila) { // Se o numero de pessoas na fila de espera for maior que o numero de pessoas a frente que essa pessoa admite, ela desiste
                tipoPessoa = printTipoPessoa(pessoa);
                printf(VERMELHO "%s desistiu da fila do 1 porque "
                                "tinha muita gente a frente.\n" RESET,
                       tipoPessoa);
                free(tipoPessoa);
                pessoa->desistiu = TRUE; // Pessoa desiste
            } else {                     // Senão é aumentado o numero de pessoas em espera, calculado e registado o tempo de maximo de espera dessa pessoa e mais tarde verificado se a pessoa desiste a espera ou é testada
                pessoa->tempoChegadaFilaEspera = timestamp;
                pthread_mutex_lock(&mutexVariaveisCentros);
                centroTestes1.numeroPessoasEspera++; //É aumentado o numero de pessoas em espera
                pthread_mutex_unlock(&mutexVariaveisCentros);
                sem_wait(&centroTestes1.filaEspera);
                pthread_mutex_lock(&mutexVariaveisSimulacao);
                tempoEspera = minutosDecorridos - pessoa->tempoChegadaFilaEspera; // Calculado e registado o tempo de maximo de espera dessa pessoa
                pthread_mutex_unlock(&mutexVariaveisSimulacao);
                if (tempoEspera > pessoa->tempoMaximoEspera) { // Se passou muito tempo à espera, a pessoa desiste
                    pessoa->desistiu = TRUE;                   // Pessoa desiste
                    pthread_mutex_lock(&mutexVariaveisCentros);
                    centroTestes1.numeroPessoasEspera--; //É diminuido o número de pessoas em espera
                    sem_getvalue(&centroTestes1.filaEspera, &valorSemaforo);
                    if (valorSemaforo < centroTestes1.numeroPostosDisponiveis) { // Liberta uma pessoa da fila caso haja posto de testagem disponiveis
                        sem_post(&centroTestes1.filaEspera);
                    }
                    pthread_mutex_unlock(&mutexVariaveisCentros);
                    tipoPessoa = printTipoPessoa(pessoa);
                    printf(VERMELHO "%s desistiu no centro 1 porque "
                                    "passou muito tempo a espera.\n" RESET,
                           tipoPessoa);
                    free(tipoPessoa);
                } else { // Senão, não desiste e vai ser testada
                    tipoPessoa = printTipoPessoa(pessoa);
                    printf(VERDE "%s foi testada no centro 1.\n" RESET, tipoPessoa);
                    free(tipoPessoa);
                    pthread_mutex_lock(&mutexVariaveisCentros);
                    centroTestes1.numeroPessoasEspera--; //É diminuido o número de pessoas em espera
                    for (index = 0; index < configuracao.numeroPontosTestagemCentro1 + 1; index++) {
                        if (tempoCooldownPontosTestagemCentro1[index] == -1) // Verifica qual o posto testagem livre e começa-se o cooldown
                        {
                            centroTestes1.numeroPostosDisponiveis--;                                              // Diminui o numero depostos disponiveis
                            tempoCooldownPontosTestagemCentro1[index] = configuracao.tempoCooldownPontosTestagem; //É atribuito o cooldown ao posto escolhido
                            break;
                        }
                    }
                    pthread_mutex_unlock(&mutexVariaveisCentros);
                    pessoa->estado = ISOLAMENTO; // A pessoa é enviada para isolamento
                    sem_wait(&mutexVariaveisMonitor);
                    pessoasEmIsolamento++;           // O numero de pessoas em isolamento aumenta
                    casosEmEstudo++;                 // O numero de casos em estudo aumenta
                    numeroPessoasTestadas++;         // O numero de pessoas testadas aumenta
                    somaTemposEspera += tempoEspera; //É somado o tempo de espera que essa pessoa esteve sujeita para depois ser feita a média
                    sem_post(&mutexVariaveisMonitor);
                }
            }
        } else {
            pthread_mutex_unlock(&mutexVariaveisCentros);
            pessoa->desistiu = TRUE; // Pessoa desiste
        }
    } else { // CENTRO TESTES 2
        pthread_mutex_lock(&mutexVariaveisCentros);
        int numeroPessoasEsperaCentro2 =
            centroTestes2.numeroPessoasNormalEspera +
            centroTestes2.numeroPessoasPrioritariasEspera; // O numero de pessoas em espera do centro 2 é a soma do numero de pessoas normais em espera com o numero de pessoas prioritarias em espera
        pthread_mutex_unlock(&mutexVariaveisCentros);
        if (numeroPessoasEsperaCentro2 < configuracao.tamanhoFilaCentro2) { // Se o numero de pessoas na fila de espera
                                                                            // for menor que o tamanho da fila avança
            tipoPessoa = printTipoPessoa(pessoa);
            printf("%s chegou a fila do centro 2.\n", tipoPessoa);
            free(tipoPessoa);
            if (pessoa->numeroPessoasAFrenteParaDesistir < numeroPessoasEsperaCentro2) { // Se o numero de pessoas na fila de espera for maior que o numero de pessoas a frente que essa pessoa admite, ela desiste
                tipoPessoa = printTipoPessoa(pessoa);
                printf(VERMELHO "%s desistiu da fila do centro 2 "
                                "porque tinha muita gente a frente.\n" RESET,
                       tipoPessoa);
                free(tipoPessoa);
                pessoa->desistiu = TRUE; // Pessoa desiste
            } else { // Senão, é verificado se a pessoa é um idoso ou não para depois ser aumentada a respetiva fila de espera (normal ou prioritaria), calculado e registado o tempo de maximo de espera dessa pessoa e mais
                     // tarde verificado se a pessoa desiste a espera ou é testada
                pessoa->tempoChegadaFilaEspera = timestamp;
                int aux;
                if (pessoa->idoso) { // Verifica se é um idoso ou não
                    pthread_mutex_lock(&mutexVariaveisCentros);
                    centroTestes2.numeroPessoasPrioritariasEspera++;
                    pthread_mutex_unlock(&mutexVariaveisCentros);
                    sem_wait(&centroTestes2.filaEsperaPrioritaria);
                } else { // Se não for idoso
                    pthread_mutex_lock(&mutexVariaveisCentros);
                    centroTestes2.numeroPessoasNormalEspera++; // Aumenta o numero de pessoas da fila de espera normal
                    pthread_mutex_unlock(&mutexVariaveisCentros);
                    sem_wait(&centroTestes2.filaEsperaNormal);
                }
                pthread_mutex_lock(&mutexVariaveisSimulacao);
                tempoEspera = minutosDecorridos - pessoa->tempoChegadaFilaEspera; //É calculado e registado o tempo de maximo de espera dessa pessoa
                pthread_mutex_unlock(&mutexVariaveisSimulacao);
                if (tempoEspera > pessoa->tempoMaximoEspera) { // Se passou muito tempo à espera, a pessoa desiste
                    pessoa->desistiu = TRUE;                   // Pessoa desistiu
                    pthread_mutex_lock(&mutexVariaveisCentros);
                    if (pessoa->idoso) { // Se for idoso, ele é retirado da fila prioritaria porque desistiu
                        centroTestes2.numeroPessoasPrioritariasEspera--;
                    } else { // Se for normal, ele é retirado da fila normal porque desistiu
                        centroTestes2.numeroPessoasNormalEspera--;
                    }
                    int assinalarSemaforoNormal = ((centroTestes2.numeroPessoasPrioritariasEspera == 0 && centroTestes2.numeroPessoasNormalEspera == 0) ||
                                                   (centroTestes2.numeroPessoasNormalEspera > 0 && (idososTestadosConsecutivamente >= 5 || centroTestes2.numeroPessoasPrioritariasEspera == 0)));
                    int assinalarSemaforoPrioritario = ((centroTestes2.numeroPessoasPrioritariasEspera == 0 && centroTestes2.numeroPessoasNormalEspera == 0) ||
                                                        (centroTestes2.numeroPessoasPrioritariasEspera > 0 && (idososTestadosConsecutivamente < 5 || centroTestes2.numeroPessoasNormalEspera == 0)));
                    if (assinalarSemaforoNormal) { // Liberta fila normal
                        sem_getvalue(&centroTestes2.filaEsperaNormal, &valorSemaforo);
                        if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
                            sem_post(&centroTestes2.filaEsperaNormal);
                        }
                    }
                    if (assinalarSemaforoPrioritario) { // Liberta fila prioritaria
                        sem_getvalue(&centroTestes2.filaEsperaPrioritaria, &valorSemaforo);
                        if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
                            sem_post(&centroTestes2.filaEsperaPrioritaria);
                        }
                    }
                    pthread_mutex_unlock(&mutexVariaveisCentros);
                    tipoPessoa = printTipoPessoa(pessoa);
                    printf(VERMELHO "%s desistiu no centro 2 porque "
                                    "passou muito tempo a espera.\n" RESET,
                           tipoPessoa);
                    free(tipoPessoa);
                } else { // Senão, não desiste e vai ser testada
                    tipoPessoa = printTipoPessoa(pessoa);
                    printf(VERDE "%s foi testada no centro 2.\n" RESET, tipoPessoa);
                    free(tipoPessoa);
                    pthread_mutex_lock(&mutexVariaveisCentros);
                    if (pessoa->idoso) { // Se for idoso, é aumentado o numero de idosos testados consecutivamente e fila normal fica em espera
                        idososTestadosConsecutivamente++;
                        sem_getvalue(&centroTestes2.filaEsperaNormal, &valorSemaforo);
                        if (valorSemaforo > 0) {
                            sem_wait(&centroTestes2.filaEsperaNormal);
                        }
                    } else { // Senão o numero de idosos testados consecutivamente é colocado a 0 e a fila prioritaria fica em espera
                        idososTestadosConsecutivamente = 0;
                        sem_getvalue(&centroTestes2.filaEsperaPrioritaria, &valorSemaforo);
                        if (valorSemaforo > 0) {
                            sem_wait(&centroTestes2.filaEsperaPrioritaria);
                        }
                    }
                    if (pessoa->idoso) { // Se for idoso, ele é retirado da fila prioritaria para que seja testado
                        centroTestes2.numeroPessoasPrioritariasEspera--;
                    } else { // Se for normal, ele é retirado da fila normal para que seja testado
                        centroTestes2.numeroPessoasNormalEspera--;
                    }
                    for (index = 0; index < configuracao.numeroPontosTestagemCentro2 + 1; index++) {
                        if (tempoCooldownPontosTestagemCentro2[index] == -1) // Verifica qual o posto testagem livre e começa-se o cooldown
                        {
                            centroTestes2.numeroPostosDisponiveis--;                                              // Diminui o numero de postos disponiveis
                            tempoCooldownPontosTestagemCentro2[index] = configuracao.tempoCooldownPontosTestagem; //É atribuito o cooldown ao posto escolhido
                            break;
                        }
                    }
                    pthread_mutex_unlock(&mutexVariaveisCentros);
                    pessoa->estado = ISOLAMENTO; // A pessoa é enviada para isolamento
                    sem_wait(&mutexVariaveisMonitor);
                    pessoasEmIsolamento++;           // O numero de pessoas em isolamento aumenta
                    casosEmEstudo++;                 // O numero de casos em estudo aumenta
                    numeroPessoasTestadas++;         // O numero de pessoas testadas aumenta
                    somaTemposEspera += tempoEspera; //É somado o tempo de espera que essa pessoa esteve sujeita para depois ser feita a média
                    sem_post(&mutexVariaveisMonitor);
                }
            }
        } else {
            pthread_mutex_unlock(&mutexVariaveisCentros);
            pessoa->desistiu = TRUE; // Pessoa desiste
        }
    }
}

void Pessoa(void *ptr) {
    struct pessoa pessoa = criaPessoa(); // Cria uma pessoa
    PessoasCriadas[pessoa.id] = &pessoa; // Guarda o endereço da pessoa
    char mensagem[TAMANHO_LINHA];
    char *tipoPessoa;
    while (TRUE) {
        FilaEspera(&pessoa); // Entra na função FilaEspera para decidir se a pessoa desiste durante o processo de espera ou termina o processo indo para o isolamento
        if (!pessoa.desistiu) {
            int tempoEsperaTeste = 0;
            if (pessoa.tipoTeste == TESTE_RAPIDO) { // Se o tipo de teste for teste rapido
                tempoEsperaTeste = configuracao.tempoTesteRapido * HORA * 1000;
            } else {
                tempoEsperaTeste = configuracao.tempoTesteNormal * HORA * 1000;
            }
            usleep(tempoEsperaTeste);
            fazerTeste(&pessoa);                  // Entra na função para saber qual o resultado do teste
            if (pessoa.estadoTeste == POSITIVO) { // Se o teste for positivo
                tipoPessoa = printTipoPessoa(&pessoa);
                printf(AMARELO "%s testou positivo.\n" RESET,
                       tipoPessoa); //É dada a mensagem a indicar o resultado
                free(tipoPessoa);
                sem_wait(&mutexVariaveisMonitor);
                casosEmEstudo--;        // Diminui o numero de casos em estudo
                casosPositivosAtivos++; // Aumente o numero de casos positivos ativos
                casosPositivosTotal++;  // Aumenta o numero de casos positivos total
                sem_post(&mutexVariaveisMonitor);
                sem_init(&pessoa.semaforoPessoa, 0, 0);
                pessoa.numeroDiasDesdePositivo = 0; // Coloca a 0 o numero de dias desde o ultimo caso positivo
                break;
            } else if (pessoa.estadoTeste == NEGATIVO) { // Se o teste for negativo
                tipoPessoa = printTipoPessoa(&pessoa);
                printf(AMARELO "%s testou negativo.\n" RESET,
                       tipoPessoa); //É dada a mensagem a indicar o resultado
                free(tipoPessoa);
                sem_wait(&mutexVariaveisMonitor);
                casosEmEstudo--;       // Diminui o numero de casos em estudo
                pessoasEmIsolamento--; // Diminui o numero de pessoas em isolamento
                sem_post(&mutexVariaveisMonitor);
                pessoa.estado = SOBREVIVEU; // O estado da pessoa é colocado como sobrevivente
                break;
            } else { // Se o teste for inconclusivo
                tipoPessoa = printTipoPessoa(&pessoa);
                printf(AMARELO "%s estou inconclusivo.\n" RESET,
                       tipoPessoa); //É dada a mensagem a indicar o resultado
                free(tipoPessoa);
                sem_wait(&mutexVariaveisMonitor);
                casosEmEstudo--;       // Diminui o numero de casos em estudo
                pessoasEmIsolamento--; // Diminui o numero de pessoas em isolamento
                sem_post(&mutexVariaveisMonitor);
                pessoa.estado = ESPERA; // O estado da pessoa é colocado como espera
            }
        } else {
            break;
        }
    }
    if (pessoa.estadoTeste == POSITIVO) {   // Se o teste for positivo
        pessoa.numeroDiasDesdePositivo = 0; // Coloca a 0 o numero de dias desde o ultimo caso positivo
        if (pessoa.precisaHospital) {       // Vai para o Hospital se houver camas livres
            pthread_mutex_lock(&mutexVariaveisHospital);
            if (numeroPacientesNoHospital < configuracao.tamanhoHospital) { // Se o numero de pacientes no hospital for menor que o tamanho do hospital, a pessoa é colocada no hospital
                int index;
                for (index = 0; index < configuracao.tamanhoHospital; index++) {
                    if (IDsDoentesNoHospital[index] == -1) {
                        IDsDoentesNoHospital[index] = pessoa.id; // A pessoa é colocada numa vaga do hospital
                        break;
                    }
                }
                pthread_mutex_unlock(&mutexVariaveisHospital);
                pessoa.estado = HOSPITAL; // O estado da pessoa é colocado hospital
                tipoPessoa = printTipoPessoa(&pessoa);
                printf(CIANO "%s foi transportada para o hospital.\n" RESET,
                       tipoPessoa); //É dada a mensagem que a pessoa foi enviada para o hospital
                free(tipoPessoa);
                sem_post(&semaforoMedicos);
                sem_wait(&mutexVariaveisMonitor);
                pessoasEmIsolamento--;       // Diminui o numero de pessoas em isolamento
                numeroPacientesNoHospital++; // Aumenta o numero de pacientes no hospital
                sem_post(&mutexVariaveisMonitor);
                sem_wait(&semaforoDoentes);
            } else {
                pthread_mutex_unlock(&mutexVariaveisHospital);
                tipoPessoa = printTipoPessoa(&pessoa);
                printf(AMARELO "%s precisava de ajuda medica mas ficara em isolamento.\n" RESET, tipoPessoa);
                free(tipoPessoa);
            }
        }
        sem_wait(&pessoa.semaforoPessoa);
    }
}

void Medico(void *ptr) {
    struct pessoa medico = criaMedico(); // Cria um medico
    PessoasCriadas[medico.id] = &medico; // Guarda o endereço do medico
    char mensagem[TAMANHO_LINHA];
    int tempoEsperaTeste;
    char *tipoPessoa;
    int index, idPaciente;
    sem_init(&medico.semaforoPessoa, 0, 0);
    while (TRUE) {
        sem_wait(&semaforoMedicos);
        sem_wait(&mutexVariaveisMonitor);
        medicosDisponiveis--; // Diminui o número de médicos disponiveis
        sem_post(&mutexVariaveisMonitor);
        pthread_mutex_lock(&mutexVariaveisHospital);
        for (index = 0; index < configuracao.tamanhoHospital; index++) {
            if (IDsMedicosASerUsados[index] == -1 && IDsDoentesNoHospital[index] != -1) {
                IDsMedicosASerUsados[index] = medico.id; // O médico é colocado na lista de medicos a ser usados
                idPaciente = IDsDoentesNoHospital[index];
                break;
            }
        }
        pthread_mutex_unlock(&mutexVariaveisHospital);
        tipoPessoa = printTipoPessoa(&medico);
        printf(CIANO "%s vai tratar do paciente com id %d.\n" RESET, tipoPessoa,
               idPaciente); //É dada a mensagem a indicar que o médico X está a tratar do paciente
        free(tipoPessoa);
        sem_post(&semaforoDoentes);
        sem_wait(&medico.semaforoPessoa); // Medico acabou de tratar de doente ou este morreu -> sai da espera no semaforo
        tempoEsperaTeste = configuracao.tempoTesteRapido * HORA * 1000 + numeroAleatorio(100000, 0);
        if (medico.estado == ISOLAMENTO) { // Se o medico estiver no isolamento
            while (TRUE) {
                usleep(tempoEsperaTeste);
                fazerTeste(&medico);                  // Entra na função para saber qual o resultado do teste
                if (medico.estadoTeste == POSITIVO) { // Se o teste for positivo
                    tipoPessoa = printTipoPessoa(&medico);
                    printf(BRANCO "%s testou positivo.\n" RESET,
                           tipoPessoa); //É dada a mensagem a indicar o resultado
                    free(tipoPessoa);
                    sem_wait(&mutexVariaveisMonitor);
                    casosEmEstudo--;        // Diminui o numero de casos em estudo
                    casosPositivosAtivos++; // Aumente o numero de casos positivos ativos
                    casosPositivosTotal++;  // Aumenta o numero de casos positivos total
                    sem_post(&mutexVariaveisMonitor);
                    medico.numeroDiasDesdePositivo = 0; // Coloca a 0 o numero de dias desde o ultimo caso positivo
                    break;
                } else if (medico.estadoTeste == NEGATIVO) { // Se o teste for negativo
                    tipoPessoa = printTipoPessoa(&medico);
                    printf(BRANCO "%s testou negativo.\n" RESET,
                           tipoPessoa); //É dada a mensagem a indicar o resultado
                    free(tipoPessoa);
                    sem_wait(&mutexVariaveisMonitor);
                    casosEmEstudo--;       // Diminui o numero de casos em estudo
                    pessoasEmIsolamento--; // Diminui o numero de pessoas em isolamento
                    medicosDisponiveis++;  // Aumenta o número de médicos disponiveis
                    sem_post(&mutexVariaveisMonitor);
                    medico.estado = SOBREVIVEU; // O estado da médico é colocado como sobrevivente
                    break;
                } else {
                    tipoPessoa = printTipoPessoa(&medico);
                    printf(BRANCO "%s testou inconclusivo.\n" RESET,
                           tipoPessoa); //É dada a mensagem a indicar o resultado
                    free(tipoPessoa);
                    tempoEsperaTeste = configuracao.tempoTesteRapido * HORA * 1000 + numeroAleatorio(100000, 0);
                }
            }
        }
        if (medico.estadoTeste == POSITIVO) {   // Se o teste for positivo
            medico.numeroDiasDesdePositivo = 0; // Coloca a 0 o numero de dias desde o ultimo caso positivo
            if (medico.precisaHospital) {       // Vai para o Hospital se houver camas livres
                pthread_mutex_lock(&mutexVariaveisHospital);
                if (numeroPacientesNoHospital < configuracao.tamanhoHospital) { // Se o numero de pacientes no hospital for menor que o tamanho do hospital, o médico é colocada no hospital
                    medico.estado = HOSPITAL;                                   // O estado do medico é colocado hospital
                    tipoPessoa = printTipoPessoa(&medico);
                    printf(CIANO "%s foi transportada para o hospital.\n" RESET,
                           tipoPessoa); //É dada a mensagem que o médico foi enviado para o hospital
                    free(tipoPessoa);
                    for (index = 0; index < configuracao.tamanhoHospital; index++) {
                        if (IDsDoentesNoHospital[index] == -1) {
                            IDsDoentesNoHospital[index] = medico.id; // O médico é colocada numa vaga do hospital
                            break;
                        }
                    }
                    sem_post(&semaforoMedicos);
                    pthread_mutex_unlock(&mutexVariaveisHospital);
                    sem_wait(&mutexVariaveisMonitor);
                    pessoasEmIsolamento--;       // Diminui o numero de pessoas em isolamento
                    numeroPacientesNoHospital++; // Aumenta o numero de pacientes no hospital
                    sem_post(&mutexVariaveisMonitor);
                    sem_wait(&semaforoDoentes);
                } else {
                    pthread_mutex_unlock(&mutexVariaveisHospital);
                    tipoPessoa = printTipoPessoa(&medico);
                    printf(AMARELO "%s precisava de ajuda medica mas ficara em isolamento.\n" RESET, tipoPessoa);
                    free(tipoPessoa);
                }
            }
            sem_wait(&medico.semaforoPessoa);
            if (medico.estado == MORREU) {
                break;
            }
        }
        medico.estadoTeste = NAO_TESTOU; // O estado da médico é colocado como não testou
    }
}

/*---------------------------------------
            SIMULACAO
-----------------------------------------*/

void simulacao(char *filename) {
    srand(time(NULL));
    carregarConfiguracao(filename);
    iniciarSemaforosETrincos();
    char mensagem[TAMANHO_LINHA];
    int timeStampAnterior = current_timestamp();
    int auxTimeStamp, numeroDia = 1;
    int tempoLimite = configuracao.tempoSimulacao * DIA;
    enviarMensagem("Z_0_Z"); // Mensagem que indica o comeco da simulacao
    int index;
    char *tipoPessoa;
    char mensagensAEnviar[TAMANHO_LINHA];
    for (index = 0; index < configuracao.tamanhoHospital; index++) { //É colocada a lista de doentes no hospital e a lista medicos a serem usados a -1 (Não está lá ninguem)
        IDsDoentesNoHospital[index] = -1;
        IDsMedicosASerUsados[index] = -1;
    }

    for (index = 0; index < configuracao.tamanhoHospital; index++) {
        // Cria tarefas médicos
        if (pthread_create(&IDtarefa[idPessoa], NULL, Medico, NULL)) {
            printf("Erro na criação da tarefa\n");
            exit(1);
        }
    }
    sprintf(mensagem, "%d_%d_%d", configuracao.tamanhoHospital, 11,
            0); //É dada uma mensagem com o numero de médicos disponiveis
    strcat(mensagensAEnviar, mensagem);
    enviarMensagem(mensagensAEnviar);
    medicosDisponiveis = configuracao.tamanhoHospital;
    int libertarMedico = FALSE;
    int valorSemaforo = -1, pessoaMorreu;
    int assinalarSemaforoNormal, assinalarSemaforoPrioritario;
    int numeroPessoasRecuperaramHospital = 0, numeroPessoasRecuperaramIsolamento = 0, numeroPessoasMorreramIsolamento = 0, numeroPessoasMorreramHospital = 0, numeroMedicosParaIsolamento = 0, numeroMedicosRecuperaram = 0;
    int tempoMedioChegadaCentros = configuracao.tempoMedioChegada;
    int indexPicos = 0, fimPico = 0, emPico = FALSE;

    if (configuracao.diasPicos[indexPicos] == numeroDia) { // Se o dia atual for igual a um dos dias de pico as probablidades de a população e os medicos darem positivo aumentam
                                                           // O tempo medio de chegadas é divido por 2
                                                           //É aberto mais 1 posto em cada centro
        emPico = TRUE;
        fimPico = configuracao.diasPicos[indexPicos] + configuracao.duracoesPicos[indexPicos];
        tempoMedioChegadaCentros -= tempoMedioChegadaCentros / 3;
        configuracao.probabilidadePopulacaoPositivo += 0.1;
        configuracao.probabilidadeMedicoPositivo += 0.1;
        pthread_mutex_lock(&mutexVariaveisCentros);
        tempoCooldownPontosTestagemCentro1[configuracao.numeroPontosTestagemCentro1] = 0;
        tempoCooldownPontosTestagemCentro2[configuracao.numeroPontosTestagemCentro2] = 0;
        pthread_mutex_unlock(&mutexVariaveisCentros);
        printf("---------------------------------------------INICIO "
               "PICO--------------------------------------------\n");
    }
    int proximoInstanteChegada = numeroAleatorio(tempoMedioChegadaCentros + tempoMedioChegadaCentros / 3, tempoMedioChegadaCentros - tempoMedioChegadaCentros / 3);
    if (proximoInstanteChegada == 0) {
        proximoInstanteChegada++;
    }
    while (tempoDecorrido != tempoLimite) { // Enquanto o tempo decorrido não for igual ao tempo limite a simulação continua
        auxTimeStamp = current_timestamp();
        if (auxTimeStamp != timeStampAnterior) { // Se o tempo atual for diferente do tempoanterior, aumenta o tempo
            tempoDecorrido++;                    // Aumenta o tempo decorrido
            timeStampAnterior = auxTimeStamp;
            if (tempoDecorrido % MINUTO == 0) { // Passou um minuto na simulação e é enviada as mensagens para o monitor
                pthread_mutex_lock(&mutexVariaveisSimulacao);
                minutosDecorridos++; // Aumenta o numero de minutos decorridos
                pthread_mutex_unlock(&mutexVariaveisSimulacao);
                mensagensAEnviar[0] = '\0';
                pthread_mutex_lock(&mutexVariaveisCentros);
                for (index = 0; index < configuracao.numeroPontosTestagemCentro1 + 1; index++) {
                    if (tempoCooldownPontosTestagemCentro1[index] == 0) { // Se o tempo de cooldown do posto for igual a 0, esse posto é colocado como disponivel
                        centroTestes1.numeroPostosDisponiveis++;
                        printf(MAGENTA "%d %s de testagem disponivel no centro 1.\n" RESET, centroTestes1.numeroPostosDisponiveis, (centroTestes1.numeroPostosDisponiveis == 1 ? "posto" : "postos"));
                        sem_post(&centroTestes1.filaEspera);
                        tempoCooldownPontosTestagemCentro1[index]--;
                    } else if (tempoCooldownPontosTestagemCentro1[index] > 0) { // Se o tempo de cooldown do posto for maior que 0, é feita a contagem decrescente do cooldown
                        tempoCooldownPontosTestagemCentro1[index]--;
                    }
                }
                for (index = 0; index < configuracao.numeroPontosTestagemCentro2 + 1; index++) {
                    if (tempoCooldownPontosTestagemCentro2[index] == 0) { // Se o tempo de cooldown atinge os 0, o posto é colocado como disponivel
                        centroTestes2.numeroPostosDisponiveis++;          //É aumentado o numero de postos disponiveis
                        printf(MAGENTA "%d %s de testagem disponivel no centro 2.\n" RESET, centroTestes2.numeroPostosDisponiveis, (centroTestes2.numeroPostosDisponiveis == 1 ? "posto" : "postos"));
                        assinalarSemaforoNormal = ((centroTestes2.numeroPessoasPrioritariasEspera == 0 && centroTestes2.numeroPessoasNormalEspera == 0) ||
                                                   (centroTestes2.numeroPessoasNormalEspera > 0 && (idososTestadosConsecutivamente >= 5 || centroTestes2.numeroPessoasPrioritariasEspera == 0)));
                        assinalarSemaforoPrioritario = ((centroTestes2.numeroPessoasPrioritariasEspera == 0 && centroTestes2.numeroPessoasNormalEspera == 0) ||
                                                        (centroTestes2.numeroPessoasPrioritariasEspera > 0 && (idososTestadosConsecutivamente < 5 || centroTestes2.numeroPessoasNormalEspera == 0)));
                        if (assinalarSemaforoNormal) {
                            sem_getvalue(&centroTestes2.filaEsperaNormal, &valorSemaforo);
                            if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
                                sem_post(&centroTestes2.filaEsperaNormal); // Liberta fila normal
                            }
                        }
                        if (assinalarSemaforoPrioritario) {
                            sem_getvalue(&centroTestes2.filaEsperaPrioritaria, &valorSemaforo);
                            if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
                                sem_post(&centroTestes2.filaEsperaPrioritaria); // Liberta fila prioritaria
                            }
                        }
                        tempoCooldownPontosTestagemCentro2[index]--;            // Coloca o tempo decooldown a -1;
                    } else if (tempoCooldownPontosTestagemCentro2[index] > 0) { // Se o tempo de cooldown do posto for maior que 0, é feita a contagem decrescente do cooldown
                        tempoCooldownPontosTestagemCentro2[index]--;
                    }
                }
                pthread_mutex_unlock(&mutexVariaveisCentros);
                if (minutosDecorridos % proximoInstanteChegada == 0) {
                    proximoInstanteChegada = numeroAleatorio(tempoMedioChegadaCentros + tempoMedioChegadaCentros / 3, tempoMedioChegadaCentros - tempoMedioChegadaCentros / 3);
                    if (proximoInstanteChegada == 0) {
                        proximoInstanteChegada++;
                    }
                    // Cria tarefas pessoas
                    if (pthread_create(&IDtarefa[idPessoa], NULL, Pessoa, NULL)) {
                        printf("Erro na criação da tarefa\n");
                        exit(1);
                    }
                }
                if (minutosDecorridos % (60 * 24) == 0) { // Se o minutos decorridos for igual a 1 dia
                    printf("---------------------------------------------DIA "
                           "%d---------------------------------------------\n",
                           ++numeroDia); //É indicado o dia
                    sprintf(mensagem, "%d_%d_%d", 0, 10, 0);
                    strcat(mensagensAEnviar, mensagem);
                    pthread_mutex_lock(&mutexVariaveisHospital);
                    numeroPessoasRecuperaramHospital = 0, numeroPessoasRecuperaramIsolamento = 0, numeroPessoasMorreramHospital = 0, numeroPessoasMorreramIsolamento = 0, numeroMedicosParaIsolamento = 0,
                    numeroMedicosRecuperaram = 0;
                    for (index = 0; index < configuracao.tamanhoHospital; index++) {
                        libertarMedico = FALSE;                                                                                    // variavel que indica se medico pode ir para isolamento
                        if (IDsDoentesNoHospital[index] != -1) {                                                                   // Se há doente no hospital
                            if (PessoasCriadas[IDsDoentesNoHospital[index]]->numeroDiasDesdePositivo == configuracao.tempoCurar) { // Se já passou o tempo para curar
                                tipoPessoa = printTipoPessoa(PessoasCriadas[IDsDoentesNoHospital[index]]);
                                printf(CIANO "%s recuperou no hospital.\n" RESET, tipoPessoa);
                                free(tipoPessoa);
                                sem_post(&PessoasCriadas[IDsDoentesNoHospital[index]]->semaforoPessoa);
                                numeroPessoasRecuperaramHospital++;                                              // Aumenta o numero de pessoas que recuperaram no hospital
                                numeroMedicosRecuperaram += PessoasCriadas[IDsDoentesNoHospital[index]]->medico; // Se a pessoa for médico (+1), é aumentado o numero de medicos que recuperaram
                                PessoasCriadas[IDsDoentesNoHospital[index]]->estado = SOBREVIVEU;
                                IDsDoentesNoHospital[index] = -1;                         // A vaga do hospital passa a estar livre
                                libertarMedico = TRUE;                                    // O medico deve ir para isolamento
                            } else {                                                      // Se ainda não passou o tempo para curar
                                if (PessoasCriadas[IDsDoentesNoHospital[index]]->idoso) { // Se a pessoa for idosa
                                    pessoaMorreu = IDsMedicosASerUsados[index] != -1 ? probabilidade(configuracao.probabilidadeIdosoMorrer / 4) : probabilidade(configuracao.probabilidadeIdosoMorrer);
                                } else { // Se a pessoa não for idosa
                                    pessoaMorreu = IDsMedicosASerUsados[index] != -1 ? probabilidade(configuracao.probabilidadeNaoIdosoMorrer / 4) : probabilidade(configuracao.probabilidadeNaoIdosoMorrer);
                                }
                                if (pessoaMorreu) {
                                    sem_post(&PessoasCriadas[IDsDoentesNoHospital[index]]->semaforoPessoa);
                                    tipoPessoa = printTipoPessoa(PessoasCriadas[IDsDoentesNoHospital[index]]);
                                    printf(CIANO "%s morreu no hospital.\n" RESET, tipoPessoa);
                                    free(tipoPessoa);
                                    numeroPessoasMorreramHospital++;                              //É aumentado o número de pessoas que morreram
                                    PessoasCriadas[IDsDoentesNoHospital[index]]->estado = MORREU; // O estado da pessoa passa para morreu
                                    IDsDoentesNoHospital[index] = -1;                             // A vaga do hospital passa a estar livre
                                    libertarMedico = TRUE;                                        // O medico deve ir para isolamento
                                }
                            }
                            if (IDsMedicosASerUsados[index] != -1 && libertarMedico) {            // Se o médico tiver sido utilizado e já estiver livre, é enviado para isolamento
                                numeroMedicosParaIsolamento++;                                    //É aumentado o número de médicos em isolamento
                                PessoasCriadas[IDsMedicosASerUsados[index]]->estado = ISOLAMENTO; // O estado do médico passa a ser isolamento
                                tipoPessoa = printTipoPessoa(PessoasCriadas[IDsMedicosASerUsados[index]]);
                                printf(CIANO "%s vai para isolamento.\n" RESET, tipoPessoa);
                                free(tipoPessoa);
                                sem_post(&PessoasCriadas[IDsMedicosASerUsados[index]]->semaforoPessoa);
                                IDsMedicosASerUsados[index] = -1; // A posição que o medico estava a utilizar na lista de médicos a ser utilizados passa a ficar livre para que
                            }
                        }
                        if (IDsDoentesNoHospital[index] != -1) {                                    // Se a pessoa não recuperou e não morreu, aumentar o contador
                            PessoasCriadas[IDsDoentesNoHospital[index]]->numeroDiasDesdePositivo++; //É aumentado o numero de dias desde que a pessoa teve teste positivo
                        }
                    }
                    pthread_mutex_unlock(&mutexVariaveisHospital);
                    int idPessoaAtual = idPessoa;
                    for (index = 0; index < idPessoaAtual; index++) {
                        if (PessoasCriadas[index] != NULL && PessoasCriadas[index]->estado == ISOLAMENTO && PessoasCriadas[index]->estadoTeste == POSITIVO) { // Se a pessoa se encontra em isolamento com o teste positivo
                            if (PessoasCriadas[index]->precisaHospital) {
                                if (PessoasCriadas[index]->idoso) {
                                    pessoaMorreu = probabilidade(configuracao.probabilidadeIdosoMorrer);
                                } else {
                                    pessoaMorreu = probabilidade(configuracao.probabilidadeNaoIdosoMorrer);
                                }
                                if (pessoaMorreu) {
                                    sem_post(&PessoasCriadas[index]->semaforoPessoa);
                                    tipoPessoa = printTipoPessoa(PessoasCriadas[index]);
                                    printf(AZUL "%s morreu em isolamento.\n" RESET, tipoPessoa);
                                    free(tipoPessoa);
                                    numeroPessoasMorreramIsolamento++;
                                    PessoasCriadas[index]->estado = MORREU;
                                }
                            } else if (PessoasCriadas[index]->numeroDiasDesdePositivo == configuracao.tempoCurar) { // Se já passou o tempo para curar
                                tipoPessoa = printTipoPessoa(PessoasCriadas[index]);
                                printf(AZUL "%s recuperou em isolamento.\n" RESET, tipoPessoa);
                                free(tipoPessoa);
                                PessoasCriadas[index]->estado = SOBREVIVEU;                // O estado da pessoa passa a ser sobreviveu
                                PessoasCriadas[index]->estadoTeste = NAO_TESTOU;           // O estado do teste da pessoa passa a ser nao testou
                                numeroPessoasRecuperaramIsolamento++;                      //É aumentado o numero de pessoas que recuperaram no isolamento
                                numeroMedicosRecuperaram += PessoasCriadas[index]->medico; // Caso a pessoa seja médico (+1) é aumentado o numero de medicos que recuperaram
                                sem_post(&PessoasCriadas[index]->semaforoPessoa);
                            } else {                                              // Se ainda não passou o tempo para curar
                                PessoasCriadas[index]->numeroDiasDesdePositivo++; //É aumentado o numero de dias desde que a pessoa teve teste positivo
                            }
                        }
                    }
                    sem_wait(&mutexVariaveisMonitor);
                    casosRecuperados += numeroPessoasRecuperaramIsolamento + numeroPessoasRecuperaramHospital; // O numero de casos recuperados é a soma do numero de pessoas que recuperaram no isolamento , o numero de pessoas
                                                                                                               // que recuperaram no hospital e o numero de casos recuperados anteriormente
                    numeroPacientesNoHospital -= (numeroPessoasRecuperaramHospital + numeroPessoasMorreramHospital); // O numero de pacientes no hostipal é a subtração do numero de pacientes no hospital anteriormente e a soma
                                                                                                                     // do numero de pessoas que recuperarm no hospital e o numero de pessoas que morreram no hospital
                    pessoasEmIsolamento -= (numeroPessoasRecuperaramIsolamento + numeroPessoasMorreramIsolamento); // O numero de pessoas em isolamento é subtraçao do numero de pessoas em isolamento anteriomente e o numero de
                                                                                                                   // pessoas que recuperaram no isolamento e o numero de pessoas que morreram em isolamento
                    pessoasEmIsolamento += numeroMedicosParaIsolamento; // O numero de pessoas em isolamento é a soma do numero de pessoas em isolamento anteriormente  e o numero de medicos para isolamento
                    medicosDisponiveis += numeroMedicosRecuperaram;     // O numero de medicos disponiveis é a soma de medicos disponiveis anteriormente e o numero de medicos que recuperaram
                    // printf("%d\n",numeroMedicosParaIsolamento);
                    casosEmEstudo += numeroMedicosParaIsolamento; // O numero de casos em estudo é a soma do numero de casos em estudo anteriormente e o numero de medicos para isolamento
                    casosPositivosAtivos -= (numeroPessoasRecuperaramHospital + numeroPessoasMorreramHospital + numeroPessoasMorreramIsolamento + numeroPessoasRecuperaramIsolamento);
                    // O numero de casos postivos ativos é subtração do numero de casos positivos ativos anteriormente e a soma do numero de pessoas que recuperaram no hospital, o numero de pessoas que morreram e o numero de
                    // pessoas que recuperaram no isolamento e o numero de pessoas que morreram em isolamento
                    numeroMortos +=
                        numeroPessoasMorreramHospital + numeroPessoasMorreramIsolamento; // O numero de mortos é a soma do numero de mortos anteriomente e o numero de pessoas que morreram no hospital e em isolamento
                    sem_post(&mutexVariaveisMonitor);
                    strcat(mensagensAEnviar, "/");
                    sprintf(mensagem, "%d_%d_%d", casosRecuperados, 8, 0);
                    strcat(mensagensAEnviar, mensagem);
                    strcat(mensagensAEnviar, "/");
                    sprintf(mensagem, "%d_%d_%d", numeroMortos, 9, 0);
                    strcat(mensagensAEnviar, mensagem);
                    strcat(mensagensAEnviar, "/");
                    if (configuracao.diasPicos[indexPicos] == numeroDia && !emPico) { // Se o dia atual for igual a um dos dias de pico as probablidades de a população e os medicos darem positivo aumentam O tempo medio de
                                                                                      // chegadas é divido por 2 É aberto mais 1 posto em cada centro
                        emPico = TRUE;
                        fimPico = configuracao.diasPicos[indexPicos] + configuracao.duracoesPicos[indexPicos];
                        tempoMedioChegadaCentros -= tempoMedioChegadaCentros / 3;
                        configuracao.probabilidadePopulacaoPositivo += 0.1;
                        configuracao.probabilidadeMedicoPositivo += 0.1;
                        pthread_mutex_lock(&mutexVariaveisCentros);
                        tempoCooldownPontosTestagemCentro1[configuracao.numeroPontosTestagemCentro1] = 0;
                        tempoCooldownPontosTestagemCentro2[configuracao.numeroPontosTestagemCentro2] = 0;
                        pthread_mutex_unlock(&mutexVariaveisCentros);
                        printf("---------------------------------------------INICIO "
                               "PICO--------------------------------------------\n");
                    } else if (fimPico == numeroDia && emPico) { // Se o dia atual for igual a um dos dias de fim de pico as probablidades de a população e os medicos darem positivo voltam ao normal O tempo medio de chegadas
                                                                 // volta aonormal E o numero de postos por centro voltam ao normal
                        tempoMedioChegadaCentros += tempoMedioChegadaCentros / 3;
                        configuracao.probabilidadePopulacaoPositivo -= 0.1;
                        configuracao.probabilidadeMedicoPositivo -= 0.1;
                        for (index = indexPicos; index < sizeof(configuracao.diasPicos) / sizeof(configuracao.diasPicos[0]); index++) {
                            if (configuracao.diasPicos[index] > numeroDia) {
                                indexPicos = index;
                                break;
                            }
                        }
                        fimPico = 0;
                        emPico = FALSE;
                        pthread_mutex_lock(&mutexVariaveisCentros);
                        tempoCooldownPontosTestagemCentro1[configuracao.numeroPontosTestagemCentro1] = -2;
                        tempoCooldownPontosTestagemCentro2[configuracao.numeroPontosTestagemCentro2] = -2;
                        centroTestes1.numeroPostosDisponiveis--;
                        centroTestes2.numeroPostosDisponiveis--;
                        sem_getvalue(&centroTestes2.filaEsperaNormal, &valorSemaforo);
                        if (valorSemaforo > 0) {
                            sem_wait(&centroTestes2.filaEsperaNormal);
                        }
                        sem_getvalue(&centroTestes2.filaEsperaPrioritaria, &valorSemaforo);
                        if (valorSemaforo > 0) {
                            sem_wait(&centroTestes2.filaEsperaPrioritaria);
                        }
                        sem_getvalue(&centroTestes1.filaEspera, &valorSemaforo);
                        if (valorSemaforo > 0) {
                            sem_wait(&centroTestes1.filaEspera);
                        }
                        pthread_mutex_unlock(&mutexVariaveisCentros);
                        printf("---------------------------------------------FIM "
                               "PICO--------------------------------------------\n");
                    }
                }
                pthread_mutex_lock(&mutexVariaveisCentros);
                sprintf(mensagem, "%d_%d_%d", centroTestes1.numeroPessoasEspera, 0, 1);
                strcat(mensagensAEnviar, mensagem);
                strcat(mensagensAEnviar, "/");
                sprintf(mensagem, "%d_%d_%d", centroTestes2.numeroPessoasNormalEspera + centroTestes2.numeroPessoasPrioritariasEspera, 0, 2);
                pthread_mutex_unlock(&mutexVariaveisCentros);
                strcat(mensagensAEnviar, mensagem);
                strcat(mensagensAEnviar, "/");
                sem_wait(&mutexVariaveisMonitor);
                sprintf(mensagem, "%d_%d_%d", idPessoa - configuracao.tamanhoHospital, 5, 0);
                strcat(mensagensAEnviar, mensagem);
                strcat(mensagensAEnviar, "/");
                sprintf(mensagem, "%d_%d_%d", casosPositivosAtivos, 1, 0);
                strcat(mensagensAEnviar, mensagem);
                strcat(mensagensAEnviar, "/");
                sprintf(mensagem, "%d_%d_%d", casosPositivosTotal, 2, 0);
                strcat(mensagensAEnviar, mensagem);
                strcat(mensagensAEnviar, "/");
                sprintf(mensagem, "%d_%d_%d", casosEmEstudo, 3, 0);
                strcat(mensagensAEnviar, mensagem);
                strcat(mensagensAEnviar, "/");
                sprintf(mensagem, "%d_%d_%d", pessoasEmIsolamento, 4, 0);
                strcat(mensagensAEnviar, mensagem);
                strcat(mensagensAEnviar, "/");
                sprintf(mensagem, "%d_%d_%d", numeroPacientesNoHospital, 6, 0);
                strcat(mensagensAEnviar, mensagem);
                strcat(mensagensAEnviar, "/");
                sprintf(mensagem, "%d_%d_%d", medicosDisponiveis, 11, 0);
                strcat(mensagensAEnviar, mensagem);
                if (numeroPessoasTestadas > 0) { // Se o numero de pessoas testadas for maior que 0, é enviada a mensagem com a media de tempo de espera
                    tempoMedioEspera = somaTemposEspera / numeroPessoasTestadas;
                    strcat(mensagensAEnviar, "/");
                    sprintf(mensagem, "%d_%d_%d", tempoMedioEspera, 7, 0);
                    strcat(mensagensAEnviar, mensagem);
                }
                sem_post(&mutexVariaveisMonitor);
                enviarMensagem(mensagensAEnviar);
            }
        }
    }
    enviarMensagem("Z_1_Z"); // Mensagem que indica o fim da simulacao
}

void iniciarSemaforosETrincos() {
    if (pthread_mutex_init(&mutexCriarPessoa, NULL) != 0) { // Mensagem de erro
        printf("Inicializacao do trinco falhou.\n");
    }
    if (pthread_mutex_init(&mutexVariaveisCentros, NULL) != 0) { // Mensagem de erro
        printf("Inicializacao do trinco falhou.\n");
    }
    if (pthread_mutex_init(&mutexVariaveisSimulacao, NULL) != 0) { // Mensagem de erro
        printf("Inicializacao do trinco falhou.\n");
    }
    if (pthread_mutex_init(&mutexVariaveisHospital, NULL) != 0) { // Mensagem de erro
        printf("Inicializacao do trinco falhou.\n");
    }
    // Inicialização dos semaforos e trincos
    sem_init(&centroTestes1.filaEspera, 0, 0);
    sem_init(&mutexVariaveisMonitor, 0, 1);
    sem_init(&semaforoEnviarMensagem, 0, 1);
    centroTestes1.numeroPostosDisponiveis = 0;
    tempoCooldownPontosTestagemCentro1 = (int *)calloc(configuracao.numeroPontosTestagemCentro1 + 1, sizeof(int));
    int index;
    for (index = 0; index < configuracao.numeroPontosTestagemCentro1; index++) {
        tempoCooldownPontosTestagemCentro1[index] = 0;
    }
    tempoCooldownPontosTestagemCentro1[index] = -2;
    sem_init(&centroTestes2.filaEsperaPrioritaria, 0, 0);
    sem_init(&centroTestes2.filaEsperaNormal, 0, 0);
    centroTestes2.numeroPostosDisponiveis = 0;
    tempoCooldownPontosTestagemCentro2 = (int *)calloc(configuracao.numeroPontosTestagemCentro2 + 1, sizeof(int));
    for (index = 0; index < configuracao.numeroPontosTestagemCentro2; index++) {
        tempoCooldownPontosTestagemCentro2[index] = 0;
    }
    tempoCooldownPontosTestagemCentro2[index] = -2;

    sem_init(&semaforoMedicos, 0, 0);
    sem_init(&semaforoDoentes, 0, 0);
    IDsMedicosASerUsados = (int *)calloc(configuracao.tamanhoHospital, sizeof(int));
    IDsDoentesNoHospital = (int *)calloc(configuracao.tamanhoHospital, sizeof(int));
}

void carregarConfiguracao(char nomeFicheiro[]) {
    FILE *configFile;
    configFile = fopen(nomeFicheiro, "r");
    if (configFile == NULL) {
        perror("ERRO!\nFicheiro de configuração não existe.");
        exit(EXIT_FAILURE);
    } else if (strcmp(nomeFicheiro, "simulador.conf") != 0) // se strcmp retorna 0, então os string são iguais
    {
        perror("ERRO!\nNão é o ficheiro de configuração.");
        exit(EXIT_FAILURE);
    }
    // Obtem o tamanho do ficheiro:
    fseek(configFile, 0, SEEK_END);
    long fileSize = ftell(configFile);
    rewind(configFile);
    char buffer[fileSize];
    fread(buffer, 1, fileSize, configFile);
    fclose(configFile);

    int i = 0;
    char *p = strtok(buffer, "\n");
    char *lines[25];
    while (p != NULL) {
        lines[i++] = p;
        p = strtok(NULL, "\n");
    }
    char *array[2];
    char *values[TAMANHO_CONFIGURACAO];
    for (int index = 0; index < TAMANHO_CONFIGURACAO; index++) {
        char *aux = strtok(lines[index], ":");
        i = 0;
        while (aux != NULL) {
            array[i++] = aux;
            aux = strtok(NULL, ":");
        }
        values[index] = array[1];
    }

    // Guardar os valores do ficheiro na estrutura de configuracao
    configuracao.tempoMedioChegada = (int)strtol(values[0], NULL, 10);
    if (configuracao.tempoMedioChegada <= 0) {
        printf("Tempo medio de chegada aos centros de teste deve ser superior a 0!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.tempoTesteNormal = strtol(values[1], NULL, 10);
    configuracao.tempoTesteRapido = strtol(values[2], NULL, 10);
    if (configuracao.tempoTesteNormal <= 0) {
        printf("Tempo do teste normal deve ser superior a 0!\n");
        exit(EXIT_FAILURE);
    } else if (configuracao.tempoTesteRapido <= 0) {
        printf("Tempo do teste rapido deve ser superior a 0!\n");
        exit(EXIT_FAILURE);
    } else if (configuracao.tempoTesteNormal <= configuracao.tempoTesteRapido) {
        printf("O teste rapido não pode demorar mais que o teste normal!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.tempoCooldownPontosTestagem = strtol(values[3], NULL, 10);
    if (configuracao.tempoCooldownPontosTestagem <= 0) {
        printf("Tempo de cooldown dos pontos de testagem nos centros de teste deve ser superior a 0!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.tempoMaximoEsperaMedio = strtol(values[4], NULL, 10);
    if (configuracao.tempoMaximoEsperaMedio <= 0) {
        printf("Tempo maximo medio de espera deve ser superior a 0!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.tamanhoFilaCentro1 = strtol(values[5], NULL, 10);
    if (configuracao.tamanhoFilaCentro1 <= 0) {
        printf("Tamanho maximo da fila do centro 1 deve ser superior a 0!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.tamanhoFilaCentro2 = strtol(values[6], NULL, 10);
    if (configuracao.tamanhoFilaCentro2 <= 0) {
        printf("Tamanho maximo da fila do centro 2 deve ser superior a 0!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.numeroPontosTestagemCentro1 = strtol(values[7], NULL, 10);
    if (configuracao.numeroPontosTestagemCentro1 <= 0) {
        printf("Numero de pontos de testagem no centro 1 deve ser superior a 0!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.numeroPontosTestagemCentro2 = strtol(values[8], NULL, 10);
    if (configuracao.numeroPontosTestagemCentro2 <= 0) {
        printf("Numero de pontos de testagem no centro 2 deve ser superior a 0!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.tamanhoHospital = strtol(values[9], NULL, 10);
    if (configuracao.tamanhoHospital <= 0) {
        printf("Tamanho do hospital deve ser superior a 0!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.probabilidadeSerIdoso = strtof(values[10], NULL);
    if (configuracao.probabilidadeSerIdoso < 0 || configuracao.probabilidadeSerIdoso > 1) {
        printf("Probabilidades tem de ser maiores ou iguais a 0 e menores ou iguais a 1!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.probabilidadeMedicoPositivo = strtof(values[11], NULL);
    if (configuracao.probabilidadeMedicoPositivo < 0 || configuracao.probabilidadeMedicoPositivo > 1) {
        printf("Probabilidades tem de ser maiores ou iguais a 0 e menores ou iguais a 1!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.probabilidadePopulacaoPositivo = strtof(values[12], NULL);
    if (configuracao.probabilidadePopulacaoPositivo < 0 || configuracao.probabilidadePopulacaoPositivo > 1) {
        printf("Probabilidades tem de ser maiores ou iguais a 0 e menores ou iguais a 1!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.probabilidadeTesteNormalInconclusivo = strtof(values[13], NULL);
    if (configuracao.probabilidadeTesteNormalInconclusivo < 0 || configuracao.probabilidadeTesteNormalInconclusivo > 1) {
        printf("Probabilidades tem de ser maiores ou iguais a 0 e menores ou iguais a 1!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.probabilidadeTesteRapidoInconclusivo = strtof(values[14], NULL);
    if (configuracao.probabilidadeTesteRapidoInconclusivo < 0 || configuracao.probabilidadeTesteRapidoInconclusivo > 1) {
        printf("Probabilidades tem de ser maiores ou iguais a 0 e menores ou iguais a 1!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.probabilidadeIdosoPrecisaHospital = strtof(values[15], NULL);
    if (configuracao.probabilidadeIdosoPrecisaHospital < 0 || configuracao.probabilidadeIdosoPrecisaHospital > 1) {
        printf("Probabilidades tem de ser maiores ou iguais a 0 e menores ou iguais a 1!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.probabilidadeNaoIdosoPrecisaHospital = strtof(values[16], NULL);
    if (configuracao.probabilidadeNaoIdosoPrecisaHospital < 0 || configuracao.probabilidadeNaoIdosoPrecisaHospital > 1) {
        printf("Probabilidades tem de ser maiores ou iguais a 0 e menores ou iguais a 1!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.probabilidadeIdosoMorrer = strtof(values[17], NULL);
    if (configuracao.probabilidadeIdosoMorrer < 0 || configuracao.probabilidadeIdosoMorrer > 1) {
        printf("Probabilidades tem de ser maiores ou iguais a 0 e menores ou iguais a 1!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.probabilidadeNaoIdosoMorrer = strtof(values[18], NULL);
    if (configuracao.probabilidadeNaoIdosoMorrer < 0 || configuracao.probabilidadeNaoIdosoMorrer > 1) {
        printf("Probabilidades tem de ser maiores ou iguais a 0 e menores ou iguais a 1!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.tempoCurar = strtol(values[19], NULL, 10);
    if (configuracao.tempoCurar <= 0) {
        printf("Tempo para recuperar do CONID-19 deve ser superior a 0!\n");
        exit(EXIT_FAILURE);
    }
    configuracao.tempoSimulacao = strtol(values[20], NULL, 10);
    if (configuracao.tempoSimulacao <= 0) {
        printf("Duracao da simulacao deve ser superior a 0!\n");
        exit(EXIT_FAILURE);
    }
    char *auxPicos = values[21];
    // printf("%s9\n",values[20]);
    // printf("%s9\n",values[21]);
    // printf("%d\n",!(strcmp(values[20],values[21])));
    if (strcmp(values[20], values[21])) {
        int index = 0;
        p = strtok(auxPicos, ", ");
        char picosSeparados[15][15];
        while (p != NULL) {
            strcpy(picosSeparados[index++], p);
            p = strtok(NULL, ", ");
        }
        for (i = 0; i < index; i++) {
            p = strtok(picosSeparados[i], "-");
            configuracao.diasPicos[i] = strtol(p, NULL, 10);
            p = strtok(NULL, "-");
            configuracao.duracoesPicos[i] = strtol(p, NULL, 10);
        }
    }
}

int main(int argc, char *argv[]) {

    if (argc == 2) {
        socketfd = criaSocket(); // Cria o socket
        simulacao(argv[1]);      // Inicia a simulação
        close(socketfd);         // Fecha o socket
        return 0;
    } else {
        perror("ERRO!\n(Só) É preciso passar como argumento o ficheiro de "
               "configuração.");
        exit(EXIT_FAILURE);
    }
}