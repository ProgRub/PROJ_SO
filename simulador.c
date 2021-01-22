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

int numeroAleatorio(int numeroMaximo, int numeroMinimo) {
  return rand() % (numeroMaximo + 1 - numeroMinimo) + numeroMinimo;
}
int probabilidade(float prob) { return numeroAleatorio(100, 0) < (prob * 100); }
long long current_timestamp() {
  struct timeval te;
  gettimeofday(&te, NULL); // get current time
  long long milliseconds =
      te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
  // printf("milliseconds: %lld\n", milliseconds);
  return milliseconds;
}
void fazerTeste(struct pessoa *pessoa) {
  float probabilidadeInconclusivo = 0.0;
  if (pessoa->tipoTeste == TESTE_NORMAL) {
    probabilidadeInconclusivo =
        configuracao.probabilidadeTesteNormalInconclusivo;
  } else {
    probabilidadeInconclusivo =
        configuracao.probabilidadeTesteRapidoInconclusivo;
  }
  if (probabilidade(probabilidadeInconclusivo)) { // Teste é inconclusivo
    pessoa->estadoTeste = INCONCLUSIVO;
  } else {
    if (!pessoa->medico) { // se a pessoa a ser testada não é médica
      if (probabilidade(
              configuracao
                  .probabilidadePopulacaoPositivo)) { // pessoa testou positivo
        pessoa->estadoTeste = POSITIVO;
      } else {
        pessoa->estadoTeste = NEGATIVO;
      }
    } else { // se a pessoa a ser testada é médica
      if (probabilidade(
              configuracao
                  .probabilidadeMedicoPositivo)) { // médico testou positivo
        pessoa->estadoTeste = POSITIVO;
      } else {
        pessoa->estadoTeste = NEGATIVO;
      }
    }
  }
}
char *printTipoPessoa(struct pessoa *pessoa) {
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

struct pessoa criaPessoa() {

  struct pessoa p;
  p.id = idPessoa;
  p.medico = FALSE;
  p.centroTeste = numeroAleatorio(2, 1);
  p.estadoTeste = NAO_TESTOU;
  p.tipoTeste = TESTE_NORMAL;
  p.idoso = probabilidade(configuracao.probabilidadeSerIdoso);
  if (p.idoso) {
    p.centroTeste = probabilidade(0.90) +
                    1; // pessoas idosas preferem o centro de testes 2, o
                       // prioritário, probabilidade tenderá a dar 1+1=2
  } else {
    p.centroTeste = probabilidade(0.10) +
                    1; // pessoas normais preferem o centro de testes 1,
                       // probabilidade tenderá a dar 0 + 1 = centro de testes 1
  }
  p.desistiu = FALSE;
  if (p.centroTeste == 1) {
    p.numeroPessoasAFrenteParaDesistir =
        numeroAleatorio(configuracao.tamanhoFilaCentro1,
                        (configuracao.tamanhoFilaCentro1 * 3) / 4);
  } else {
    p.numeroPessoasAFrenteParaDesistir =
        numeroAleatorio(configuracao.tamanhoFilaCentro2,
                        (configuracao.tamanhoFilaCentro2 * 3) / 4);
  }
  p.tempoMaximoEspera =
      numeroAleatorio(configuracao.tempoMaximoEsperaMedio,
                      (configuracao.tempoMaximoEsperaMedio * 3) / 4);
  p.estado = ESPERA;
  sem_wait(&mutexVariaveisMonitor);
  idPessoa++;
  sem_post(&mutexVariaveisMonitor);
  return p;
}
struct pessoa criaMedico() {
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
  int valorSemaforo = -1;
  char *tipoPessoa;
  pthread_mutex_lock(&mutexVariaveisCentros);
  if (pessoa->centroTeste == 1) { // CENTRO TESTES 1
    if (centroTestes1.numeroPessoasEspera < configuracao.tamanhoFilaCentro1) {
      tipoPessoa = printTipoPessoa(pessoa);
      // printf("%s chegou a fila do centro 1.\n", tipoPessoa);
      free(tipoPessoa);
      // sprintf(mensagem, "%d-%d-%d", 0, 0, 1);
      // enviarMensagem(mensagem);
      if (pessoa->numeroPessoasAFrenteParaDesistir <
          centroTestes1.numeroPessoasEspera) {
        pthread_mutex_unlock(&mutexVariaveisCentros);
        tipoPessoa = printTipoPessoa(pessoa);
        // printf(VERMELHO "%s desistiu da fila do 1 porque "
        //                 "tinha muita gente a frente.\n" RESET,
        //        tipoPessoa);
        free(tipoPessoa);
        pessoa->desistiu = TRUE;
      } else {
        pessoa->tempoChegadaFilaEspera = timestamp;
        centroTestes1.numeroPessoasEspera++;
        // if (centroTestes1.numeroPessoasEspera >
        //     configuracao.tamanhoFilaCentro1 - 5) {
        //   pessoa->tipoTeste = TESTE_RAPIDO;
        // }
        pthread_mutex_unlock(&mutexVariaveisCentros);
        sem_wait(&centroTestes1.filaEspera);
        pthread_mutex_lock(&mutexVariaveisSimulacao);
        tempoEspera = minutosDecorridos - pessoa->tempoChegadaFilaEspera;
        pthread_mutex_unlock(&mutexVariaveisSimulacao);
        if (tempoEspera > pessoa->tempoMaximoEspera) { // passou muito tempo à
          // espera, a pessoa desiste
          pessoa->desistiu = TRUE;
          pthread_mutex_lock(&mutexVariaveisCentros);
          centroTestes1.numeroPessoasEspera--;
          sem_getvalue(&centroTestes1.filaEspera, &valorSemaforo);
          if (valorSemaforo < centroTestes1.numeroPostosDisponiveis) {
            sem_post(&centroTestes1.filaEspera);
          }
          pthread_mutex_unlock(&mutexVariaveisCentros);
          tipoPessoa = printTipoPessoa(pessoa);
          // printf(VERMELHO "%s desistiu no centro 1 porque "
          //                 "passou muito tempo a espera.\n" RESET,
          //        tipoPessoa);
          free(tipoPessoa);
        } else { // não desiste, vai ser testada
          tipoPessoa = printTipoPessoa(pessoa);
          printf(VERDE "%s foi testada no centro 1.\n" RESET, tipoPessoa);
          free(tipoPessoa);
          pthread_mutex_lock(&mutexVariaveisCentros);
          centroTestes1.numeroPessoasEspera--;
          for (index = 0; index < configuracao.numeroPontosTestagemCentro1;
               index++) {
            if (tempoCooldownPontosTestagemCentro1[index] ==
                -1) // ponto testagem está livre, começa-se o cooldown
            {
              centroTestes1.numeroPostosDisponiveis--;
              tempoCooldownPontosTestagemCentro1[index] =
                  configuracao.tempoCooldownPontosTestagem;
              break;
            }
          }
          pthread_mutex_unlock(&mutexVariaveisCentros);
          pessoa->estado = ISOLAMENTO;
          sem_wait(&mutexVariaveisMonitor);
          pessoasEmIsolamento++;
          casosEmEstudo++;
          numeroPessoasTestadas++;
          somaTemposEspera += tempoEspera;
          sem_post(&mutexVariaveisMonitor);
        }
      }
    } else {
      pthread_mutex_unlock(&mutexVariaveisCentros);
    }
  } else { // CENTRO TESTES 2
    int numeroPessoasEsperaCentro2 =
        centroTestes2.numeroPessoasNormalEspera +
        centroTestes2.numeroPessoasPrioritariasEspera;
    pthread_mutex_unlock(&mutexVariaveisCentros);
    if (numeroPessoasEsperaCentro2 < configuracao.tamanhoFilaCentro2) {
      tipoPessoa = printTipoPessoa(pessoa);
      // printf("%s chegou a fila do centro 2.\n", tipoPessoa);
      free(tipoPessoa);
      if (pessoa->numeroPessoasAFrenteParaDesistir <
          numeroPessoasEsperaCentro2) {
        tipoPessoa = printTipoPessoa(pessoa);
        // printf(VERMELHO "%s desistiu da fila do centro 2 "
        //                 "porque tinha muita gente a frente.\n" RESET,
        //        tipoPessoa);
        free(tipoPessoa);
        pessoa->desistiu = TRUE;
      } else {
        pessoa->tempoChegadaFilaEspera = timestamp;
        // if (numeroPessoasEsperaCentro2 > configuracao.tamanhoFilaCentro2 - 5)
        // {
        //   pessoa->tipoTeste = TESTE_RAPIDO;
        // }
        int aux;
        if (pessoa->idoso) {
          pthread_mutex_lock(&mutexVariaveisCentros);
          centroTestes2.numeroPessoasPrioritariasEspera++;
          if (!(centroTestes2.numeroPessoasPrioritariasEspera == 0 ||
                idososTestadosConsecutivamente >= 5)) {
            sem_getvalue(&centroTestes2.filaEsperaNormal, &valorSemaforo);
            for (aux = 0; aux < valorSemaforo; aux++) {
              sem_wait(&centroTestes2.filaEsperaNormal);
            }
          }
          pthread_mutex_unlock(&mutexVariaveisCentros);
          sem_wait(&centroTestes2.filaEsperaPrioritaria);
        } else {
          pthread_mutex_lock(&mutexVariaveisCentros);
          centroTestes2.numeroPessoasNormalEspera++;
          pthread_mutex_unlock(&mutexVariaveisCentros);
          sem_wait(&centroTestes2.filaEsperaNormal);
        }
        pthread_mutex_lock(&mutexVariaveisSimulacao);
        tempoEspera = minutosDecorridos - pessoa->tempoChegadaFilaEspera;
        pthread_mutex_unlock(&mutexVariaveisSimulacao);
        if (tempoEspera > pessoa->tempoMaximoEspera) { // passou muito tempo à
          // espera, a pessoa desiste
          pessoa->desistiu = TRUE;
          pthread_mutex_lock(&mutexVariaveisCentros);
          if (pessoa->idoso) {
            centroTestes2.numeroPessoasPrioritariasEspera--;
          } else {
            centroTestes2.numeroPessoasNormalEspera--;
          }
          int assinalarSemaforoNormal =
              ((centroTestes2.numeroPessoasPrioritariasEspera == 0 &&
                centroTestes2.numeroPessoasNormalEspera == 0) ||
               (centroTestes2.numeroPessoasNormalEspera > 0 &&
                (idososTestadosConsecutivamente >= 5 ||
                 centroTestes2.numeroPessoasPrioritariasEspera == 0)));
          int assinalarSemaforoPrioritario =
              ((centroTestes2.numeroPessoasPrioritariasEspera == 0 &&
                centroTestes2.numeroPessoasNormalEspera == 0) ||
               (centroTestes2.numeroPessoasPrioritariasEspera > 0 &&
                (idososTestadosConsecutivamente < 5 ||
                 centroTestes2.numeroPessoasNormalEspera == 0)));
          if (assinalarSemaforoNormal) {
            sem_getvalue(&centroTestes2.filaEsperaNormal, &valorSemaforo);
            if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
              sem_post(&centroTestes2.filaEsperaNormal);
            }
          }
          if (assinalarSemaforoPrioritario) {
            sem_getvalue(&centroTestes2.filaEsperaPrioritaria, &valorSemaforo);
            if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
              sem_post(&centroTestes2.filaEsperaPrioritaria);
            }
          }
          pthread_mutex_unlock(&mutexVariaveisCentros);
          tipoPessoa = printTipoPessoa(pessoa);
          // printf(VERMELHO "%s desistiu no centro 2 porque "
          //                 "passou muito tempo a espera.\n" RESET,
          //        tipoPessoa);
          free(tipoPessoa);
        } else { // não desiste, vai ser testada
          tipoPessoa = printTipoPessoa(pessoa);
          printf(VERDE "%s foi testada no centro 2.\n" RESET, tipoPessoa);
          free(tipoPessoa);
          pthread_mutex_lock(&mutexVariaveisCentros);
          if (pessoa->idoso) {
            idososTestadosConsecutivamente++;
            sem_getvalue(&centroTestes2.filaEsperaNormal, &valorSemaforo);
            if (valorSemaforo > 0) {
              sem_wait(&centroTestes2.filaEsperaNormal);
            }
          } else {
            idososTestadosConsecutivamente = 0;
            sem_getvalue(&centroTestes2.filaEsperaPrioritaria, &valorSemaforo);
            if (valorSemaforo > 0) {
              sem_wait(&centroTestes2.filaEsperaPrioritaria);
            }
          }
          if (pessoa->idoso) {
            centroTestes2.numeroPessoasPrioritariasEspera--;
          } else {
            centroTestes2.numeroPessoasNormalEspera--;
          }
          for (index = 0; index < configuracao.numeroPontosTestagemCentro2;
               index++) {
            if (tempoCooldownPontosTestagemCentro2[index] ==
                -1) // ponto testagem está livre, começa-se o cooldown
            {
              centroTestes2.numeroPostosDisponiveis--;
              tempoCooldownPontosTestagemCentro2[index] =
                  configuracao.tempoCooldownPontosTestagem;
              break;
            }
          }
          pthread_mutex_unlock(&mutexVariaveisCentros);
          pessoa->estado = ISOLAMENTO;
          sem_wait(&mutexVariaveisMonitor);
          pessoasEmIsolamento++;
          casosEmEstudo++;
          numeroPessoasTestadas++;
          somaTemposEspera += tempoEspera;
          sem_post(&mutexVariaveisMonitor);
        }
      }
    } else {
      pthread_mutex_unlock(&mutexVariaveisCentros);
    }
  }
}

