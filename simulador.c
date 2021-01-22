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
int casosEmEstudo = 0, casosPositivosAtivos = 0, casosPositivosTotal = 0,
    casosRecuperados = 0, numeroMortos = 0, pessoasEmIsolamento = 0,
    medicosDisponiveis = 0, numeroPessoasTestadas = 0, somaTemposEspera = 0,
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
    printf("Erro: socket nao foi criado \n");
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
  // sem_wait(&semaforoEnviarMensagem);
  int numero;
  char mensagem[TAMANHO_LINHA];
  if (strcpy(mensagem, mensagemAEnviar) != 0) {
    numero = strlen(mensagem) + 1;
    if (write(socketfd, mensagem, numero) != numero) {
      printf("Erro no write!\n");
    }
  }

  // usleep(750);
  // sem_post(&semaforoEnviarMensagem);
}

/*---------------------------------------
            FUNCOES AUXILIARES
-----------------------------------------*/

int numeroAleatorio(int numeroMaximo, int numeroMinimo) { //Retorna um numero aleatorio entre numero minimo e numero maximo
  return rand() % (numeroMaximo + 1 - numeroMinimo) + numeroMinimo;
}
int probabilidade(float prob) {  //Função probabilidade, retorna true ou false
  return numeroAleatorio(100, 0) < (prob * 100); 
}
long long current_timestamp() {
  struct timeval te;
  gettimeofday(&te, NULL); // get current time
  long long milliseconds =
      te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
  // printf("milliseconds: %lld\n", milliseconds);
  return milliseconds;
}
void fazerTeste(struct pessoa *pessoa) { //Função atribui resultado do teste
  float probabilidadeInconclusivo = 0.0;
  if (pessoa->tipoTeste == TESTE_NORMAL) { //Verifica qual o tipo de testes (Teste normal ou rapido)
    probabilidadeInconclusivo = configuracao.probabilidadeTesteNormalInconclusivo;
  } else {
    probabilidadeInconclusivo = configuracao.probabilidadeTesteRapidoInconclusivo;
  }
  if (probabilidade(probabilidadeInconclusivo)) { //Verifica se o teste for inconclusivo
    pessoa->estadoTeste = INCONCLUSIVO; // Atribui o estadoTeste inconclusivo
  } else { //Senão verifica se é medico ou não e faz uma probabilidade para decidir se o teste é positivo ou negativo
    if (!pessoa->medico) { // se a pessoa a ser testada não é médica
      if (probabilidade(configuracao.probabilidadePopulacaoPositivo)) { // pessoa testou positivo
        pessoa->estadoTeste = POSITIVO;
      } else {
        pessoa->estadoTeste = NEGATIVO;
      }
    } else { // se a pessoa a ser testada é médica
      if (probabilidade(configuracao.probabilidadeMedicoPositivo)) { // médico testou positivo
        pessoa->estadoTeste = POSITIVO;
      } else {
        pessoa->estadoTeste = NEGATIVO;
      }
    }
  }
}
char *printTipoPessoa(struct pessoa *pessoa) { //Função para escrever o tipo de pessoa Medico (Idoso ou normal) ou Pessoa (Idoso ou normal)
  char *imprimir = malloc(35);
  sprintf(imprimir, "%s %s com o id %d",
          (pessoa->medico ? "O medico" : "A pessoa"),
          (pessoa->idoso ? (pessoa->medico ? "idoso" : "idosa") : "normal"),
          pessoa->id);
  return imprimir;
}

/*---------------------------------------
            PESSOA
-----------------------------------------*/

struct pessoa criaPessoa() { //Cria a pessoa

  struct pessoa p;
  p.id = idPessoa;
  p.medico = FALSE;
  p.centroTeste = numeroAleatorio(2, 1);
  p.estadoTeste = NAO_TESTOU;
  p.tipoTeste = TESTE_NORMAL;
  p.idoso = probabilidade(configuracao.probabilidadeSerIdoso);
  if (p.idoso) {
    p.centroTeste = probabilidade(0.90) + 1; // pessoas idosas preferem o centro de testes 2, o
                                             // prioritário, probabilidade tenderá a dar 1+1=2
  } else {
    p.centroTeste = probabilidade(0.10) + 1; // pessoas normais preferem o centro de testes 1,
                                             // probabilidade tenderá a dar 0 + 1 = centro de testes 1
  }
  p.desistiu = FALSE;
  if (p.centroTeste == 1) {
    p.numeroPessoasAFrenteParaDesistir = numeroAleatorio(configuracao.tamanhoFilaCentro1, (configuracao.tamanhoFilaCentro1 * 3) / 4);
  } else {
    p.numeroPessoasAFrenteParaDesistir = numeroAleatorio(configuracao.tamanhoFilaCentro2, (configuracao.tamanhoFilaCentro2 * 3) / 4);
  }
  p.tempoMaximoEspera = numeroAleatorio(configuracao.tempoMaximoEsperaMedio, (configuracao.tempoMaximoEsperaMedio * 3) / 4);
  p.estado = ESPERA;
  sem_wait(&mutexVariaveisMonitor);
  idPessoa++;
  sem_post(&mutexVariaveisMonitor);
  return p;
}
struct pessoa criaMedico() { //Cria medico
  struct pessoa m;
  m.id = idPessoa;
  m.medico = TRUE;
  m.idoso = probabilidade(configuracao.probabilidadeSerIdoso);
  m.tipoTeste = TESTE_RAPIDO;
  m.estadoTeste = NAO_TESTOU;
  m.desistiu = FALSE;
  sem_wait(&mutexVariaveisMonitor);
  idPessoa++;
  sem_post(&mutexVariaveisMonitor);
  return m;
}

