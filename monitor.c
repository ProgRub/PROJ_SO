// Grupo constituído por:
// 2046018 Rúben José Gouveia Rodrigues
// 2045218 Filipe Orlando Namora Gomes
// 2028616 José Alejandro Ferreira Gouveia

#include "config.h"
char SEPARADOR[] = "++++++++++++++++++++++++++++++++++++++++++++\n";

int fimSimulacao = FALSE;

int numeroPessoas = 0, tamanhoFilaCentro1 = 0, numeroPessoasEmIsolamento = 0, tamanhoFilaCentro2 = 0, casosPositivosTotal = 0, casosPositivosAtivos = 0, casosEmEstudo = 0, numeroMortos = 0, casosRecuperados = 0, doentesNoHospital = 0, medicosDisponiveis = 0;

/*---------------------------------------
            SOCKETS
-----------------------------------------*/
void criaServidor()
{
    //sockfd -> criacao para a primeira comunicacao
    //novoSocket -> criacao para a segunda comunicacao
    int sockfd, novoSocket, tamanhoCliente, tamanhoServidor;
    struct sockaddr_un end_cli, serv_addr;

    //Verifica a criacao do socket
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        printf("Erro ao criar o Socket\n");
    }

    //Incializa os valores do buffer a zero
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, UNIXSTR_PATH);
    tamanhoServidor = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);
    unlink(UNIXSTR_PATH);

    //Liga o socket a um endereco
    if (bind(sockfd, (struct sockaddr *)&serv_addr, tamanhoServidor) < 0)
    {
        printf("Erro a ligar o socket a um endereco\n");
    }

    //Espera a conexao com o simulador
    printf("Esperando pelo simulador...\n");
    listen(sockfd, 1);

    //Criacao de um novo scoket
    tamanhoCliente = sizeof(end_cli);
    novoSocket = accept(sockfd, (struct sockaddr *)&end_cli, &tamanhoCliente);
    if (novoSocket < 0)
    {
        printf("Erro na aceitacao \n");
    } //Verifica o erro na aceitacao da ligacao

    //Criacao de um novo processo
    int pid;
    if ((pid = fork()) < 0)
    {
        printf("Erro ao criar o processo filho \n"); //Erro na criacao do processo filho
    }
    else if (pid == 0)
    { //Processo filho irá tratar das sucessivas leituras e fecha o socket do processo pai
        close(sockfd);
        leituraSocket(novoSocket);
    }
    close(novoSocket);
}

void leituraSocket(int sockfd)
{
    int numero = 0;
    char buffer[TAMANHO_LINHA];
    while (!fimSimulacao)
    {
        numero = read(sockfd, buffer, TAMANHO_LINHA); //Le a mensagem do socket e guarda no buffer
        if (numero == 0)
        { //Quando chega ao fim
            // printf("FIM");
            break;
        }
        else if (numero < 0)
        {
            printf("Erro: Nao leu do socket \n");
        }
        else
        {
            printf("Mensagem Recebida");
            trataMensagem(buffer);
        }
    }
}

void trataMensagem(char mensagem[])
{
    //Auxiliario
    char bufferAuxiliario[30];
    strcpy(bufferAuxiliario, mensagem);
    char *valoresSeparados[4][30];
    int index = 0;

    //Obtem o head da lista ligada que se obtem separando bufferAuxiliario por "-"
    char *auxiliario = strtok(bufferAuxiliario, "-");

    //Ciclo que percorre e vai separando pelos _ e copiando para valoresSeparados[i]
    while (auxiliario != NULL)
    {
        strcpy(valoresSeparados[index++], auxiliario);
        auxiliario = strtok(NULL, "-");
    }

    //Onde vai guardar os valores depois da divisao
    int acontecimento = strtol(valoresSeparados[2], NULL, 10);
    if (!strcmp(valoresSeparados[0], "Z") && !strcmp(valoresSeparados[1], "Z") && !strcmp(valoresSeparados[3], "Z"))
    {
        if (acontecimento == 0)
        {
            // printf("Bem vindo! A simulação comecou!\n");
        }
        else
        {
            // printf("O tempo limite da simulacao foi atingido.\n");
            fimSimulacao = TRUE;
        }
    }
    else
    {
        int especificacaoAcontecimento = strtol(valoresSeparados[3], NULL, 10);
        switch (acontecimento)
        {
        case 0: //Utilizador chegou à fila de um centro.
            numeroPessoas++;
            if (especificacaoAcontecimento == 1)
            {
                tamanhoFilaCentro1++;
            }
            else
            {
                tamanhoFilaCentro2++;
            }
            break;
        case 1: //Utilizador saiu da fila de um centro, porque foi testado
            casosEmEstudo++;
            if (especificacaoAcontecimento == 1)
            {
                tamanhoFilaCentro1--;
            }
            else
            {
                tamanhoFilaCentro2--;
            }
            break;
        case 2: //Utilizador saiu da fila de um centro, porque desistiu
            if (especificacaoAcontecimento == 1)
            {
                tamanhoFilaCentro1--;
            }
            else
            {
                tamanhoFilaCentro2--;
            }
            break;
        case 3: //Utilizador vai para isolamento
            numeroPessoasEmIsolamento++;
            break;
        case 4: //Utilizador vai para o hospital
            doentesNoHospital++;
            break;
        case 5: //Medico vai tratar de doente
            medicosDisponiveis--;
            break;
        case 6: //Utilizador morre ou no hospital ou em isolamento
            numeroMortos++;
            casosPositivosAtivos--;
            if (especificacaoAcontecimento == 1)
            {
                doentesNoHospital--;
            }
            else
            {
                numeroPessoasEmIsolamento--;
            }
            break;
        case 7://Medico acaba de tratar de doente (ou este morre) e vai para isolamento
            numeroPessoasEmIsolamento++;
            doentesNoHospital--;
            break;
        case 8: //Medico é testado em isolamento
            casosEmEstudo++;
            break;
        case 9: //Utilizador morre em isolamento
            numeroMortos++;
            casosPositivosAtivos--;
            break;
        case 10: //Utilizador testa positivo
            casosPositivosAtivos++;
            casosPositivosTotal++;
            casosEmEstudo--;
            break;
        case 11://Utilizador testa negativo
            casosEmEstudo--;
            break;
        case 12: //Utilizador recupera
            casosPositivosAtivos--;
            break;
        case 13: //Medico criado
            medicosDisponiveis++;
            break;
        case 14: //O teste ao utilizador é inconclusivo
            casosEmEstudo--;
            break;
        }
    }
    imprimirInformacao();
}

