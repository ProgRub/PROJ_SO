// Grupo constituído por:
// 2046018 Rúben José Gouveia Rodrigues
// 2045218 Filipe Orlando Namora Gomes
// 2028616 José Alejandro Ferreira Gouveia

#include "config.h"
char SEPARADOR[] = "++++++++++++++++++++++++++++++++++++++++++++\n";

int fimSimulacao = FALSE;

int numeroPessoas = 0, numeroDiasPassados = 1, tempoMedioEspera = 0,
    tamanhoFilaCentro1 = 0, numeroPessoasEmIsolamento = 0,
    tamanhoFilaCentro2 = 0, casosPositivosTotal = 0, casosPositivosAtivos = 0,
    casosEmEstudo = 0, numeroMortos = 0, casosRecuperados = 0,
    doentesNoHospital = 0, medicosDisponiveis = 0,
    numeroPessoasTestadasNosCentros = 0, somaTemposEspera = 0;

/*---------------------------------------
            SOCKETS
-----------------------------------------*/
void criaServidor() {
  // sockfd -> criacao para a primeira comunicacao
  // novoSocket -> criacao para a segunda comunicacao
  int sockfd, novoSocket, tamanhoCliente, tamanhoServidor;
  struct sockaddr_un end_cli, serv_addr;

  // Verifica a criacao do socket
  if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    printf("Erro ao criar o Socket\n");
  }

  // Incializa os valores do buffer a zero
  bzero((char *)&serv_addr, sizeof(serv_addr));
  serv_addr.sun_family = AF_UNIX;
  strcpy(serv_addr.sun_path, UNIXSTR_PATH);
  tamanhoServidor = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
  unlink(UNIXSTR_PATH);

  // Liga o socket a um endereco
  if (bind(sockfd, (struct sockaddr *)&serv_addr, tamanhoServidor) < 0) {
    printf("Erro a ligar o socket a um endereco\n");
  }

  // Espera a conexao com o simulador
  printf("Esperando pelo simulador...\n");
  listen(sockfd, 1);

  // Criacao de um novo scoket
  tamanhoCliente = sizeof(end_cli);
  novoSocket = accept(sockfd, (struct sockaddr *)&end_cli, &tamanhoCliente);
  if (novoSocket < 0) {
    printf("Erro na aceitacao \n");
  } // Verifica o erro na aceitacao da ligacao

  // Criacao de um novo processo
  int pid;
  if ((pid = fork()) < 0) {
    printf("Erro ao criar o processo filho \n"); // Erro na criacao do processo
                                                 // filho
  } else if (pid == 0) { // Processo filho irá tratar das sucessivas leituras e
                         // fecha o socket do processo pai
    close(sockfd);
    leituraSocket(novoSocket);
  }
  close(novoSocket);
}

void leituraSocket(int sockfd) {
  int numero = 0;
  char buffer[TAMANHO_LINHA];
  while (!fimSimulacao) {
    numero = read(sockfd, buffer,
                  TAMANHO_LINHA); // Le a mensagem do socket e guarda no buffer
    if (numero == 0) {            // Quando chega ao fim
      // printf("FIM");
      break;
    } else if (numero < 0) {
      printf("Erro: Nao leu do socket \n");
    } else {
      // printf("Mensagem Recebida");
      trataMensagem(buffer);
    }
  }
}

void trataMensagem(char mensagem[]) {
  // Auxiliario
  char bufferAuxiliario[TAMANHO_LINHA];
  strcpy(bufferAuxiliario, mensagem);
  char mensagensSeparadas[15][30];
  char valoresSeparados[3][30];
  int index = 0;
  int indexMensagem=0;
  int i = 0;
  int numeroMensagens = 0;
  char *p = strtok(bufferAuxiliario, "/");

  while (p != NULL) {
    strcpy(mensagensSeparadas[numeroMensagens++], p);
    p = strtok(NULL, "/");
  }

  // for (i = 0; i < 15; ++i) {
  //     printf("%s\n", mensagensSeparadas[i]);}
  
  // Obtem o head da lista ligada que se obtem separando bufferAuxiliario por
  // "-"
  while (indexMensagem < numeroMensagens) {
  index = 0;
    char *auxiliario = strtok(mensagensSeparadas[indexMensagem], "-");
      // strcpy(valoresSeparados[index++], auxiliario);
      // printf("%s\n", auxiliario);
    indexMensagem++;

    // Ciclo que percorre e vai separando pelos - e copiando para
    // valoresSeparados[i]
    while (auxiliario != NULL) {
      strcpy(valoresSeparados[index++], auxiliario);
      auxiliario = strtok(NULL, "-");
    }

  // for (i = 0; i < 3; ++i) {
  //     printf("%d %s\n", i,valoresSeparados[i]);}

    // Onde vai guardar os valores depois da divisao
    int acontecimento = strtol(valoresSeparados[1], NULL, 10);
    if (!strcmp(valoresSeparados[0], "Z") &&
        !strcmp(valoresSeparados[2], "Z")) {
      if (acontecimento == 0) {
        // printf("Bem vindo! A simulação comecou!\n");
      } else {
        // printf("O tempo limite da simulacao foi atingido.\n");
        fimSimulacao = TRUE;
      }
    } else {
      int numero;
      if (strcmp(valoresSeparados[0], "Z")) {
        numero = strtol(valoresSeparados[0], NULL, 10);
      } else {
        numero = -1;
      }
      int especificacaoAcontecimento;
      if (strcmp(valoresSeparados[2], "Z")) {
        especificacaoAcontecimento = strtol(valoresSeparados[2], NULL, 10);
      } else {
        especificacaoAcontecimento = -1;
      }
      switch (acontecimento) {
      case 0: // Pessoa chegou à fila de um centro.
        // numeroPessoas++;
        if (especificacaoAcontecimento == 1) {
          tamanhoFilaCentro1=numero;
        } else {
          tamanhoFilaCentro2=numero;
        }
        break;
      case 1:
        casosPositivosAtivos = numero;
        break;
      case 2:
        casosPositivosTotal = numero;
        break;
      case 3:
        casosEmEstudo = numero;
        break;
      case 4:
        numeroPessoasEmIsolamento = numero;
        break;
      case 5:
        numeroPessoas = numero;
        break;
      case 6:
        doentesNoHospital = numero;
        break;
        case 7:
        tempoMedioEspera=numero;
        break;
        case 8:
        casosRecuperados=numero;
        break;
        case 9:
        numeroMortos=numero;
        break;
      case 10: // Passou um dia na simulação
        numeroDiasPassados++;
        break;
      case 11: // Medico criado
        medicosDisponiveis = numero;
        break;
      }
    }
  }
  imprimirInformacao();
}

