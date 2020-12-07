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
        case 0: //Pessoa chegou à fila de um centro.
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
        
        //A mensagem com este case é enviada depois da mensagem com o case 0
        //Temos que criar uma variavel na estrutura da pessoa que guarda o id do enfermeiro no simulador
        case 1: //Pessoa saiu da fila de um centro, porque vai ser testado
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

        //A mensagem com este case é enviada depois da mensagem com o case 0
        case 2: //Pessoa saiu da fila de um centro, porque desistiu
            if (especificacaoAcontecimento == 1)
            {
                tamanhoFilaCentro1--;
            }
            else
            {
                tamanhoFilaCentro2--;
            }
            break;

        //A mensagem com este case é enviada depois da mensagem com o case 5
        case 3: //Pessoa vai para isolamento
            numeroPessoasEmIsolamento++;
            casosEmEstudo++;
            break;

        //case 4: //Utilizador vai para o hospital
        //    doentesNoHospital++;
        //    break;

        //A mensagem com este case é enviada depois da mensagem com o case 3
        case 4: //Pessoa vai para o hospital se o teste for positivo e sai do isolamento
            doentesNoHospital++;
            casosPositivosAtivos++;
            casosPositivosTotal++;
            numeroPessoasEmIsolamento--;
            casosEmEstudo--;
            break;

        //A mensagem com este case é enviada depois da mensagem com o case 1
        case 5: //Enfermeiro é chamado para comecar a testagem de uma pessoa
            medicosDisponiveis--;
            break;

        //O caso 6 não faz sentido, o medico só deverá ser enviado para isolamento em caso que um dos seus anteriores pacientes 
        //tenha dado teste positivos
        //case 6://Medico acaba de tratar de doente (ou este morre) e o médico vai para isolamento
        //    numeroPessoasEmIsolamento++;
        //    doentesNoHospital--;
        //    casosPositivosAtivos--;
        //    if (especificacaoAcontecimento == 1)
        //    {
        //        numeroMortos++;
        //    }
        //    break;

        //A mensagem com este case é enviada depois da mensagem com o case 5
        case 6://Enfermeiro acaba a testagem do paciente e envia-o para isolamento, ficando assim disponviel
            medicosDisponiveis++;
            break;
        

        //O caso 7 não faz sentido, pois o isolamento serve apenas para a pessoa saber o resultado, se o resultado for positivo é enviada
        //para internamento
        //Sendo assim a pessoa só poderá morrer durante o internamento no hospital
        //case 7: //Utilizador morre no isolamento
        //    numeroMortos++;
        //    numeroPessoasEmIsolamento--;
        //    casosPositivosAtivos--;
        //    break;

        //A mensagem com este case é enviada depois da mensagem com o case 4
        case 7: //Paciente pode ou não morrer no hospital 
            casosPositivosAtivos--;
            if (especificacaoAcontecimento == 1)
            {
                numeroMortos++;
            }
            break;

        //case 8: //Medico é testado em isolamento
        //    casosEmEstudo++;
        //    break;

        //A mensagem com este case é enviada depois da mensagem com o case 4
        case 8: //O enfermeiro é enviado para isolamento pois um dos seus pacientes testou positivo
            casosEmEstudo++;
            numeroPessoasEmIsolamento++;
            medicosDisponiveis--;
            break;

        //Este case é o case 4
        //case 9: //Utilizador testa positivo
        //    casosPositivosAtivos++;
        //    casosPositivosTotal++;
        //    casosEmEstudo--;
        //    break;

        case 9: //Enfermeiro pode ou não morrer no hospital, se morrer esse enfermeiro é substituido
            casosPositivosAtivos--;
            medicosDisponiveis++;
            if (especificacaoAcontecimento == 1)
            {
                numeroMortos++;
            }
            break;

        //A mensagem com este case é enviada depois da mensagem com o case 3
        case 10://Paciente testa negativo e sai do isolamento
            casosEmEstudo--;
            numeroPessoasEmIsolamento--;
            break;

        //A mensagem com este case é enviada depois da mensagem com o case 8
        case 11://Enfermeiro testa negativo e sai do isolamento
            casosEmEstudo--;
            numeroPessoasEmIsolamento--;
            medicosDisponiveis++;
            break;

        //Este case é o "else" do case 7
        //case 11: //Utilizador recupera
        //    casosPositivosAtivos--;
        //    break;

        case 12: //Enfermeiro criado
            medicosDisponiveis++;
            break;

        //A mensagem com este case é enviada depois da mensagem com o case 3
        case 13: //O teste do paciente foi inconclusivo e por isso tem que ser enviado para um nova testagem
            casosEmEstudo--;
            numeroPessoasEmIsolamento--;
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