void FilaEspera(struct pessoa *pessoa) {
  char mensagem[TAMANHO_LINHA];
  long timestamp = minutosDecorridos;
  int index, tempoEspera;
  int valorSemaforo; //Fica com o tamanho do semaforo
  char *tipoPessoa;
  pthread_mutex_lock(&mutexVariaveisCentros);
  if (pessoa->centroTeste == 1) { // CENTRO TESTES 1
    if (centroTestes1.numeroPessoasEspera < configuracao.tamanhoFilaCentro1) { //Se o numero de pessoas na fila de espera for menor que o tamanho da fila avança
      tipoPessoa = printTipoPessoa(pessoa);
      free(tipoPessoa);
      if (pessoa->numeroPessoasAFrenteParaDesistir < centroTestes1.numeroPessoasEspera) { //Se o numero de pessoas na fila de espera for maior que o numero de pessoas a frente 
                                                                                          //que essa pessoa admite, ela desiste
        pthread_mutex_unlock(&mutexVariaveisCentros);
        tipoPessoa = printTipoPessoa(pessoa);
        free(tipoPessoa);
        pessoa->desistiu = TRUE; //Pessoa desiste
      } else { //Senão é aumentado o numero de pessoas em espera, calculado e registado o tempo de maximo de espera dessa pessoa e mais tarde verificado se a pessoa desiste a espera ou é testada
        pessoa->tempoChegadaFilaEspera = timestamp;
        centroTestes1.numeroPessoasEspera++; //É aumentado o numero de pessoas em espera
        pthread_mutex_unlock(&mutexVariaveisCentros);
        sem_wait(&centroTestes1.filaEspera);
        pthread_mutex_lock(&mutexVariaveisSimulacao);
        tempoEspera = minutosDecorridos - pessoa->tempoChegadaFilaEspera; //Calculado e registado o tempo de maximo de espera dessa pessoa 
        pthread_mutex_unlock(&mutexVariaveisSimulacao);
        if (tempoEspera > pessoa->tempoMaximoEspera) { // Se passou muito tempo à espera, a pessoa desiste
          pessoa->desistiu = TRUE; //Pessoa desiste
          pthread_mutex_lock(&mutexVariaveisCentros);
          centroTestes1.numeroPessoasEspera--; //É diminuido o número de pessoas em espera
          sem_getvalue(&centroTestes1.filaEspera, &valorSemaforo);
          if (valorSemaforo < centroTestes1.numeroPostosDisponiveis) { //Liberta uma pessoa da fila caso haja posto de testagem disponiveis
            sem_post(&centroTestes1.filaEspera);
          }
          pthread_mutex_unlock(&mutexVariaveisCentros);
          tipoPessoa = printTipoPessoa(pessoa);
          free(tipoPessoa);
        } else { // Senão, não desiste e vai ser testada
          tipoPessoa = printTipoPessoa(pessoa);
          free(tipoPessoa);
          pthread_mutex_lock(&mutexVariaveisCentros);
          centroTestes1.numeroPessoasEspera--; //É diminuido o número de pessoas em espera
          for (index = 0; index < configuracao.numeroPontosTestagemCentro1; index++) {
            if (tempoCooldownPontosTestagemCentro1[index] == -1) // Verifica qual o posto testagem livre e começa-se o cooldown
            {
              centroTestes1.numeroPostosDisponiveis--; //Diminui o numero de postos disponiveis
              tempoCooldownPontosTestagemCentro1[index] = configuracao.tempoCooldownPontosTestagem; //É atribuito o cooldown ao posto escolhido
              break;
            }
          }
          pthread_mutex_unlock(&mutexVariaveisCentros);
          pessoa->estado = ISOLAMENTO; //A pessoa é enviada para isolamento
          sem_wait(&mutexVariaveisMonitor);
          pessoasEmIsolamento++; //O numero de pessoas em isolamento aumenta
          casosEmEstudo++; //O numero de casos em estudo aumenta
          numeroPessoasTestadas++; //O numero de pessoas testadas aumenta
          somaTemposEspera += tempoEspera; //É somado o tempo de espera que essa pessoa esteve sujeita para depois ser feita a média
          sem_post(&mutexVariaveisMonitor);
        }
      }
    } else {
      pthread_mutex_unlock(&mutexVariaveisCentros);
    }
  } else { // CENTRO TESTES 2
    int numeroPessoasEsperaCentro2 = centroTestes2.numeroPessoasNormalEspera + centroTestes2.numeroPessoasPrioritariasEspera; //O numero de pessoas em espera do centro 2 é a soma do numero de
                                                                                                                              //pessoas normais em espera com o numero de pessoas prioritarias
                                                                                                                              //em espera
    pthread_mutex_unlock(&mutexVariaveisCentros);
    if (numeroPessoasEsperaCentro2 < configuracao.tamanhoFilaCentro2) { //Se o numero de pessoas na fila de espera for menor que o tamanho da fila avança
      tipoPessoa = printTipoPessoa(pessoa);
      free(tipoPessoa);
      if (pessoa->numeroPessoasAFrenteParaDesistir < numeroPessoasEsperaCentro2) { //Se o numero de pessoas na fila de espera for maior que o numero de pessoas a frente 
                                                                                   //que essa pessoa admite, ela desiste
        tipoPessoa = printTipoPessoa(pessoa);
        free(tipoPessoa);
        pessoa->desistiu = TRUE; //Pessoa desiste
      } else { //Senão, é verificado se a pessoa é um idoso ou não para depois ser aumentada a respetiva fila de espera (normal ou prioritaria), 
               //calculado e registado o tempo de maximo de espera dessa pessoa e mais tarde verificado se a pessoa desiste a espera ou é testada
        pessoa->tempoChegadaFilaEspera = timestamp;
        int aux;
        if (pessoa->idoso) { //Verifica se é um idoso ou não
          pthread_mutex_lock(&mutexVariaveisCentros);
          centroTestes2.numeroPessoasPrioritariasEspera++;
          if (!(centroTestes2.numeroPessoasPrioritariasEspera == 0 || idososTestadosConsecutivamente >= 5)) { //Se o numero de pessoas na fila de espera prioritaria for diferente de 0 e
                                                                                                              //o numero de idosos testados consecutivamente for menor que 5 então a fila de espera
                                                                                                              //normal é colocada em pausa
            sem_getvalue(&centroTestes2.filaEsperaNormal, &valorSemaforo);
            for (aux = 0; aux < valorSemaforo; aux++) {
              sem_wait(&centroTestes2.filaEsperaNormal);
            }
          }
          pthread_mutex_unlock(&mutexVariaveisCentros);
          sem_wait(&centroTestes2.filaEsperaPrioritaria);
        } else { //Se não for idoso
          pthread_mutex_lock(&mutexVariaveisCentros);
          centroTestes2.numeroPessoasNormalEspera++; //Aumenta o numero de pessoas da fila de espera normal
          pthread_mutex_unlock(&mutexVariaveisCentros);
          sem_wait(&centroTestes2.filaEsperaNormal);
        }
        pthread_mutex_lock(&mutexVariaveisSimulacao);
        tempoEspera = minutosDecorridos - pessoa->tempoChegadaFilaEspera; //É calculado e registado o tempo de maximo de espera dessa pessoa
        pthread_mutex_unlock(&mutexVariaveisSimulacao);
        if (tempoEspera > pessoa->tempoMaximoEspera) { // Se passou muito tempo à espera, a pessoa desiste
          pessoa->desistiu = TRUE; //Pessoa desistiu
          pthread_mutex_lock(&mutexVariaveisCentros);
          if (pessoa->idoso) { //Se for idoso, ele é retirado da fila prioritaria porque desistiu
            centroTestes2.numeroPessoasPrioritariasEspera--;
          } else { //Se for normal, ele é retirado da fila normal porque desistiu
            centroTestes2.numeroPessoasNormalEspera--;
          }
          int assinalarSemaforoNormal = ((centroTestes2.numeroPessoasPrioritariasEspera == 0 && centroTestes2.numeroPessoasNormalEspera == 0) ||
                                        (centroTestes2.numeroPessoasNormalEspera > 0 && (idososTestadosConsecutivamente >= 5 || centroTestes2.numeroPessoasPrioritariasEspera == 0)));
          int assinalarSemaforoPrioritario = ((centroTestes2.numeroPessoasPrioritariasEspera == 0 && centroTestes2.numeroPessoasNormalEspera == 0) ||
                                        (centroTestes2.numeroPessoasPrioritariasEspera > 0 && (idososTestadosConsecutivamente < 5 || centroTestes2.numeroPessoasNormalEspera == 0)));
          if (assinalarSemaforoNormal) { //Liberta fila normal
            sem_getvalue(&centroTestes2.filaEsperaNormal, &valorSemaforo);
            if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
              sem_post(&centroTestes2.filaEsperaNormal);
            }
          }
          if (assinalarSemaforoPrioritario) { //Liberta fila prioritaria
            sem_getvalue(&centroTestes2.filaEsperaPrioritaria, &valorSemaforo);
            if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
              sem_post(&centroTestes2.filaEsperaPrioritaria);
            }
          }
          pthread_mutex_unlock(&mutexVariaveisCentros);
          tipoPessoa = printTipoPessoa(pessoa);
          free(tipoPessoa);
        } else { // Senão, não desiste e vai ser testada
          tipoPessoa = printTipoPessoa(pessoa);
          free(tipoPessoa);
          pthread_mutex_lock(&mutexVariaveisCentros);
          if (pessoa->idoso) { //Se for idoso, é aumentado o numero de idosos testados consecutivamente e fila normal fica em espera
            idososTestadosConsecutivamente++;
            sem_getvalue(&centroTestes2.filaEsperaNormal, &valorSemaforo);
            if (valorSemaforo > 0) {
              sem_wait(&centroTestes2.filaEsperaNormal);
            }
          } else { //Senão o numero de idosos testados consecutivamente é colocado a 0 e a fila prioritaria fica em espera
            idososTestadosConsecutivamente = 0;
            sem_getvalue(&centroTestes2.filaEsperaPrioritaria, &valorSemaforo);
            if (valorSemaforo > 0) {
              sem_wait(&centroTestes2.filaEsperaPrioritaria);
            }
          }
          if (pessoa->idoso) { //Se for idoso, ele é retirado da fila prioritaria para que seja testado
            centroTestes2.numeroPessoasPrioritariasEspera--;
          } else { //Se for normal, ele é retirado da fila normal para que seja testado
            centroTestes2.numeroPessoasNormalEspera--;
          }
          for (index = 0; index < configuracao.numeroPontosTestagemCentro2; index++) {
            if (tempoCooldownPontosTestagemCentro2[index] == -1) // Verifica qual o posto testagem livre e começa-se o cooldown
            {
              centroTestes2.numeroPostosDisponiveis--; //Diminui o numero de postos disponiveis
              tempoCooldownPontosTestagemCentro2[index] = configuracao.tempoCooldownPontosTestagem; //É atribuito o cooldown ao posto escolhido
              break;
            }
          }
          pthread_mutex_unlock(&mutexVariaveisCentros);
          pessoa->estado = ISOLAMENTO; //A pessoa é enviada para isolamento
          sem_wait(&mutexVariaveisMonitor);
          pessoasEmIsolamento++; //O numero de pessoas em isolamento aumenta
          casosEmEstudo++; //O numero de casos em estudo aumenta
          numeroPessoasTestadas++; //O numero de pessoas testadas aumenta
          somaTemposEspera += tempoEspera; //É somado o tempo de espera que essa pessoa esteve sujeita para depois ser feita a média
          sem_post(&mutexVariaveisMonitor);
        }
      }
    } else {
      pthread_mutex_unlock(&mutexVariaveisCentros);
    }
  }
}