/*---------------------------------------
            IMPRIMIR INFORMACAO
-----------------------------------------*/
void limparFicheiro() { fclose(fopen("Relatorio.txt", "w")); }

void escreveEmFicheiro() {
  limparFicheiro();
  FILE *ficheiroRegisto;
  ficheiroRegisto = fopen("Relatorio.txt", "a");
  if (ficheiroRegisto == NULL) {
    printf("Nao foi possivel abrir o ficheiro %s.\n", "Relatorio.txt");
  } else {
    fprintf(ficheiroRegisto, "%s", SEPARADOR);
    if (!fimSimulacao) {
      fprintf(ficheiroRegisto, "%s", "Estado atual => Simulacao a decorrer!\n");
    } else {
      fprintf(ficheiroRegisto, "%s", "Estado atual => Simulacao finalizada.\n");
    }
    fprintf(ficheiroRegisto, "\t\t\t DIA %d\n", numeroDiasPassados);
    fprintf(ficheiroRegisto, "Pessoas: %d\n", numeroPessoas);
    fprintf(ficheiroRegisto, "Pessoas a espera no centro 1: %d\n",
            tamanhoFilaCentro1);
    fprintf(ficheiroRegisto, "Pessoas a espera no centro 2: %d\n",
            tamanhoFilaCentro2);
    fprintf(ficheiroRegisto, "Casos positivos (total): %d\n",
            casosPositivosTotal);
    fprintf(ficheiroRegisto, "Casos positivos (ativos): %d\n",
            casosPositivosAtivos);
    fprintf(ficheiroRegisto, "Casos em estudo: %d\n", casosEmEstudo);
    fprintf(ficheiroRegisto, "Pessoas em isolamento: %d\n",
            numeroPessoasEmIsolamento);
    fprintf(ficheiroRegisto, "Numero de mortos: %d\n", numeroMortos);
    fprintf(ficheiroRegisto, "Casos recuperados: %d\n", casosRecuperados);
    fprintf(ficheiroRegisto, "Doentes no hospital: %d\n", doentesNoHospital);
    fprintf(ficheiroRegisto, "Medicos disponiveis: %d\n", medicosDisponiveis);
    fprintf(ficheiroRegisto, "Tempo medio de espera (em minutos): %d\n",
            tempoMedioEspera);
    fprintf(ficheiroRegisto, "%s", SEPARADOR);
    fclose(ficheiroRegisto);
  }
}

void imprimirInformacao() {
  printf("%s", SEPARADOR);
  if (!fimSimulacao) {
    printf("%s", "Estado atual => Simulacao a decorrer!\n");
  } else {
    printf("%s", "Estado atual => Simulacao finalizada.\n");
  }
  printf("\t\t\t DIA %d\n", numeroDiasPassados);
  printf("Pessoas: %d\n", numeroPessoas);
  printf("Pessoas a espera no centro 1: %d\n", tamanhoFilaCentro1);
  printf("Pessoas a espera no centro 2: %d\n", tamanhoFilaCentro2);
  printf("Casos positivos (total): %d\n", casosPositivosTotal);
  printf("Casos positivos (ativos): %d\n", casosPositivosAtivos);
  printf("Casos em estudo: %d\n", casosEmEstudo);
  printf("Pessoas em isolamento: %d\n", numeroPessoasEmIsolamento);
  printf("Numero de mortos: %d\n", numeroMortos);
  printf("Casos recuperados: %d\n", casosRecuperados);
  printf("Doentes no hospital: %d\n", doentesNoHospital);
  printf("Medicos disponiveis: %d\n", medicosDisponiveis);
  printf("Tempo medio de espera (em minutos): %d\n", tempoMedioEspera);
  printf("%s", SEPARADOR);
}

int main(int argc, char *argv[]) {
  printf("++++++++++++ Bem vindo ++++++++++++\n");
  printf("1: Comecar simulacao          \n");
  // printf ( "2: Limpar ficheiros da simulacao \n" );
  printf("%s\n", SEPARADOR);
  int opcao = 0;
  while (!fimSimulacao) {
    while (opcao != 1) {
      printf("Opcao: ");
      scanf("%d", &opcao); // Le valor introduzido
      if (opcao != 1) {
        printf("Isso nao e uma simulacao.\n");
        opcao = 0;
      }
    }
    criaServidor();
  }
  escreveEmFicheiro();
  return 0;
}