void Pessoa(void *ptr) {
  struct pessoa pessoa = criaPessoa();
  PessoasCriadas[pessoa.id] = &pessoa;
  char mensagem[TAMANHO_LINHA];
  char *tipoPessoa;
  while (TRUE) {
    FilaEspera(&pessoa);
    if (!pessoa.desistiu) {
      int tempoEsperaTeste = 0;
      if (pessoa.tipoTeste == TESTE_RAPIDO) {
        tempoEsperaTeste = configuracao.tempoTesteRapido * HORA * 1000;
      } else {
        tempoEsperaTeste = configuracao.tempoTesteNormal * HORA * 1000;
      }
      usleep(tempoEsperaTeste);
      fazerTeste(&pessoa);
      if (pessoa.estadoTeste == POSITIVO) {
        tipoPessoa = printTipoPessoa(&pessoa);
        printf(AMARELO "%s testou positivo \n" RESET, tipoPessoa);
        free(tipoPessoa);
        sem_wait(&mutexVariaveisMonitor);
        casosEmEstudo--;
        casosPositivosAtivos++;
        casosPositivosTotal++;
        sem_post(&mutexVariaveisMonitor);
        sem_init(&pessoa.semaforoPessoa, 0, 0);
        pessoa.numeroDiasDesdePositivo = 0;
        break;
      } else if (pessoa.estadoTeste == NEGATIVO) {
        tipoPessoa = printTipoPessoa(&pessoa);
        printf(AMARELO "%s testou negativo \n" RESET, tipoPessoa);
        free(tipoPessoa);
        sem_wait(&mutexVariaveisMonitor);
        casosEmEstudo--;
        pessoasEmIsolamento--;
        sem_post(&mutexVariaveisMonitor);
        pessoa.estado = SOBREVIVEU;
        break;
      } else {
        tipoPessoa = printTipoPessoa(&pessoa);
        printf(AMARELO "%s estou inconclusivo \n" RESET, tipoPessoa);
        free(tipoPessoa);
        sem_wait(&mutexVariaveisMonitor);
        casosEmEstudo--;
        pessoasEmIsolamento--;
        sem_post(&mutexVariaveisMonitor);
        pessoa.estado = ESPERA;
      }
    } else {
      break;
    }
  }
  if (pessoa.estadoTeste == POSITIVO) {
    pessoa.numeroDiasDesdePositivo = 0;
    if ((pessoa.idoso &&
         probabilidade(configuracao.probabilidadeIdosoPrecisaHospital)) ||
        (!pessoa.idoso &&
         probabilidade(
             configuracao
                 .probabilidadeNaoIdosoPrecisaHospital))) { // Vai para o
                                                            // Hospital
      pessoa.estado = HOSPITAL;
      tipoPessoa = printTipoPessoa(&pessoa);
      printf(CIANO "%s foi transportada para o hospital.\n" RESET, tipoPessoa);
      free(tipoPessoa);
      pthread_mutex_lock(&mutexVariaveisHospital);
      if (numeroPacientesNoHospital < configuracao.tamanhoHospital) {
        int index;
        for (index = 0; index < configuracao.tamanhoHospital; index++) {
          if (IDsDoentesNoHospital[index] == -1) {
            IDsDoentesNoHospital[index] = pessoa.id;
            break;
          }
        }
        sem_post(&semaforoMedicos);
        pthread_mutex_unlock(&mutexVariaveisHospital);
        sem_wait(&mutexVariaveisMonitor);
        pessoasEmIsolamento--;
        numeroPacientesNoHospital++;
        sem_post(&mutexVariaveisMonitor);
        sem_wait(&semaforoDoentes);
        pthread_mutex_lock(&mutexVariaveisHospital);
      }
      pthread_mutex_unlock(&mutexVariaveisHospital);
    }
    // printf("PRESEMAFORO\n");
    sem_wait(&pessoa.semaforoPessoa);
    // printf("POSSEMAFORO\n");
  }
}

