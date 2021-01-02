// Grupo constituído por:
// 2046018 Rúben José Gouveia Rodrigues
// 2045218 Filipe Orlando Namora Gomes
// 2028616 José Alejandro Ferreira Gouveia

#include "config.h"
char SEPARADOR[] = "++++++++++++++++++++++++++++++++++++++++++++\n";

int fimSimulacao = FALSE;

int numeroPessoas = 0, numeroDiasPassados = 0, tempoMedioEspera = 0,
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
  char bufferAuxiliario[30];
  strcpy(bufferAuxiliario, mensagem);
  char *valoresSeparados[4][30];
  int index = 0;

  // Obtem o head da lista ligada que se obtem separando bufferAuxiliario por
  // "-"
  char *auxiliario = strtok(bufferAuxiliario, "-");

  // Ciclo que percorre e vai separando pelos - e copiando para
  // valoresSeparados[i]
  while (auxiliario != NULL) {
    strcpy(valoresSeparados[index++], auxiliario);
    auxiliario = strtok(NULL, "-");
  }

  // Onde vai guardar os valores depois da divisao
  int acontecimento = strtol(valoresSeparados[2], NULL, 10);
  if (!strcmp(valoresSeparados[0], "Z") && !strcmp(valoresSeparados[1], "Z") &&
      !strcmp(valoresSeparados[3], "Z")) {
    if (acontecimento == 0) {
      // printf("Bem vindo! A simulação comecou!\n");
    } else {
      // printf("O tempo limite da simulacao foi atingido.\n");
      fimSimulacao = TRUE;
    }
  } else {
    int idPessoa = strtol(valoresSeparados[0], NULL, 10);
    int timestamp = strtol(valoresSeparados[1], NULL, 10);
    int especificacaoAcontecimento = strtol(valoresSeparados[3], NULL, 10);
    switch (acontecimento) {
    case 0: // Pessoa chegou à fila de um centro.
      numeroPessoas++;
      if (especificacaoAcontecimento == 1) {
        tamanhoFilaCentro1++;
      } else {
        tamanhoFilaCentro2++;
      }
      break;

    // A mensagem com este case é enviada depois da mensagem com o case 0
    // Temos que criar uma variavel na estrutura da pessoa que guarda o id do
    // enfermeiro no simulador
    case 1: // Pessoa saiu da fila de um centro, porque vai ser testado
      casosEmEstudo++;
      numeroPessoasEmIsolamento++;
      numeroPessoasTestadasNosCentros++;
      somaTemposEspera += timestamp;
      tempoMedioEspera = somaTemposEspera / numeroPessoasTestadasNosCentros;
      if (especificacaoAcontecimento == 1) {
        tamanhoFilaCentro1--;
      } else {
        tamanhoFilaCentro2--;
      }
      break;

    // A mensagem com este case é enviada depois da mensagem com o case 0
    case 2: // Pessoa saiu da fila de um centro, porque desistiu
      if (especificacaoAcontecimento == 1) {
        tamanhoFilaCentro1--;
      } else {
        tamanhoFilaCentro2--;
      }
      break;

    // A mensagem com este case é enviada depois da mensagem com o case 5
    case 3: // Pessoa vai para isolamento
      break;
    case 4: // Pessoa vai para o hospital e sai do isolamento
      doentesNoHospital++;
      numeroPessoasEmIsolamento--;
      break;
    // A mensagem com este case é enviada depois da mensagem com o case 1
    case 5: // Medico vai tratar doente
      medicosDisponiveis--;
      break;
    case 6: // Medico acaba de tratar de doente (ou este morre) e o médico vai
            // para isolamento
      doentesNoHospital--;
      numeroPessoasEmIsolamento++;
      break;
    case 7: // Medico é testado em isolamento
      casosEmEstudo++;
      numeroPessoasEmIsolamento++;
      medicosDisponiveis--;
      break;
    case 8: // Pessoa testa positivo
      casosPositivosAtivos++;
      casosPositivosTotal++;
      casosEmEstudo--;
      break;
    case 9: // Pessoa testa negativo
      casosEmEstudo--;
      numeroPessoasEmIsolamento--;
      if (especificacaoAcontecimento == 1) {
        // printf("HERE %d\n",especificacaoAcontecimento);
        // printf("HERE %d\n",medicosDisponiveis);
        medicosDisponiveis++;
        // printf("HERE %d\n",medicosDisponiveis);
        // while(TRUE);
      }
      break;
    case 10: // Pessoa ou Medico recupera
      casosPositivosAtivos--;
      casosRecuperados++;
      if (especificacaoAcontecimento == 1) {
        medicosDisponiveis++;
      }
      break;
    case 11: // Medico criado
      medicosDisponiveis++;
      break;
    case 12: // O teste ao pessoa é inconclusivo
      casosEmEstudo--;
      numeroPessoasEmIsolamento--;
      break;
    case 13: // Pessoa ou Medico morre
      numeroMortos++;
      casosPositivosAtivos--;
      if (especificacaoAcontecimento == 1) {
        medicosDisponiveis--;
      }
      break;
    case 14: // Passou um dia na simulação
      numeroDiasPassados++;
      break;
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
    fprintf(ficheiroRegisto,"%s", SEPARADOR);
    if (!fimSimulacao) {
      fprintf(ficheiroRegisto,"%s", "Estado atual => Simulacao a decorrer!\n");
    } else {
      fprintf(ficheiroRegisto,"%s", "Estado atual => Simulacao finalizada.\n");
    }
    fprintf(ficheiroRegisto,"\t\t\t DIA %d\n", numeroDiasPassados);
    fprintf(ficheiroRegisto,"Pessoas: %d\n", numeroPessoas);
    fprintf(ficheiroRegisto,"Pessoas a espera no centro 1: %d\n", tamanhoFilaCentro1);
    fprintf(ficheiroRegisto,"Pessoas a espera no centro 2: %d\n", tamanhoFilaCentro2);
    fprintf(ficheiroRegisto,"Casos positivos (total): %d\n", casosPositivosTotal);
    fprintf(ficheiroRegisto,"Casos positivos (ativos): %d\n", casosPositivosAtivos);
    fprintf(ficheiroRegisto,"Casos em estudo: %d\n", casosEmEstudo);
    fprintf(ficheiroRegisto,"Pessoas em isolamento: %d\n", numeroPessoasEmIsolamento);
    fprintf(ficheiroRegisto,"Numero de mortos: %d\n", numeroMortos);
    fprintf(ficheiroRegisto,"Casos recuperados: %d\n", casosRecuperados);
    fprintf(ficheiroRegisto,"Doentes no hospital: %d\n", doentesNoHospital);
    fprintf(ficheiroRegisto,"Medicos disponiveis: %d\n", medicosDisponiveis);
    fprintf(ficheiroRegisto,"Tempo medio de espera (em minutos): %d\n", tempoMedioEspera);
    fprintf(ficheiroRegisto,"%s", SEPARADOR);
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