void Pessoa(void *ptr) {
  struct pessoa pessoa = criaPessoa(); //Cria uma pessoa
  PessoasCriadas[pessoa.id] = &pessoa; //Guarda o endereço da pessoa
  char mensagem[TAMANHO_LINHA];
  char *tipoPessoa;
  while (TRUE) {
    FilaEspera(&pessoa); //Entra na função FilaEspera para decidir se a pessoa desiste durante o processo de espera ou termina o processo indo para o isolamento
    if (!pessoa.desistiu) {
      int tempoEsperaTeste = 0;
      if (pessoa.tipoTeste == TESTE_RAPIDO) { //Se o tipo de teste for teste rapido
        tempoEsperaTeste = configuracao.tempoTesteRapido * HORA * 1000;
      } else {
        tempoEsperaTeste = configuracao.tempoTesteNormal * HORA * 1000;
      }
      usleep(tempoEsperaTeste);
      fazerTeste(&pessoa); //Entra na função para saber qual o resultado do teste
      if (pessoa.estadoTeste == POSITIVO) { //Se o teste for positivo
        tipoPessoa = printTipoPessoa(&pessoa);
        printf(AMARELO "%s testou positivo \n" RESET, tipoPessoa); //É dada a mensagem a indicar o resultado
        free(tipoPessoa);
        sem_wait(&mutexVariaveisMonitor);
        casosEmEstudo--; //Diminui o numero de casos em estudo
        casosPositivosAtivos++; //Aumente o numero de casos positivos ativos
        casosPositivosTotal++; // Aumenta o numero de casos positivos total
        sem_post(&mutexVariaveisMonitor);
        sem_init(&pessoa.semaforoPessoa, 0, 0);
        pessoa.numeroDiasDesdePositivo = 0; //Coloca a 0 o numero de dias desde o ultimo caso positivo
        break;
      } else if (pessoa.estadoTeste == NEGATIVO) { //Se o teste for negativo
        tipoPessoa = printTipoPessoa(&pessoa);
        printf(AMARELO "%s testou negativo \n" RESET, tipoPessoa); //É dada a mensagem a indicar o resultado
        free(tipoPessoa);
        sem_wait(&mutexVariaveisMonitor);
        casosEmEstudo--; //Diminui o numero de casos em estudo
        pessoasEmIsolamento--; //Diminui o numero de pessoas em isolamento
        sem_post(&mutexVariaveisMonitor);
        pessoa.estado = SOBREVIVEU; //O estado da pessoa é colocado como sobrevivente
        break;
      } else { //Se o teste for inconclusivo
        tipoPessoa = printTipoPessoa(&pessoa);
        printf(AMARELO "%s estou inconclusivo \n" RESET, tipoPessoa); //É dada a mensagem a indicar o resultado
        free(tipoPessoa);
        sem_wait(&mutexVariaveisMonitor);
        casosEmEstudo--; //Diminui o numero de casos em estudo
        pessoasEmIsolamento--; //Diminui o numero de pessoas em isolamento
        sem_post(&mutexVariaveisMonitor);
        pessoa.estado = ESPERA; //O estado da pessoa é colocado como espera
      }
    } else {
      break;
    }
  }
  if (pessoa.estadoTeste == POSITIVO) { //Se o teste for positivo
    pessoa.numeroDiasDesdePositivo = 0; //Coloca a 0 o numero de dias desde o ultimo caso positivo
    if ((pessoa.idoso && probabilidade(configuracao.probabilidadeIdosoPrecisaHospital)) || 
        (!pessoa.idoso && probabilidade(configuracao.probabilidadeNaoIdosoPrecisaHospital))) { // Vai para o Hospital
      pessoa.estado = HOSPITAL; //O estado da pessoa é colocado hospital
      tipoPessoa = printTipoPessoa(&pessoa);
      printf(CIANO "%s foi transportada para o hospital.\n" RESET, tipoPessoa); //É dada a mensagem que a pessoa foi enviada para o hospital
      free(tipoPessoa);
      pthread_mutex_lock(&mutexVariaveisHospital);
      if (numeroPacientesNoHospital < configuracao.tamanhoHospital) { //Se o numero de pacientes no hospital for menor que o tamanho do hospital, a pessoa é colocada no hospital
        int index;
        for (index = 0; index < configuracao.tamanhoHospital; index++) {
          if (IDsDoentesNoHospital[index] == -1) { 
            IDsDoentesNoHospital[index] = pessoa.id; //A pessoa é colocada numa vaga do hospital
            break;
          }
        }
        sem_post(&semaforoMedicos);
        pthread_mutex_unlock(&mutexVariaveisHospital);
        sem_wait(&mutexVariaveisMonitor);
        pessoasEmIsolamento--; //Diminui o numero de pessoas em isolamento
        numeroPacientesNoHospital++; //Aumenta o numero de pacientes no hospital
        sem_post(&mutexVariaveisMonitor);
        sem_wait(&semaforoDoentes);
        pthread_mutex_lock(&mutexVariaveisHospital);
      }
      pthread_mutex_unlock(&mutexVariaveisHospital);
    }
    sem_wait(&pessoa.semaforoPessoa);
  }
}