void Medico(void *ptr) {
  struct pessoa medico = criaMedico();
  PessoasCriadas[medico.id] = &medico;
  char mensagem[TAMANHO_LINHA];
  int tempoEsperaTeste;
  char *tipoPessoa;
  int index;
  sem_init(&medico.semaforoPessoa, 0, 0);
  while (TRUE) {
    sem_wait(&semaforoMedicos);
    tipoPessoa = printTipoPessoa(&medico);
    printf(CIANO "%s vai tratar do doente\n" RESET, tipoPessoa);
    free(tipoPessoa);
    // sprintf(mensagem, "%d-%d-%d", "Z", 5, "Z");
    // enviarMensagem(mensagem);
    sem_wait(&mutexVariaveisMonitor);
    medicosDisponiveis--;
    sem_post(&mutexVariaveisMonitor);
    pthread_mutex_lock(&mutexVariaveisHospital);
    for (index = 0; index < configuracao.tamanhoHospital; index++) {
      if (IDsMedicosASerUsados[index] == -1) {
        IDsMedicosASerUsados[index] = medico.id;
        break;
      }
    }
    pthread_mutex_unlock(&mutexVariaveisHospital);
    sem_post(&semaforoDoentes);
    sem_wait(
        &medico.semaforoPessoa); // Medico acabou de tratar de doente ou
                                 // este morreu -> sai da espera no semaforo
    tempoEsperaTeste =
        configuracao.tempoTesteRapido * HORA * 1000; // DIA * 1000 +
    if (medico.estado == ISOLAMENTO) {
      while (TRUE) {
        usleep(tempoEsperaTeste);
        fazerTeste(&medico);
        if (medico.estadoTeste == POSITIVO) {
          tipoPessoa = printTipoPessoa(&medico);
          printf(BRANCO "%s testou positivo \n" RESET, tipoPessoa);
          free(tipoPessoa);
          sem_wait(&mutexVariaveisMonitor);
          casosEmEstudo--;
          casosPositivosAtivos++;
          casosPositivosTotal++;
          sem_post(&mutexVariaveisMonitor);
          medico.numeroDiasDesdePositivo = 0;
          break;
        } else if (medico.estadoTeste == NEGATIVO) {
          tipoPessoa = printTipoPessoa(&medico);
          printf(BRANCO "%s testou negativo \n" RESET, tipoPessoa);
          free(tipoPessoa);
          sem_wait(&mutexVariaveisMonitor);
          casosEmEstudo--;
          pessoasEmIsolamento--;
          medicosDisponiveis++;
          sem_post(&mutexVariaveisMonitor);
          medico.estado = SOBREVIVEU;
          break;
        } else {
          tipoPessoa = printTipoPessoa(&medico);
          printf(BRANCO "%s testou inconclusivo \n" RESET, tipoPessoa);
          free(tipoPessoa);
          tempoEsperaTeste = configuracao.tempoTesteRapido * HORA * 1000;
        }
      }
    }
    if (medico.estadoTeste == POSITIVO) {
      medico.numeroDiasDesdePositivo = 0;
      if ((medico.idoso &&
           probabilidade(configuracao.probabilidadeIdosoPrecisaHospital)) ||
          (!medico.idoso &&
           probabilidade(
               configuracao
                   .probabilidadeNaoIdosoPrecisaHospital))) { // Vai para o
                                                              // Hospital
        medico.estado = HOSPITAL;
        tipoPessoa = printTipoPessoa(&medico);
        printf(CIANO "%s foi transportada para o hospital.\n" RESET,
               tipoPessoa);
        free(tipoPessoa);
        pthread_mutex_lock(&mutexVariaveisHospital);
        if (numeroPacientesNoHospital < configuracao.tamanhoHospital) {
          for (index = 0; index < configuracao.tamanhoHospital; index++) {
            if (IDsDoentesNoHospital[index] == -1) {
              IDsDoentesNoHospital[index] = medico.id;
              break;
            }
          }
          sem_post(&semaforoMedicos);
          pthread_mutex_unlock(&mutexVariaveisHospital);
          sem_wait(&mutexVariaveisMonitor);
          pessoasEmIsolamento--;
          numeroPacientesNoHospital++;
          sem_post(&mutexVariaveisMonitor);
          sem_wait(&semaforoDoentes);
          pthread_mutex_lock(&mutexVariaveisHospital);
        }
        pthread_mutex_unlock(&mutexVariaveisHospital);
      }
      sem_wait(&medico.semaforoPessoa);
    }
    medico.estadoTeste = NAO_TESTOU;
  }

  int tempoComPaciente = configuracao.tempoCurar * DIA;
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
  enviarMensagem("Z-0-Z"); // Mensagem que indica o comeco da simulacao
  int index;
  char *tipoPessoa;
  char mensagensAEnviar[TAMANHO_LINHA];
  for (index = 0; index < configuracao.tamanhoHospital; index++) {
    IDsDoentesNoHospital[index] = -1;
    IDsMedicosASerUsados[index] = -1;
  }

  for (index = 0; index < configuracao.tamanhoHospital; index++) {
    if (pthread_create(&IDtarefa[idPessoa], NULL, Medico, NULL)) {
      printf("Erro na criação da tarefa\n");
      exit(1);
    }
  }
  sprintf(mensagem, "%d-%d-%d", configuracao.tamanhoHospital, 11, 0);
  strcat(mensagensAEnviar, mensagem);
  enviarMensagem(mensagensAEnviar);
  medicosDisponiveis = configuracao.tamanhoHospital;
  int libertarMedico = FALSE;
  int valorSemaforo = -1;
  int assinalarSemaforoNormal, assinalarSemaforoPrioritario;
  int numeroPessoasRecuperaramHospital = 0,
      numeroPessoasRecuperaramIsolamento = 0, numeroPessoasMorreram = 0,
      numeroMedicosParaIsolamento = 0, numeroMedicosRecuperaram = 0;
  int tempoMedioChegadaCentros = configuracao.tempoMedioChegada;
  int proximoInstanteChegada =
      numeroAleatorio(tempoMedioChegadaCentros + tempoMedioChegadaCentros / 2,
                      tempoMedioChegadaCentros - tempoMedioChegadaCentros / 2);
  int indexPicos = 0, fimPico = 0;

  if (configuracao.diasPicos[indexPicos] == numeroDia) {
    // for (index=0;index<configuracao.numeroPontosTestagemCentro1+1;index++){
    //   printf("%d\n",tempoCooldownPontosTestagemCentro1[index]);
    // }
    // for (index=0;index<configuracao.numeroPontosTestagemCentro2+1;index++){
    //   printf("%d\n",tempoCooldownPontosTestagemCentro2[index]);
    // }
    fimPico = configuracao.diasPicos[indexPicos] +
              configuracao.duracoesPicos[indexPicos];
    tempoMedioChegadaCentros /= 2;
    configuracao.probabilidadePopulacaoPositivo += 0.1;
    configuracao.probabilidadeMedicoPositivo += 0.1;
    pthread_mutex_lock(&mutexVariaveisCentros);
    centroTestes1.numeroPostosDisponiveis++;
    centroTestes2.numeroPostosDisponiveis++;
    tempoCooldownPontosTestagemCentro1
        [configuracao.numeroPontosTestagemCentro1] = -1;
    tempoCooldownPontosTestagemCentro2
        [configuracao.numeroPontosTestagemCentro2 ] = -1;
    sem_post(&centroTestes1.filaEspera);
    assinalarSemaforoNormal =
        ((centroTestes2.numeroPessoasPrioritariasEspera == 0 &&
          centroTestes2.numeroPessoasNormalEspera == 0) ||
         (centroTestes2.numeroPessoasNormalEspera > 0 &&
          (idososTestadosConsecutivamente >= 5 ||
           centroTestes2.numeroPessoasPrioritariasEspera == 0)));
    assinalarSemaforoPrioritario =
        ((centroTestes2.numeroPessoasPrioritariasEspera == 0 &&
          centroTestes2.numeroPessoasNormalEspera == 0) ||
         (centroTestes2.numeroPessoasPrioritariasEspera > 0 &&
          (idososTestadosConsecutivamente < 5 ||
           centroTestes2.numeroPessoasNormalEspera == 0)));
    if (assinalarSemaforoNormal) {
      sem_getvalue(&centroTestes2.filaEsperaNormal, &valorSemaforo);
      if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
        sem_post(&centroTestes2.filaEsperaNormal);
      }
    }
    if (assinalarSemaforoPrioritario) {
      sem_getvalue(&centroTestes2.filaEsperaPrioritaria, &valorSemaforo);
      if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
        sem_post(&centroTestes2.filaEsperaPrioritaria);
      }
    }
    // printf("\n\n");
    // for (index=0;index<configuracao.numeroPontosTestagemCentro1+1;index++){
    //   printf("%d\n",tempoCooldownPontosTestagemCentro1[index]);
    // }
    // for (index=0;index<configuracao.numeroPontosTestagemCentro2+1;index++){
    //   printf("%d\n",tempoCooldownPontosTestagemCentro2[index]);
    // }
    pthread_mutex_unlock(&mutexVariaveisCentros);
    printf("---------------------------------------------INICIO "
           "PICO--------------------------------------------\n");
  }
  while (tempoDecorrido != tempoLimite) {
    auxTimeStamp = current_timestamp();
    if (auxTimeStamp != timeStampAnterior) {
      tempoDecorrido++;
      timeStampAnterior = auxTimeStamp;
      if (tempoDecorrido % MINUTO == 0) { // passou um minuto na simulação
        pthread_mutex_lock(&mutexVariaveisSimulacao);
        minutosDecorridos++;
        pthread_mutex_unlock(&mutexVariaveisSimulacao);
        mensagensAEnviar[0] = '\0';
        pthread_mutex_lock(&mutexVariaveisCentros);
        sprintf(mensagem, "%d-%d-%d", centroTestes1.numeroPessoasEspera, 0, 1);
        strcat(mensagensAEnviar, mensagem);
        strcat(mensagensAEnviar, "/");
        sprintf(mensagem, "%d-%d-%d",
                centroTestes2.numeroPessoasNormalEspera +
                    centroTestes2.numeroPessoasPrioritariasEspera,
                0, 2);
        pthread_mutex_unlock(&mutexVariaveisCentros);
        strcat(mensagensAEnviar, mensagem);
        strcat(mensagensAEnviar, "/");
        sprintf(mensagem, "%d-%d-%d", idPessoa - configuracao.tamanhoHospital,
                5, 0);
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
        if (numeroPessoasTestadas > 0) {
          tempoMedioEspera = somaTemposEspera / numeroPessoasTestadas;
          strcat(mensagensAEnviar, "/");
          sprintf(mensagem, "%d-%d-%d", tempoMedioEspera, 7, 0);
          strcat(mensagensAEnviar, mensagem);
        }
        sem_post(&mutexVariaveisMonitor);
        pthread_mutex_lock(&mutexVariaveisCentros);
        for (index = 0; index < configuracao.numeroPontosTestagemCentro1+1;
             index++) {
          if (tempoCooldownPontosTestagemCentro1[index] == 0) {
            printf(
                MAGENTA
                "Posto de testagem disponivel no centro de testes 1\n" RESET);
            centroTestes1.numeroPostosDisponiveis++;
            sem_post(&centroTestes1.filaEspera);
            tempoCooldownPontosTestagemCentro1[index]--;
          } else if (tempoCooldownPontosTestagemCentro1[index] > 0) {
            tempoCooldownPontosTestagemCentro1[index]--;
          }
        }
        for (index = 0; index < configuracao.numeroPontosTestagemCentro2+1;
             index++) {
          if (tempoCooldownPontosTestagemCentro2[index] == 0) {
            printf(
                MAGENTA
                "Posto de testagem disponivel no centro de testes 2\n" RESET);
            centroTestes2.numeroPostosDisponiveis++;
            assinalarSemaforoNormal =
                ((centroTestes2.numeroPessoasPrioritariasEspera == 0 &&
                  centroTestes2.numeroPessoasNormalEspera == 0) ||
                 (centroTestes2.numeroPessoasNormalEspera > 0 &&
                  (idososTestadosConsecutivamente >= 5 ||
                   centroTestes2.numeroPessoasPrioritariasEspera == 0)));
            assinalarSemaforoPrioritario =
                ((centroTestes2.numeroPessoasPrioritariasEspera == 0 &&
                  centroTestes2.numeroPessoasNormalEspera == 0) ||
                 (centroTestes2.numeroPessoasPrioritariasEspera > 0 &&
                  (idososTestadosConsecutivamente < 5 ||
                   centroTestes2.numeroPessoasNormalEspera == 0)));
            if (assinalarSemaforoNormal) {
              sem_getvalue(&centroTestes2.filaEsperaNormal, &valorSemaforo);
              if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
                sem_post(&centroTestes2.filaEsperaNormal);
              }
            }
            if (assinalarSemaforoPrioritario) {
              sem_getvalue(&centroTestes2.filaEsperaPrioritaria,
                           &valorSemaforo);
              if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
                sem_post(&centroTestes2.filaEsperaPrioritaria);
              }
            }
            tempoCooldownPontosTestagemCentro2[index]--;
          } else if (tempoCooldownPontosTestagemCentro2[index] > 0) {
            tempoCooldownPontosTestagemCentro2[index]--;
          }
        }
        pthread_mutex_unlock(&mutexVariaveisCentros);
        if (minutosDecorridos % proximoInstanteChegada == 0) {
          proximoInstanteChegada = numeroAleatorio(
              tempoMedioChegadaCentros + tempoMedioChegadaCentros / 2,
              tempoMedioChegadaCentros - tempoMedioChegadaCentros / 2);
          // cria tarefas pessoas
          if (pthread_create(&IDtarefa[idPessoa], NULL, Pessoa, NULL)) {
            printf("Erro na criação da tarefa\n");
            exit(1);
          }
        }
        if (minutosDecorridos % (60 * 24) == 0) {
          printf("---------------------------------------------DIA "
                 "%d---------------------------------------------\n",
                 ++numeroDia);
          sprintf(mensagem, "%d-%d-%d", 0, 10, 0);
          strcat(mensagensAEnviar, "/");
          strcat(mensagensAEnviar, mensagem);
          pthread_mutex_lock(&mutexVariaveisHospital);
          numeroPessoasRecuperaramHospital = 0,
          numeroPessoasRecuperaramIsolamento = 0, numeroPessoasMorreram = 0,
          numeroMedicosParaIsolamento = 0;
          for (index = 0; index < configuracao.tamanhoHospital; index++) {
            libertarMedico = FALSE;
            if (IDsDoentesNoHospital[index] != -1) { // se há doente no hospital
              if (PessoasCriadas[IDsDoentesNoHospital[index]]
                      ->numeroDiasDesdePositivo ==
                  configuracao.tempoCurar) { // se já passou o tempo para curar
                tipoPessoa = printTipoPessoa(
                    PessoasCriadas[IDsDoentesNoHospital[index]]);
                printf(CIANO "%s recuperou\n" RESET, tipoPessoa);
                free(tipoPessoa);
                sem_post(&PessoasCriadas[IDsDoentesNoHospital[index]]
                              ->semaforoPessoa);
                numeroPessoasRecuperaramHospital++;
                numeroMedicosRecuperaram +=
                    PessoasCriadas[IDsDoentesNoHospital[index]]->medico;
                PessoasCriadas[IDsDoentesNoHospital[index]]->estado =
                    SOBREVIVEU;
                // casosPositivosAtivos--;
                // numeroPacientesNoHospital--;
                IDsDoentesNoHospital[index] = -1;
                libertarMedico = TRUE;
              } else {
                if (PessoasCriadas[IDsDoentesNoHospital[index]]->idoso) {
                  int pessoaMorreu =
                      IDsMedicosASerUsados[index] != 0
                          ? probabilidade(
                                configuracao.probabilidadeIdosoMorrer / 4)
                          : probabilidade(
                                configuracao.probabilidadeIdosoMorrer);
                  if (pessoaMorreu) {
                    sem_post(&PessoasCriadas[IDsDoentesNoHospital[index]]
                                  ->semaforoPessoa);
                    tipoPessoa = printTipoPessoa(
                        PessoasCriadas[IDsDoentesNoHospital[index]]);
                    printf(CIANO "%s morreu \n" RESET, tipoPessoa);
                    free(tipoPessoa);
                    numeroPessoasMorreram++;
                    PessoasCriadas[IDsDoentesNoHospital[index]]->estado =
                        MORREU;
                    // casosPositivosAtivos--;
                    // numeroPacientesNoHospital--;
                    IDsDoentesNoHospital[index] = -1;
                    libertarMedico = TRUE;
                  }
                } else {
                  int pessoaMorreu =
                      IDsMedicosASerUsados[index] != 0
                          ? probabilidade(
                                configuracao.probabilidadeNaoIdosoMorrer / 4)
                          : probabilidade(
                                configuracao.probabilidadeNaoIdosoMorrer);
                  if (pessoaMorreu) {
                    sem_post(&PessoasCriadas[IDsDoentesNoHospital[index]]
                                  ->semaforoPessoa);
                    tipoPessoa = printTipoPessoa(
                        PessoasCriadas[IDsDoentesNoHospital[index]]);
                    printf(CIANO "%s morreu \n" RESET, tipoPessoa);
                    free(tipoPessoa);
                    numeroPessoasMorreram++;
                    PessoasCriadas[IDsDoentesNoHospital[index]]->estado =
                        MORREU;
                    // casosPositivosAtivos--;
                    // numeroPacientesNoHospital--;
                    IDsDoentesNoHospital[index] = -1;
                    libertarMedico = TRUE;
                  }
                }
              }
              if (IDsMedicosASerUsados[index] != -1 && libertarMedico) {
                numeroMedicosParaIsolamento++;
                PessoasCriadas[IDsMedicosASerUsados[index]]->estado =
                    ISOLAMENTO;
                sem_post(&PessoasCriadas[IDsMedicosASerUsados[index]]
                              ->semaforoPessoa);
                IDsMedicosASerUsados[index] = -1;
              }
            }
            if (IDsDoentesNoHospital[index] !=
                -1) { // se a pessoa não recuperou e não morreu, aumentar o
              // contador
              PessoasCriadas[IDsDoentesNoHospital[index]]
                  ->numeroDiasDesdePositivo++;
            }
          }
          pthread_mutex_unlock(&mutexVariaveisHospital);
          int idPessoaAtual = idPessoa;
          for (index = 0; index < idPessoaAtual; index++) {
            if (PessoasCriadas[index] != NULL &&
                PessoasCriadas[index]->estado == ISOLAMENTO &&
                PessoasCriadas[index]->estadoTeste == POSITIVO) {
              if (PessoasCriadas[index]->numeroDiasDesdePositivo ==
                  configuracao.tempoCurar) { // se já passou o tempo para curar
                tipoPessoa = printTipoPessoa(PessoasCriadas[index]);
                printf(AZUL "%s recuperou em isolamento.\n" RESET, tipoPessoa);
                free(tipoPessoa);
                PessoasCriadas[index]->estado = SOBREVIVEU;
                PessoasCriadas[index]->estadoTeste = NAO_TESTOU;
                numeroPessoasRecuperaramIsolamento++;
                // casosPositivosAtivos--;
                // pessoasEmIsolamento--;
                numeroMedicosRecuperaram += PessoasCriadas[index]->medico;
                sem_post(&PessoasCriadas[index]->semaforoPessoa);
              } else {
                PessoasCriadas[index]->numeroDiasDesdePositivo++;
              }
            }
          }
          sem_wait(&mutexVariaveisMonitor);
          casosRecuperados += numeroPessoasRecuperaramIsolamento +
                              numeroPessoasRecuperaramHospital;
          numeroPacientesNoHospital -=
              (numeroPessoasRecuperaramHospital + numeroPessoasMorreram);
          pessoasEmIsolamento -= numeroPessoasRecuperaramIsolamento;
          pessoasEmIsolamento += numeroMedicosParaIsolamento;
          medicosDisponiveis += numeroMedicosRecuperaram;
          casosEmEstudo += numeroMedicosParaIsolamento;
          casosPositivosAtivos -=
              (numeroPessoasRecuperaramHospital + numeroPessoasMorreram +
               numeroPessoasRecuperaramIsolamento);
          numeroMortos += numeroPessoasMorreram;
          sem_post(&mutexVariaveisMonitor);
          strcat(mensagensAEnviar, "/");
          sprintf(mensagem, "%d-%d-%d", casosRecuperados, 8, 0);
          strcat(mensagensAEnviar, mensagem);
          strcat(mensagensAEnviar, "/");
          sprintf(mensagem, "%d-%d-%d", numeroMortos, 9, 0);
          strcat(mensagensAEnviar, mensagem);
          // scanf(" ");
          if (configuracao.diasPicos[indexPicos] == numeroDia) {
            fimPico = configuracao.diasPicos[indexPicos] +
                      configuracao.duracoesPicos[indexPicos];
            tempoMedioChegadaCentros /= 2;
            configuracao.probabilidadePopulacaoPositivo += 0.1;
            configuracao.probabilidadeMedicoPositivo += 0.1;
            pthread_mutex_lock(&mutexVariaveisCentros);
            centroTestes1.numeroPostosDisponiveis++;
            centroTestes2.numeroPostosDisponiveis++;
    tempoCooldownPontosTestagemCentro1
        [configuracao.numeroPontosTestagemCentro1] = -1;
    tempoCooldownPontosTestagemCentro2
        [configuracao.numeroPontosTestagemCentro2 ] = -1;
            sem_post(&centroTestes1.filaEspera);
            assinalarSemaforoNormal =
                ((centroTestes2.numeroPessoasPrioritariasEspera == 0 &&
                  centroTestes2.numeroPessoasNormalEspera == 0) ||
                 (centroTestes2.numeroPessoasNormalEspera > 0 &&
                  (idososTestadosConsecutivamente >= 5 ||
                   centroTestes2.numeroPessoasPrioritariasEspera == 0)));
            assinalarSemaforoPrioritario =
                ((centroTestes2.numeroPessoasPrioritariasEspera == 0 &&
                  centroTestes2.numeroPessoasNormalEspera == 0) ||
                 (centroTestes2.numeroPessoasPrioritariasEspera > 0 &&
                  (idososTestadosConsecutivamente < 5 ||
                   centroTestes2.numeroPessoasNormalEspera == 0)));
            if (assinalarSemaforoNormal) {
              sem_getvalue(&centroTestes2.filaEsperaNormal, &valorSemaforo);
              if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
                sem_post(&centroTestes2.filaEsperaNormal);
              }
            }
            if (assinalarSemaforoPrioritario) {
              sem_getvalue(&centroTestes2.filaEsperaPrioritaria,
                           &valorSemaforo);
              if (valorSemaforo < centroTestes2.numeroPostosDisponiveis) {
                sem_post(&centroTestes2.filaEsperaPrioritaria);
              }
            }
            pthread_mutex_unlock(&mutexVariaveisCentros);
            printf("---------------------------------------------INICIO "
                   "PICO--------------------------------------------\n");
          } else if (fimPico == numeroDia) {
            tempoMedioChegadaCentros *= 2;
            configuracao.probabilidadePopulacaoPositivo -= 0.1;
            configuracao.probabilidadeMedicoPositivo -= 0.1;
            indexPicos++;
            fimPico = 0;
            pthread_mutex_lock(&mutexVariaveisCentros);
            tempoCooldownPontosTestagemCentro1
                [configuracao.numeroPontosTestagemCentro1] = -2;
            tempoCooldownPontosTestagemCentro2
                [configuracao.numeroPontosTestagemCentro2] = -2;
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
    for (index=0;index<configuracao.numeroPontosTestagemCentro1+1;index++){
      printf("%d\n",tempoCooldownPontosTestagemCentro1[index]);
    }
    for (index=0;index<configuracao.numeroPontosTestagemCentro2+1;index++){
      printf("%d\n",tempoCooldownPontosTestagemCentro2[index]);
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
  if (pthread_mutex_init(&mutexCriarPessoa, NULL) != 0) {
    printf("Inicializacao do trinco falhou.\n");
  }
  if (pthread_mutex_init(&mutexVariaveisCentros, NULL) != 0) {
    printf("Inicializacao do trinco falhou.\n");
  }
  //   if (pthread_mutex_init(&mutexEnviarMensagem, NULL) != 0) {
  //     printf("Inicializacao do trinco falhou.\n");
  //   }
  if (pthread_mutex_init(&mutexVariaveisSimulacao, NULL) != 0) {
    printf("Inicializacao do trinco falhou.\n");
  }
  if (pthread_mutex_init(&mutexVariaveisHospital, NULL) != 0) {
    printf("Inicializacao do trinco falhou.\n");
  }
  sem_init(&centroTestes1.filaEspera, 0,
           configuracao.numeroPontosTestagemCentro1);
  sem_init(&mutexVariaveisMonitor, 0, 1);
  sem_init(&semaforoEnviarMensagem, 0, 1);
  centroTestes1.numeroPostosDisponiveis =
      configuracao.numeroPontosTestagemCentro1;
  tempoCooldownPontosTestagemCentro1 =
      (int *)calloc(configuracao.numeroPontosTestagemCentro1 + 1, sizeof(int));
  int index;
  for (index = 0; index < configuracao.numeroPontosTestagemCentro1; index++) {
    tempoCooldownPontosTestagemCentro1[index] = -1;
  }
  tempoCooldownPontosTestagemCentro1[index] = -2;
  sem_init(&centroTestes2.filaEsperaPrioritaria, 0,
           configuracao.numeroPontosTestagemCentro2);
  sem_init(&centroTestes2.filaEsperaNormal, 0,
           configuracao.numeroPontosTestagemCentro2);
  centroTestes2.numeroPostosDisponiveis =
      configuracao.numeroPontosTestagemCentro2;
  tempoCooldownPontosTestagemCentro2 =
      (int *)calloc(configuracao.numeroPontosTestagemCentro2 + 1, sizeof(int));
  for (index = 0; index < configuracao.numeroPontosTestagemCentro2; index++) {
    tempoCooldownPontosTestagemCentro2[index] = -1;
  }
  tempoCooldownPontosTestagemCentro2[index] = -2;

  sem_init(&semaforoMedicos, 0, 0);
  sem_init(&semaforoDoentes, 0, 0);
  IDsMedicosASerUsados =
      (int *)calloc(configuracao.tamanhoHospital, sizeof(int));
  IDsDoentesNoHospital =
      (int *)calloc(configuracao.tamanhoHospital, sizeof(int));
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

  // guardar os valores do ficheiro na estrutura de configuracao
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
  // printf("Tempo medio de chegada: %d\n", configuracao.tempoMedioChegada);
  // printf("Tempo ate o resultado do teste normal: %d\n",
  // configuracao.tempoTesteNormal); printf("Tempo ate o resultado do teste
  // rapido: %d\n", configuracao.tempoTesteRapido); printf("Tempo de cooldown
  // dos pontos de testagem: %d\n", configuracao.tempoCooldownPontosTestagem);
  // printf("Tempo maximo de espera medio: %d\n",
  // configuracao.tempoMaximoEsperaMedio); printf("Tamanho maximo da fila no
  // centro de testes 1: %d\n", configuracao.tamanhoFilaCentro1);
  // printf("Tamanho maximo da fila no centro de testes 2: %d\n",
  // configuracao.tamanhoFilaCentro2); printf("Numero de pontos de testagem no
  // centro de testes 1: %d\n", configuracao.numeroPontosTestagemCentro1);
  // printf("Numero de pontos de testagem no centro de testes 2: %d\n",
  // configuracao.numeroPontosTestagemCentro2); printf("Tamanho maximo do
  // hospital: %d\n", configuracao.tamanhoHospital); printf("Probabilidade de
  // ser idoso: %f\n", configuracao.probabilidadeSerIdoso);
  // printf("Probabilidade do teste do medico dar positivo; %f\n",
  // configuracao.probabilidadeMedicoPositivo); printf("Probabilidade do teste
  // a populacao dar positivo: %f\n",
  // configuracao.probabilidadePopulacaoPositivo); printf("Probabilidade do
  // teste normal dar inconclusivo: %f\n",
  // configuracao.probabilidadeTesteNormalInconclusivo); printf("Probabilidade
  // do teste rapido dar inconclusivo: %f\n",
  // configuracao.probabilidadeTesteRapidoInconclusivo); printf("Probabilidade
  // de um nao idoso precisar de hospital: %f\n",
  // configuracao.probabilidadeNaoIdosoPrecisaHospital); printf("Probabilidade
  // de um idoso morrer: %f\n", configuracao.probabilidadeIdosoMorrer);
  // printf("Probabilidade do um nao idoso morrer: %f\n",
  // configuracao.probabilidadeNaoIdosoMorrer); printf("Tempo para curar:
  // %d\n", configuracao.tempoCurar); printf("Duracao da simulacao: %d\n",
  // configuracao.tempoSimulacao);
}

int main(int argc, char *argv[]) {

  if (argc == 2) {
    socketfd = criaSocket();
    simulacao(argv[1]);
    close(socketfd);
    return 0;
  } else {
    perror("ERRO!\n(Só) É preciso passar como argumento o ficheiro de "
           "configuração.");
    exit(EXIT_FAILURE);
  }
}