/*---------------------------------------
            IMPRIMIR INFORMACAO
-----------------------------------------*/
void limparFicheiro()
{
    fclose(fopen("Relatorio.txt", "w"));
}

void escreveEmFicheiroEMonitor(char *mensagem)
{
    FILE *ficheiroRegisto;
    ficheiroRegisto = fopen("Relatorio.txt", "a");
    if (ficheiroRegisto == NULL)
    {
        printf("Nao foi possivel abrir o ficheiro %s.\n", "Relatorio.txt");
    }
    else
    {
        printf("%s", mensagem);
        fprintf(ficheiroRegisto, "%s", mensagem);
        fclose(ficheiroRegisto);
    }
}

void imprimirInformacao()
{
    char texto[TAMANHO_LINHA];
    system("clear");
    limparFicheiro();
    escreveEmFicheiroEMonitor(SEPARADOR);
    if (!fimSimulacao)
    {
        escreveEmFicheiroEMonitor("Estado atual => Simulacao a decorrer!\n");
    }
    else
    {
        escreveEmFicheiroEMonitor("Estado atual => Simulacao finalizada.\n");
    }
    sprintf(texto, "Pessoas: %d\n", numeroPessoas);
    escreveEmFicheiroEMonitor(texto);
    sprintf(texto, "Pessoas a espera no centro 1: %d\n", tamanhoFilaCentro1);
    escreveEmFicheiroEMonitor(texto);
    sprintf(texto, "Pessoas a espera no centro 2: %d\n", tamanhoFilaCentro2);
    escreveEmFicheiroEMonitor(texto);
    sprintf(texto, "Casos positivos (total): %d\n", casosPositivosTotal);
    escreveEmFicheiroEMonitor(texto);
    sprintf(texto, "Casos positivos (ativos): %d\n", casosPositivosAtivos);
    escreveEmFicheiroEMonitor(texto);
    sprintf(texto, "Casos em estudo: %d\n", casosEmEstudo);
    escreveEmFicheiroEMonitor(texto);
    sprintf(texto, "Pessoas em isolamento: %d\n", numeroPessoasEmIsolamento);
    escreveEmFicheiroEMonitor(texto);
    sprintf(texto, "Numero de mortos: %d\n", numeroMortos);
    escreveEmFicheiroEMonitor(texto);
    sprintf(texto, "Casos recuperados: %d\n", casosRecuperados);
    escreveEmFicheiroEMonitor(texto);
    sprintf(texto, "Doentes no hospital: %d\n", doentesNoHospital);
    escreveEmFicheiroEMonitor(texto);
    sprintf(texto, "Medicos disponiveis: %d\n", medicosDisponiveis);
    escreveEmFicheiroEMonitor(texto);
    escreveEmFicheiroEMonitor(SEPARADOR);
}

int main(int argc, char *argv[])
{
    printf("++++++++++++ Bem vindo ++++++++++++\n");
    printf("1: Comecar simulacao          \n");
    // printf ( "2: Limpar ficheiros da simulacao \n" );
    printf("%s\n", SEPARADOR);
    int opcao = 0;
    while (!fimSimulacao)
    {
        while (opcao != 1)
        {
            printf("Opcao: ");
            scanf("%d", &opcao); //Le valor introduzido
            if (opcao != 1)
            {
                printf("Isso nao e uma simulacao.\n");
                opcao = 0;
            }
        }
        criaServidor();
    }
    return 0;
}