void Medico(void *ptr) {
  struct pessoa medico = criaMedico(); //Cria um medico
  PessoasCriadas[medico.id] = &medico; //Guarda o endereço do medico
  char mensagem[TAMANHO_LINHA];
  int tempoEsperaTeste;
  char *tipoPessoa;
  int index;
  sem_init(&medico.semaforoPessoa, 0, 0);
  while (TRUE) {
    sem_wait(&semaforoMedicos);
    tipoPessoa = printTipoPessoa(&medico);
    printf(CIANO "%s vai tratar do doente\n" RESET, tipoPessoa); //É dada a mensagem a indicar que o médico X está a tratar do paciente
    free(tipoPessoa);
    sem_wait(&mutexVariaveisMonitor);
    medicosDisponiveis--; //Diminui o número de médicos disponiveis
    sem_post(&mutexVariaveisMonitor);
    pthread_mutex_lock(&mutexVariaveisHospital);
    for (index = 0; index < configuracao.tamanhoHospital; index++) {
      if (IDsMedicosASerUsados[index] == -1) {
        IDsMedicosASerUsados[index] = medico.id; //O médico é colocado na lista de medicos a ser usados
        break;
      }
    }
    pthread_mutex_unlock(&mutexVariaveisHospital);
    sem_post(&semaforoDoentes);
    sem_wait(&medico.semaforoPessoa); // Medico acabou de tratar de doente ou este morreu -> sai da espera no semaforo
    tempoEsperaTeste = configuracao.tempoTesteRapido * HORA * 1000;
    if (medico.estado == ISOLAMENTO) { //Se o medico estiver no isolamento
      while (TRUE) {
        usleep(tempoEsperaTeste);
        fazerTeste(&medico); //Entra na função para saber qual o resultado do teste
        if (medico.estadoTeste == POSITIVO) { //Se o teste for positivo
          tipoPessoa = printTipoPessoa(&medico);
          printf(BRANCO "%s testou positivo \n" RESET, tipoPessoa); //É dada a mensagem a indicar o resultado
          free(tipoPessoa);
          sem_wait(&mutexVariaveisMonitor);
          casosEmEstudo--; //Diminui o numero de casos em estudo
          casosPositivosAtivos++; //Aumente o numero de casos positivos ativos
          casosPositivosTotal++; // Aumenta o numero de casos positivos total
          sem_post(&mutexVariaveisMonitor);
          medico.numeroDiasDesdePositivo = 0; //Coloca a 0 o numero de dias desde o ultimo caso positivo
          break;
        } else if (medico.estadoTeste == NEGATIVO) { //Se o teste for negativo
          tipoPessoa = printTipoPessoa(&medico);
          printf(BRANCO "%s testou negativo \n" RESET, tipoPessoa); //É dada a mensagem a indicar o resultado
          free(tipoPessoa);
          sem_wait(&mutexVariaveisMonitor);
          casosEmEstudo--; //Diminui o numero de casos em estudo
          pessoasEmIsolamento--; //Diminui o numero de pessoas em isolamento
          medicosDisponiveis++; //Aumenta o número de médicos disponiveis
          sem_post(&mutexVariaveisMonitor);
          medico.estado = SOBREVIVEU; //O estado da médico é colocado como sobrevivente
          break;
        } else {
          tipoPessoa = printTipoPessoa(&medico);
          printf(BRANCO "%s testou inconclusivo \n" RESET, tipoPessoa); //É dada a mensagem a indicar o resultado
          free(tipoPessoa);
          tempoEsperaTeste = configuracao.tempoTesteRapido * HORA * 1000;
        }
      }
    }
    if (medico.estadoTeste == POSITIVO) { //Se o teste for positivo
      medico.numeroDiasDesdePositivo = 0; //Coloca a 0 o numero de dias desde o ultimo caso positivo
      if ((medico.idoso && probabilidade(configuracao.probabilidadeIdosoPrecisaHospital)) ||
          (!medico.idoso && probabilidade(configuracao.probabilidadeNaoIdosoPrecisaHospital))) { // Vai para o Hospital
        medico.estado = HOSPITAL; //O estado do medico é colocado hospital
        tipoPessoa = printTipoPessoa(&medico);
        printf(CIANO "%s foi transportada para o hospital.\n" RESET, tipoPessoa); //É dada a mensagem que o médico foi enviado para o hospital
        free(tipoPessoa);
        pthread_mutex_lock(&mutexVariaveisHospital);
        if (numeroPacientesNoHospital < configuracao.tamanhoHospital) { //Se o numero de pacientes no hospital for menor que o tamanho do hospital, o médico é colocada no hospital
          for (index = 0; index < configuracao.tamanhoHospital; index++) {
            if (IDsDoentesNoHospital[index] == -1) {
              IDsDoentesNoHospital[index] = medico.id; //O médico é colocada numa vaga do hospital
              break;
            }
          }
          sem_post(&semaforoMedicos);
          pthread_mutex_unlock(&mutexVariaveisHospital);
          sem_wait(&mutexVariaveisMonitor);
          pessoasEmIsolamento--; //Diminui o numero de pessoas em isolamento
          numeroPacientesNoHospital++; //Aumenta o numero de pacientes no hospital
          sem_post(&mutexVariaveisMonitor);
          sem_wait(&semaforoDoentes);
          pthread_mutex_lock(&mutexVariaveisHospital);
        }
        pthread_mutex_unlock(&mutexVariaveisHospital);
      }
      sem_wait(&medico.semaforoPessoa);
    }
    medico.estadoTeste = NAO_TESTOU; //O estado da médico é colocado como não testou
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
  int auxTimeStamp, 
      numeroDia = 1;
  int tempoLimite = configuracao.tempoSimulacao * DIA;
  enviarMensagem("Z-0-Z"); // Mensagem que indica o comeco da simulacao
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
  sprintf(mensagem, "%d-%d-%d", configuracao.tamanhoHospital, 11, 0); //É dada uma mensagem com o numero de médicos disponiveis
  strcat(mensagensAEnviar, mensagem);
  enviarMensagem(mensagensAEnviar);
  medicosDisponiveis = configuracao.tamanhoHospital;
  int libertarMedico = FALSE;
  int valorSemaforo;
  int assinalarSemaforoNormal, 
      assinalarSemaforoPrioritario;
  int numeroPessoasRecuperaramHospital = 0,
      numeroPessoasRecuperaramIsolamento = 0, 
      numeroPessoasMorreram = 0,
      numeroMedicosParaIsolamento = 0, 
      numeroMedicosRecuperaram = 0;
  int tempoMedioChegadaCentros = configuracao.tempoMedioChegada;
  int proximoInstanteChegada = numeroAleatorio(tempoMedioChegadaCentros + tempoMedioChegadaCentros / 2, tempoMedioChegadaCentros - tempoMedioChegadaCentros / 2);
  int indexPicos = 0, fimPico = 0;

  if (configuracao.diasPicos[indexPicos] == numeroDia) { //Se o dia atual for igual a um dos dias de pico as probablidades de a população e os medicos darem positivo aumentam
                                                         //O tempo medio de chegadas é divido por 2
                                                         //É aberto mais 1 posto em cada centro
    fimPico = configuracao.diasPicos[indexPicos] + configuracao.duracoesPicos[indexPicos];
    tempoMedioChegadaCentros /= 2;
    configuracao.probabilidadePopulacaoPositivo += 0.1;
    configuracao.probabilidadeMedicoPositivo += 0.1;
    pthread_mutex_lock(&mutexVariaveisCentros);
    centroTestes1.numeroPostosDisponiveis++;
    centroTestes2.numeroPostosDisponiveis++;
    tempoCooldownPontosTestagemCentro1[configuracao.numeroPontosTestagemCentro1] = -1;
    tempoCooldownPontosTestagemCentro2[configuracao.numeroPontosTestagemCentro2] = -1;
    sem_post(&centroTestes1.filaEspera);
    assinalarSemaforoNormal = ((centroTestes2.numeroPessoasPrioritariasEspera == 0 && centroTestes2.numeroPessoasNormalEspera == 0) ||
                              (centroTestes2.numeroPessoasNormalEspera > 0 && (idososTestadosConsecutivamente >= 5 || centroTestes2.numeroPessoasPrioritariasEspera == 0)));
    assinalarSemaforoPrioritario = ((centroTestes2.numeroPessoasPrioritariasEspera == 0 && centroTestes2.numeroPessoasNormalEspera == 0) ||
                                   (centroTestes2.numeroPessoasPrioritariasEspera > 0 && (idososTestadosConsecutivamente < 5 || centroTestes2.numeroPessoasNormalEspera == 0)));
    if (assinalarSemaforoNormal) { 
      sem_getvalue(&centroTestes2.filaEsperaNormal, &valorSemaforo);
      if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
        sem_post(&centroTestes2.filaEsperaNormal); //Liberta fila normal
      }
    }
    if (assinalarSemaforoPrioritario) { 
      sem_getvalue(&centroTestes2.filaEsperaPrioritaria, &valorSemaforo);
      if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
        sem_post(&centroTestes2.filaEsperaPrioritaria); //Liberta fila prioritaria
      }
    }
    pthread_mutex_unlock(&mutexVariaveisCentros);
    printf("---------------------------------------------INICIO PICO--------------------------------------------\n");
  }
  while (tempoDecorrido != tempoLimite) { //Enquanto o tempo decorrido não for igual ao tempo limite a simulação continua
    auxTimeStamp = current_timestamp();
    if (auxTimeStamp != timeStampAnterior) { //Se o tempo atual for diferente do tempo anterior, aumenta o tempo
      tempoDecorrido++; //Aumenta o tempo decorrido
      timeStampAnterior = auxTimeStamp;
      if (tempoDecorrido % MINUTO == 0) { // Passou um minuto na simulação e é enviada as mensagens para o monitor
        pthread_mutex_lock(&mutexVariaveisSimulacao);
        minutosDecorridos++; //Aumenta o numero de minutos decorridos
        pthread_mutex_unlock(&mutexVariaveisSimulacao);
        mensagensAEnviar[0] = '\0';
        pthread_mutex_lock(&mutexVariaveisCentros);
        sprintf(mensagem, "%d-%d-%d", centroTestes1.numeroPessoasEspera, 0, 1);
        strcat(mensagensAEnviar, mensagem);
        strcat(mensagensAEnviar, "/");
        sprintf(mensagem, "%d-%d-%d", centroTestes2.numeroPessoasNormalEspera + centroTestes2.numeroPessoasPrioritariasEspera, 0, 2);
        pthread_mutex_unlock(&mutexVariaveisCentros);
        strcat(mensagensAEnviar, mensagem);
        strcat(mensagensAEnviar, "/");
        sprintf(mensagem, "%d-%d-%d", idPessoa - configuracao.tamanhoHospital, 5, 0);
        strcat(mensagensAEnviar, mensagem);
        strcat(mensagensAEnviar, "/");
        sem_wait(&mutexVariaveisMonitor);
        sprintf(mensagem, "%d-%d-%d", casosPositivosAtivos, 1, 0);
        strcat(mensagensAEnviar, mensagem);
        strcat(mensagensAEnviar, "/");
        sprintf(mensagem, "%d-%d-%d", casosPositivosTotal, 2, 0);
        strcat(mensagensAEnviar, mensagem);
        strcat(mensagensAEnviar, "/");
        sprintf(mensagem, "%d-%d-%d", casosEmEstudo, 3, 0);
        strcat(mensagensAEnviar, mensagem);
        strcat(mensagensAEnviar, "/");
        sprintf(mensagem, "%d-%d-%d", pessoasEmIsolamento, 4, 0);
        strcat(mensagensAEnviar, mensagem);
        strcat(mensagensAEnviar, "/");
        sprintf(mensagem, "%d-%d-%d", numeroPacientesNoHospital, 6, 0);
        strcat(mensagensAEnviar, mensagem);
        strcat(mensagensAEnviar, "/");
        sprintf(mensagem, "%d-%d-%d", medicosDisponiveis, 11, 0);
        strcat(mensagensAEnviar, mensagem);
        if (numeroPessoasTestadas > 0) { //Se o numero de pessoas testadas for maior que 0, é enviada a mensagem com a media de tempo de espera
          tempoMedioEspera = somaTemposEspera / numeroPessoasTestadas;
          strcat(mensagensAEnviar, "/");
          sprintf(mensagem, "%d-%d-%d", tempoMedioEspera, 7, 0);
          strcat(mensagensAEnviar, mensagem);
        }
        sem_post(&mutexVariaveisMonitor);
        pthread_mutex_lock(&mutexVariaveisCentros);
        for (index = 0; index < configuracao.numeroPontosTestagemCentro1 + 1; index++) {
          if (tempoCooldownPontosTestagemCentro1[index] == 0) { //Se o tempo de cooldown do posto for igual a 0, esse posto é colocado como disponivel
            centroTestes1.numeroPostosDisponiveis++;
            sem_post(&centroTestes1.filaEspera);
            tempoCooldownPontosTestagemCentro1[index]--;
          } else if (tempoCooldownPontosTestagemCentro1[index] > 0) { // Se o tempo de cooldown do posto for maior que 0, é feita a contagem decrescente do cooldown 
            tempoCooldownPontosTestagemCentro1[index]--;
          }
        }
        for (index = 0; index < configuracao.numeroPontosTestagemCentro2 + 1; index++) {
          if (tempoCooldownPontosTestagemCentro2[index] == 0) { //Se o tempo de cooldown atinge os 0, o posto é colocado como disponivel
            centroTestes2.numeroPostosDisponiveis++; //É aumentado o numero de postos disponiveis
            assinalarSemaforoNormal = ((centroTestes2.numeroPessoasPrioritariasEspera == 0 && centroTestes2.numeroPessoasNormalEspera == 0) ||
                 (centroTestes2.numeroPessoasNormalEspera > 0 && (idososTestadosConsecutivamente >= 5 || centroTestes2.numeroPessoasPrioritariasEspera == 0)));
            assinalarSemaforoPrioritario = ((centroTestes2.numeroPessoasPrioritariasEspera == 0 && centroTestes2.numeroPessoasNormalEspera == 0) ||
                 (centroTestes2.numeroPessoasPrioritariasEspera > 0 && (idososTestadosConsecutivamente < 5 || centroTestes2.numeroPessoasNormalEspera == 0)));
            if (assinalarSemaforoNormal) { 
              sem_getvalue(&centroTestes2.filaEsperaNormal, &valorSemaforo);
              if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
                sem_post(&centroTestes2.filaEsperaNormal); //Liberta fila normal
              }
            }
            if (assinalarSemaforoPrioritario) { 
              sem_getvalue(&centroTestes2.filaEsperaPrioritaria, &valorSemaforo);
              if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
                sem_post(&centroTestes2.filaEsperaPrioritaria); //Liberta fila prioritaria
              }
            }
            tempoCooldownPontosTestagemCentro2[index]--; //Coloca o tempo de cooldown a -1;
          } else if (tempoCooldownPontosTestagemCentro2[index] > 0) { // Se o tempo de cooldown do posto for maior que 0, é feita a contagem decrescente do cooldown 
            tempoCooldownPontosTestagemCentro2[index]--;
          }
        }
        pthread_mutex_unlock(&mutexVariaveisCentros);
        if (minutosDecorridos % proximoInstanteChegada == 0) {
          proximoInstanteChegada = numeroAleatorio(tempoMedioChegadaCentros + tempoMedioChegadaCentros / 2,tempoMedioChegadaCentros - tempoMedioChegadaCentros / 2);
          // Cria tarefas pessoas
          if (pthread_create(&IDtarefa[idPessoa], NULL, Pessoa, NULL)) {
            printf("Erro na criação da tarefa\n");
            exit(1);
          }
        }
        if (minutosDecorridos % (60 * 24) == 0) { //Se o minutos decorridos for igual a 1 dia
          printf("---------------------------------------------DIA %d---------------------------------------------\n",++numeroDia); //É indicado o dia
          sprintf(mensagem, "%d-%d-%d", 0, 10, 0);
          strcat(mensagensAEnviar, "/");
          strcat(mensagensAEnviar, mensagem);
          pthread_mutex_lock(&mutexVariaveisHospital);
          numeroPessoasRecuperaramHospital = 0,
          numeroPessoasRecuperaramIsolamento = 0, 
          numeroPessoasMorreram = 0,
          numeroMedicosParaIsolamento = 0, 
          numeroMedicosRecuperaram = 0;
          for (index = 0; index < configuracao.tamanhoHospital; index++) {
            libertarMedico = FALSE; //O médico está ocupado
            if (IDsDoentesNoHospital[index] != -1) { // Se há doente no hospital
              if (PessoasCriadas[IDsDoentesNoHospital[index]]->numeroDiasDesdePositivo == configuracao.tempoCurar) { // Se já passou o tempo para curar
                tipoPessoa = printTipoPessoa(PessoasCriadas[IDsDoentesNoHospital[index]]);
                printf(CIANO "%s recuperou\n" RESET, tipoPessoa);
                free(tipoPessoa);
                sem_post(&PessoasCriadas[IDsDoentesNoHospital[index]]->semaforoPessoa);
                numeroPessoasRecuperaramHospital++; //Aumenta o numero de pessoas que recuperaram no hospital
                numeroMedicosRecuperaram += PessoasCriadas[IDsDoentesNoHospital[index]]->medico; //Se a pessoa for médico (+1), é aumentado o numero de medicos que recuperaram
                PessoasCriadas[IDsDoentesNoHospital[index]]->estado = SOBREVIVEU; //O estado da pessoa para a ser sobreviviu
                IDsDoentesNoHospital[index] = -1; //A vaga do hospital passa a estar livre
                libertarMedico = TRUE; //O medico está livre
              } else { //Se ainda não passou o tempo para curar
                if (PessoasCriadas[IDsDoentesNoHospital[index]]->idoso) { //Se a pessoa for idosa
                  int pessoaMorreu = IDsMedicosASerUsados[index] != 0
                          ? probabilidade(configuracao.probabilidadeIdosoMorrer / 4)
                          : probabilidade(configuracao.probabilidadeIdosoMorrer);
                  if (pessoaMorreu) { //Se a pessoa morreu
                    sem_post(&PessoasCriadas[IDsDoentesNoHospital[index]]->semaforoPessoa);
                    tipoPessoa = printTipoPessoa(PessoasCriadas[IDsDoentesNoHospital[index]]);
                    printf(CIANO "%s morreu \n" RESET, tipoPessoa);
                    free(tipoPessoa);
                    numeroPessoasMorreram++; //É aumentado o número de pessoas que morreram
                    PessoasCriadas[IDsDoentesNoHospital[index]]->estado = MORREU; //O estado da pessoa passa para morreu
                    IDsDoentesNoHospital[index] = -1; //A vaga do hospital passa a estar livre
                    libertarMedico = TRUE; //O medico está livre
                  }
                } else { //Se a pessoa não for idosa
                  int pessoaMorreu = IDsMedicosASerUsados[index] != 0
                          ? probabilidade(configuracao.probabilidadeNaoIdosoMorrer / 4)
                          : probabilidade(configuracao.probabilidadeNaoIdosoMorrer);
                  if (pessoaMorreu) { //Se a pessoa morreu
                    sem_post(&PessoasCriadas[IDsDoentesNoHospital[index]]->semaforoPessoa);
                    tipoPessoa = printTipoPessoa(PessoasCriadas[IDsDoentesNoHospital[index]]);
                    printf(CIANO "%s morreu \n" RESET, tipoPessoa);
                    free(tipoPessoa);
                    numeroPessoasMorreram++; //É aumentado o número de pessoas que morreram
                    PessoasCriadas[IDsDoentesNoHospital[index]]->estado = MORREU; //O estado da pessoa passa para morreu
                    IDsDoentesNoHospital[index] = -1; //A vaga do hospital passa a estar livre
                    libertarMedico = TRUE; //O medico está livre
                  }
                }
              }
              if (IDsMedicosASerUsados[index] != -1 && libertarMedico) { //Se o médico tiver sido utilizado e já estiver livre, é enviado para isolamento 
                numeroMedicosParaIsolamento++; //É aumentado o número de médicos em isolamento
                PessoasCriadas[IDsMedicosASerUsados[index]]->estado =  ISOLAMENTO; //O estado do médico passa a ser isolamento
                sem_post(&PessoasCriadas[IDsMedicosASerUsados[index]] ->semaforoPessoa);
                IDsMedicosASerUsados[index] = -1; //A posição que o medico estava a utilizar na lista de médicos a ser utilizados passa a ficar livre para que possa ser preenchido
                                                  //por um novo médico
              }
            }
            if (IDsDoentesNoHospital[index] != -1) { // Se a pessoa não recuperou e não morreu, aumentar o
                                                     // contador
              PessoasCriadas[IDsDoentesNoHospital[index]]->numeroDiasDesdePositivo++; //É aumentado o numero de dias desde que a pessoa teve teste positivo
            }
          }
          pthread_mutex_unlock(&mutexVariaveisHospital);
          int idPessoaAtual = idPessoa;
          for (index = 0; index < idPessoaAtual; index++) {
            if (PessoasCriadas[index] != NULL && PessoasCriadas[index]->estado == ISOLAMENTO && PessoasCriadas[index]->estadoTeste == POSITIVO) { //Se a pessoa se encontra em isolamento com o
                                                                                                                                                  //teste positivo
              if (PessoasCriadas[index]->numeroDiasDesdePositivo == configuracao.tempoCurar) { // Se já passou o tempo para curar
                tipoPessoa = printTipoPessoa(PessoasCriadas[index]);
                printf(AZUL "%s recuperou em isolamento.\n" RESET, tipoPessoa);
                free(tipoPessoa);
                PessoasCriadas[index]->estado = SOBREVIVEU; //O estado da pessoa passa a ser sobreviveu
                PessoasCriadas[index]->estadoTeste = NAO_TESTOU; //O estado do teste da pessoa passa a ser nao testou
                numeroPessoasRecuperaramIsolamento++; //É aumentado o numero de pessoas que recuperaram no isolamento
                numeroMedicosRecuperaram += PessoasCriadas[index]->medico; //Caso a pessoa seja médico (+1) é aumentado o numero de medicos que recuperaram
                sem_post(&PessoasCriadas[index]->semaforoPessoa);
              } else { //Se ainda não passou o tempo para curar 
                PessoasCriadas[index]->numeroDiasDesdePositivo++; //É aumentado o numero de dias desde que a pessoa teve teste positivo
              }
            }
          }
          sem_wait(&mutexVariaveisMonitor);
          casosRecuperados += numeroPessoasRecuperaramIsolamento + numeroPessoasRecuperaramHospital; //O numero de casos recuperados é a soma do numero de pessoas que recuperaram no isolamento
                                                                                                     //, o numero de pessoas que recuperaram no hostipal e o numero de casos recuperados anteriormente
          numeroPacientesNoHospital -= (numeroPessoasRecuperaramHospital + numeroPessoasMorreram); //O numero de pacientes no hostipal é a subtração do numero de pacientes no hospital anteriormente
                                                                                                   //e a soma do numero de pessoas que recuperarm no hospital e o numero de pessoas que morreram no hospital
          pessoasEmIsolamento -= numeroPessoasRecuperaramIsolamento; //O numero de pessoas em isolamento é subtraçao do numero de pessoas em isolamento anteriomente e o numero de pessoas que
                                                                     //recuperaram no isolamento
          pessoasEmIsolamento += numeroMedicosParaIsolamento; //O numero de pessoas em isolamento é a soma do numero de pessoas em isolamento anteriormente  e o numero de medicos para isolamento
          medicosDisponiveis += numeroMedicosRecuperaram; //O numero de medicos disponiveis é a soma de medicos disponiveis anteriormente e o numero de medicos que recuperaram
          casosEmEstudo += numeroMedicosParaIsolamento;  //O numero de casos em estudo é a soma do numero de casos em estudo anteriormente e o numero de medicos para isolamento
          casosPositivosAtivos -= (numeroPessoasRecuperaramHospital + numeroPessoasMorreram + numeroPessoasRecuperaramIsolamento); //O numero de casos postivos ativos é subtração do numero de
                                                                                                                                   //casos positivos ativos anteriormente e a soma do numero de
                                                                                                                                   //pessoas que recuperaram no hospital, o numero de pessoas que
                                                                                                                                   //morreram e o numero de pessoas que recuperaram no isolamento
          numeroMortos += numeroPessoasMorreram; //O numero de mortos é a soma do numero de mortos anteriomente e o numero de pessoas que morreram
          sem_post(&mutexVariaveisMonitor);
          strcat(mensagensAEnviar, "/");
          sprintf(mensagem, "%d-%d-%d", casosRecuperados, 8, 0);
          strcat(mensagensAEnviar, mensagem);
          strcat(mensagensAEnviar, "/");
          sprintf(mensagem, "%d-%d-%d", numeroMortos, 9, 0);
          strcat(mensagensAEnviar, mensagem);
          if (configuracao.diasPicos[indexPicos] == numeroDia) { //Se o dia atual for igual a um dos dias de pico as probablidades de a população e os medicos darem positivo aumentam
                                                                 //O tempo medio de chegadas é divido por 2
                                                                 //É aberto mais 1 posto em cada centro
            fimPico = configuracao.diasPicos[indexPicos] + configuracao.duracoesPicos[indexPicos];
            tempoMedioChegadaCentros /= 2;
            configuracao.probabilidadePopulacaoPositivo += 0.1;
            configuracao.probabilidadeMedicoPositivo += 0.1;
            pthread_mutex_lock(&mutexVariaveisCentros);
            centroTestes1.numeroPostosDisponiveis++;
            centroTestes2.numeroPostosDisponiveis++;
            tempoCooldownPontosTestagemCentro1[configuracao.numeroPontosTestagemCentro1] = -1;
            tempoCooldownPontosTestagemCentro2[configuracao.numeroPontosTestagemCentro2] = -1;
            sem_post(&centroTestes1.filaEspera);
            assinalarSemaforoNormal = ((centroTestes2.numeroPessoasPrioritariasEspera == 0 && centroTestes2.numeroPessoasNormalEspera == 0) ||
                 (centroTestes2.numeroPessoasNormalEspera > 0 && (idososTestadosConsecutivamente >= 5 || centroTestes2.numeroPessoasPrioritariasEspera == 0)));
            assinalarSemaforoPrioritario = ((centroTestes2.numeroPessoasPrioritariasEspera == 0 && centroTestes2.numeroPessoasNormalEspera == 0) ||
                 (centroTestes2.numeroPessoasPrioritariasEspera > 0 && (idososTestadosConsecutivamente < 5 || centroTestes2.numeroPessoasNormalEspera == 0)));
            if (assinalarSemaforoNormal) {
              sem_getvalue(&centroTestes2.filaEsperaNormal, &valorSemaforo);
              if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
                sem_post(&centroTestes2.filaEsperaNormal); //Liberta fila normal
              }
            }
            if (assinalarSemaforoPrioritario) {
              sem_getvalue(&centroTestes2.filaEsperaPrioritaria,
                           &valorSemaforo);
              if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
                sem_post(&centroTestes2.filaEsperaPrioritaria); //Liberta fila prioritaria
              }
            }
            pthread_mutex_unlock(&mutexVariaveisCentros);
            printf("---------------------------------------------INICIO PICO--------------------------------------------\n");
          } else if (fimPico == numeroDia) { //Se o dia atual for igual a um dos dias de fim de pico as probablidades de a população e os medicos darem positivo voltam ao normal
                                             //O tempo medio de chegadas volta ao normal
                                             //E o numero de postos por centro voltam ao normal
            tempoMedioChegadaCentros *= 2;
            configuracao.probabilidadePopulacaoPositivo -= 0.1;
            configuracao.probabilidadeMedicoPositivo -= 0.1;
            indexPicos++;
            fimPico = 0;
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
        enviarMensagem(mensagensAEnviar);
      }
    }
  }
  enviarMensagem("Z-1-Z"); // Mensagem que indica o fim da simulacao
}

void iniciarSemaforosETrincos() {
  if (pthread_mutex_init(&mutexCriarPessoa, NULL) != 0) { //Mensagem de erro
    printf("Inicializacao do trinco falhou.\n");
  }
  if (pthread_mutex_init(&mutexVariaveisCentros, NULL) != 0) { //Mensagem de erro
    printf("Inicializacao do trinco falhou.\n");
  }
  if (pthread_mutex_init(&mutexVariaveisSimulacao, NULL) != 0) { //Mensagem de erro
    printf("Inicializacao do trinco falhou.\n");
  }
  if (pthread_mutex_init(&mutexVariaveisHospital, NULL) != 0) { //Mensagem de erro
    printf("Inicializacao do trinco falhou.\n");
  }
  //Inicialização dos semaforos e trincos
  sem_init(&centroTestes1.filaEspera, 0, configuracao.numeroPontosTestagemCentro1);
  sem_init(&mutexVariaveisMonitor, 0, 1);
  sem_init(&semaforoEnviarMensagem, 0, 1);
  centroTestes1.numeroPostosDisponiveis = configuracao.numeroPontosTestagemCentro1;
  tempoCooldownPontosTestagemCentro1 = (int *)calloc(configuracao.numeroPontosTestagemCentro1 + 1, sizeof(int));
  int index;
  for (index = 0; index < configuracao.numeroPontosTestagemCentro1; index++) {
    tempoCooldownPontosTestagemCentro1[index] = -1;
  }
  tempoCooldownPontosTestagemCentro1[index] = -2;
  sem_init(&centroTestes2.filaEsperaPrioritaria, 0, configuracao.numeroPontosTestagemCentro2);
  sem_init(&centroTestes2.filaEsperaNormal, 0, configuracao.numeroPontosTestagemCentro2);
  centroTestes2.numeroPostosDisponiveis = configuracao.numeroPontosTestagemCentro2;
  tempoCooldownPontosTestagemCentro2 = (int *)calloc(configuracao.numeroPontosTestagemCentro2 + 1, sizeof(int));
  for (index = 0; index < configuracao.numeroPontosTestagemCentro2; index++) {
    tempoCooldownPontosTestagemCentro2[index] = -1;
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
  } else if (strcmp(nomeFicheiro, "simulador.conf") !=
             0) // se strcmp retorn 0, então os string são iguais
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
  configuracao.tempoTesteNormal = strtol(values[1], NULL, 10);
  configuracao.tempoTesteRapido = strtol(values[2], NULL, 10);
  configuracao.tempoCooldownPontosTestagem = strtol(values[3], NULL, 10);
  configuracao.tempoMaximoEsperaMedio = strtol(values[4], NULL, 10);
  configuracao.tamanhoFilaCentro1 = strtol(values[5], NULL, 10);
  configuracao.tamanhoFilaCentro2 = strtol(values[6], NULL, 10);
  configuracao.numeroPontosTestagemCentro1 = strtol(values[7], NULL, 10);
  configuracao.numeroPontosTestagemCentro2 = strtol(values[8], NULL, 10);
  configuracao.tamanhoHospital = strtol(values[9], NULL, 10);
  configuracao.probabilidadeSerIdoso = strtof(values[10], NULL);
  configuracao.probabilidadeMedicoPositivo = strtof(values[11], NULL);
  configuracao.probabilidadePopulacaoPositivo = strtof(values[12], NULL);
  configuracao.probabilidadeTesteNormalInconclusivo = strtof(values[13], NULL);
  configuracao.probabilidadeTesteRapidoInconclusivo = strtof(values[14], NULL);
  configuracao.probabilidadeIdosoPrecisaHospital = strtof(values[15], NULL);
  configuracao.probabilidadeNaoIdosoPrecisaHospital = strtof(values[16], NULL);
  configuracao.probabilidadeIdosoMorrer = strtof(values[17], NULL);
  configuracao.probabilidadeNaoIdosoMorrer = strtof(values[18], NULL);
  configuracao.tempoCurar = strtol(values[19], NULL, 10);
  configuracao.tempoSimulacao = strtol(values[20], NULL, 10);
  char *auxPicos = values[21];
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

int main(int argc, char *argv[]) {

  if (argc == 2) {
    socketfd = criaSocket(); //Cria o socket
    simulacao(argv[1]); //Inicia a simulação
    close(socketfd); //Fecha o socket
    return 0;
  } else {
    perror("ERRO!\n(Só) É preciso passar como argumento o ficheiro de "
           "configuração.");
    exit(EXIT_FAILURE);
  }
}