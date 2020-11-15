#include "config.h"


struct atracao_toboga {
    int pessoas_que_entraram;
    int pessoas_que_usufruiram;
    int pessoas_fila_espera;
    int boias_seguidas;
    int desistencias;
    int tempo_medio_espera;
    int i;
    int tempos[9999];
};

struct atracao_piscina {
    int pessoas_que_entraram;
    int pessoas_que_usufruiram;
    int pessoas_fila_espera;
    int pessoas_dentro_da_piscina;
    int desistencias;
    int tempo_medio_espera;
    int i;
    int tempos[9999];
};

struct atracao_montanha_russa {
    int pessoas_que_entraram;
    int pessoas_que_usufruiram;
    int pessoas_fila_espera;
    int viagens_de_carro;
    char estado_porto[60];
    char estado_porto_carga[60];
    char estado_porto_descarga[60];
    int i;
    int tempos[9999];
};

struct atracao_toboga toboga;
struct atracao_piscina piscina;
struct atracao_montanha_russa montanha_russa;
//-----------------------------------------------------struct atracao toboga, montanha_russa, piscina, mini_comboio;
int fim_simulacao = 0;
int n_pessoas_total = 0;
int n_normal , n_vip;
//-------------------------------tempos
int maximo ( int arr[] , int size ) {         // ver o maximo (tempo?)
    int max = 0;
    if ( size > 0 ) {
        int i = 0;
        max = arr[ 0 ];
        for ( i = 0 ; i < size ; ++ i ) {
            if ( arr[ i ] > max ) {
                max = arr[ i ];
            }
        }
    }
    return max;
}
float media ( int arr[] , int size ) {
    float media = 0;
    if ( size > 0 ) {
        int i = 0;
        int soma = 0;
        for ( i = 0 ; i < size ; ++ i ) {
            soma += arr[ i ];
        }
        media = soma / size;
    }
    return media;
}
//---------------------------------------------------------------TEMPOS---------

// GUARDA VALORES DA SIMULAÇÃO------------------------
void guarda_simulacao ( ) {
    FILE * ficheiro_exportacao; // Abre ou cria (se não existir), o ficheiro em modo escrita para acrescentar linhas
    ficheiro_exportacao = fopen ( "monitor.log" , "a" ); // Abre ou cria (se não existir), o ficheiro em modo escrita para acrescentar linhas

    // Baseado em: http://www.cplusplus.com/reference/ctime/localtime/
    time_t rawtime;
    struct tm * timeinfo;
    time ( & rawtime );
    timeinfo = localtime ( & rawtime );
    //
    fprintf ( ficheiro_exportacao , "\nSimulação de: %s \n" , asctime ( timeinfo ) );
    fprintf ( ficheiro_exportacao , "Número de clientes VIP: %d \n" , n_vip );
    fprintf ( ficheiro_exportacao , "Número de clientes NORMAL: %d \n\n" , n_normal );

    fprintf ( ficheiro_exportacao , "*** Zona 1 *** \n" );
    fprintf ( ficheiro_exportacao , "Número de pessoas que entraram na atração: %d \n" , toboga . pessoas_que_entraram );
    fprintf ( ficheiro_exportacao , "Número de pessoas que usufruíram da atração: %d \n" , toboga . pessoas_que_usufruiram );
    fprintf ( ficheiro_exportacao , "Número de boias despachadas: %d \n" , toboga . boias_seguidas );
    fprintf ( ficheiro_exportacao , "Número de desistências: %d \n" , toboga . desistencias );
    fprintf ( ficheiro_exportacao , "Número de pessoas na fila de espera: %d \n" , toboga . pessoas_fila_espera );
    fprintf ( ficheiro_exportacao , "Tempo máximo de espera: %d \n" , maximo ( toboga . tempos , toboga . i ) );
    fprintf ( ficheiro_exportacao , "Tempo médio de espera: %.2f \n" , media ( toboga . tempos , toboga . i ) );
    fprintf ( ficheiro_exportacao , "%d \n\n" , toboga . pessoas_que_usufruiram + toboga . desistencias + toboga . pessoas_fila_espera );

    fprintf ( ficheiro_exportacao , "*** Zona 2 *** \n" );
    fprintf ( ficheiro_exportacao , "Número de pessoas que entraram na atração: %d \n" , piscina . pessoas_que_entraram );
    fprintf ( ficheiro_exportacao , "Número de pessoas que usufruíram da atração: %d \n" , piscina . pessoas_que_usufruiram );
    fprintf ( ficheiro_exportacao , "Número de pessoas dentro da piscina: %d \n" , piscina . pessoas_dentro_da_piscina );
    fprintf ( ficheiro_exportacao , "Número de desistências: %d \n" , piscina . desistencias );
    fprintf ( ficheiro_exportacao , "Tempo máximo de espera: %d \n" , maximo ( piscina . tempos , piscina . i ) );
    fprintf ( ficheiro_exportacao , "Número de pessoas na fila de espera: %d \n" , piscina . pessoas_fila_espera );
    fprintf ( ficheiro_exportacao , "%d \n\n" , piscina . pessoas_que_usufruiram + piscina . desistencias + piscina . pessoas_fila_espera );

    fprintf ( ficheiro_exportacao , "*** Zona 3 *** \n" );
    fprintf ( ficheiro_exportacao , "Número de pessoas que entraram na atração: %d \n" , montanha_russa . pessoas_que_entraram );
    fprintf ( ficheiro_exportacao , "Número de pessoas que usufruíram da atração: %d \n" , montanha_russa . pessoas_que_usufruiram );
    fprintf ( ficheiro_exportacao , "Número de viagens: %d \n" , montanha_russa . viagens_de_carro );
    fprintf ( ficheiro_exportacao , "Tempo médio de espera: %.2f \n" , media ( montanha_russa . tempos , montanha_russa . i ) );

    fclose ( ficheiro_exportacao );
}

void trata_mensagem ( char * buffer ) {     
    char acontecimento_temp[2] , tempo_espera_temp[10] , horas_temp[2] , minutos_temp[2];  //são strings
    sscanf ( buffer , "%[^\n=]=%[^\n=]=%[^\n=]=%[^\n]" , acontecimento_temp , tempo_espera_temp , horas_temp , minutos_temp );
    int acontecimento = atoi ( acontecimento_temp );   //QUAIS SÃO OS ACONTECIMENTOS,HORAS,MINUTOS E TEMPO DE ESPERA ATUAIS NA SIMULAÇÃO
    int tempo_espera = atoi ( tempo_espera_temp );     //int atoi(const char *str) converts the string argument str to an integer (type int).
    int horas = atoi ( horas_temp );
    int minutos = atoi ( minutos_temp );
    char mensagem[50];   

    switch ( acontecimento ) {           // sempre que executa um acontecimento   ele le o bufer que contem todos os acontecimentos
        case 0:                
            printf ( "A simulação começou!\n" );
            break;
        case 1:
            n_normal ++;               // numero de pessoas normais aumenta 
            break;
        case 2:
            n_vip ++;                  // numero de pessoas vip aumenta 
            break;
        case 3:
            toboga . pessoas_que_entraram ++;        // pessoas no toboga aumenta 
            break;
        case 4:
            toboga . pessoas_que_usufruiram ++;        // pessoas que ja usaram o toboga
            break;
        case 5:
            toboga . boias_seguidas ++;                 // boias existentes
            break;
        case 6:
            toboga . desistencias ++;                // numero de desistencias no toboga 
            break;
        case 7:
            toboga . tempos[ toboga . i ] = tempo_espera;            // aumento do tempo de espera
            toboga . i ++;
            break;
        case 8:
            toboga . pessoas_fila_espera ++;          // mais pessoas a espera
            break; 
        case 9:
            toboga . pessoas_fila_espera --;           // menos pessoas a espera 
            break;            
        case 10:
            piscina . pessoas_que_entraram ++;
            break;
        case 11:
            piscina . pessoas_que_usufruiram ++;
            break;
        case 12:
            piscina . pessoas_dentro_da_piscina ++;
            break;
        case 13:
            piscina . pessoas_dentro_da_piscina --;
            break;
        case 14:
            piscina . pessoas_fila_espera ++;
            break;
        case 15:
            piscina . pessoas_fila_espera --;
            break;
        case 16:
            piscina . desistencias ++;
            break;
        case 17:
            if(tempo_espera>0){
				piscina . tempos[ piscina . i ] = tempo_espera;
				piscina . i ++;
            }
            break;
        case 18:
            montanha_russa . pessoas_que_entraram ++;
            break;
        case 19:
            montanha_russa . pessoas_que_usufruiram ++;
            break;
        case 20:
            montanha_russa . viagens_de_carro ++;
            break;
        case 21:
            sprintf ( mensagem , "carrega carrinho %d" , tempo_espera );
            strcpy ( montanha_russa . estado_porto_carga , mensagem );
            break;
        case 22:
            sprintf ( mensagem , "seguiu carrinho %d" , tempo_espera );
            strcpy ( montanha_russa . estado_porto , mensagem );
            break;
        case 23:
            sprintf ( mensagem , "descarregou carrinho %d " , tempo_espera );
            strcpy ( montanha_russa . estado_porto_descarga , mensagem );
            break;
        case 24:
            if(tempo_espera>0){
                montanha_russa . tempos[ montanha_russa . i ] = tempo_espera;
                montanha_russa . i ++;
            }
            break;
        case 50:
            printf ( "A simulação terminou! %d \n" , n_normal + n_vip );
            guarda_simulacao ( );
            fim_simulacao = 1;
            break;
    }
}

void reinicia_simulacao ( ) {
    n_normal = 0;
    n_vip = 0;
    toboga .pessoas_que_entraram = 0;
    toboga .pessoas_que_usufruiram = 0;
    toboga .pessoas_fila_espera = 0;
    toboga .boias_seguidas = 0;
    toboga .desistencias = 0;
    toboga .tempo_medio_espera = 0;
    toboga .i = 0;
    memset (toboga .tempos , 0 , sizeof ( int ) * 9999 ); void *memset(void *ptr, int x, size_t n);
    piscina .pessoas_que_entraram = 0;
    piscina .pessoas_que_usufruiram = 0;
    piscina .pessoas_fila_espera = 0;
    piscina .pessoas_dentro_da_piscina = 0;
    piscina .desistencias = 0;
    piscina .tempo_medio_espera = 0;
    piscina .i = 0;
    memset (piscina.tempos , 0 , sizeof ( int ) * 9999 ); //  void *memset(void *ptr, int x, size_t n);

    montanha_russa. pessoas_que_entraram = 0;
    montanha_russa. pessoas_que_usufruiram = 0;
    montanha_russa . pessoas_fila_espera = 0;
    montanha_russa . viagens_de_carro = 0;
    montanha_russa . i;

    memset ( montanha_russa . estado_porto_carga , 0 , sizeof ( montanha_russa . estado_porto_carga ) );
    memset ( montanha_russa . estado_porto_descarga , 0 , sizeof ( montanha_russa . estado_porto_descarga ) );
    memset ( montanha_russa . estado_porto , 0 , sizeof ( montanha_russa . estado_porto ) );
    memset ( montanha_russa . tempos , 0 , sizeof ( int ) * 9999 );

    printf ( "Dados limpos \n" );
}

void imprime_resultados ( ) {
    //system("clear");    
    printf ( "\n \n Estado atual: simulação a decorrer - clientes = %d \n" , n_normal + n_vip );
    printf ( "Número de clientes VIP: %d \n" , n_vip );
    printf ( "Número de clientes NORMAL: %d \n\n" , n_normal );

    printf ( "\n *** Zona 1 *** \n" );
    printf ( "Número de pessoas que entraram atração: %d \n" , toboga . pessoas_que_entraram );
    printf ( "Número de pessoas que usufruíram da atração: %d \n" , toboga . pessoas_que_usufruiram );
    printf ( "Número de boias despachadas: %d \n" , toboga . boias_seguidas );
    printf ( "Número de desistências: %d \n" , toboga . desistencias );
    printf ( "Número de pessoas na fila de espera: %d \n" , toboga . pessoas_fila_espera );
    printf ( "Tempo médio de espera: %.2f \n" , media ( toboga . tempos , toboga . i ) );

    printf ( "\n *** Zona 2 *** \n" );
    printf ( "Número de pessoas que entraram atração: %d \n" , piscina . pessoas_que_entraram );
    printf ( "Número de pessoas que usufruíram da atração: %d \n" , piscina . pessoas_que_usufruiram );
    printf ( "Número de pessoas dentro da piscina: %d \n" , piscina . pessoas_dentro_da_piscina );
    printf ( "Número de desistências: %d \n" , piscina . desistencias );
    printf ( "Tempo máximo de espera: %d \n" , maximo ( piscina . tempos , piscina . i ) );
    printf ( "Número de pessoas na fila de espera: %d \n" , piscina . pessoas_fila_espera );

    printf ( "\n *** Zona 3 *** \n" );
    printf ( "Número de pessoas que entraram atração: %d \n" , montanha_russa . pessoas_que_entraram );
    printf ( "Número de pessoas que usufruíram da atração: %d \n" , montanha_russa . pessoas_que_usufruiram );
    printf ( "Número de viagens: %d \n" , montanha_russa . viagens_de_carro );
    printf ( "Estado do porto: %s \n" , montanha_russa . estado_porto );
    printf ( "Estado do porto de carga: %s \n" , montanha_russa . estado_porto_carga );
    printf ( "Estado do porto de descarga: %s \n" , montanha_russa . estado_porto_descarga );
    printf ( "Tempo médio de espera: %.2f \n" , media ( montanha_russa . tempos , montanha_russa . i ) );
}

void trata_cliente_stream ( int socketfd ) {
    int n = 0;
    char buffer[MAXLINHA];         //MAXLINHA DEFINIDO COMO 1024   cria um array de chars de 1024
    // Lê a mensagem do socket até à dimensão da zona tampão
    while ( ! fim_simulacao ) {                                 // enquanto que n acabou a simulação 
        n = read ( socketfd , buffer , MAXLINHA );              // le  a mensagem do socket    
        if ( n == 0 ) {                                         // caso chegar ao fim 
            return;
        } else if ( n < 0 ) {        
            printf ( "Erro na leitura! \n" );
        }
        trata_mensagem ( buffer );
        imprime_resultados ( );
        /*if ( write ( socketfd , buffer , n ) != n ) {
            printf ( "Erro do servidor no write! \n" );
        } else {
            trata_mensagem ( buffer );
        }*/
    }
}
//int i = 0 , j = 0; // iteradores para os 'arrays'

void cria_servidor () {
    int socketfd, novo_socket, dim_cli, dim_serv, pid_filho;
    struct sockaddr_un end_cli , end_serv;             
	//----http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
    // 1. Criar socket stream  Primeiro enderelo é o dominio do socket, o segundo é otipo de socket, o terceiro é o protocolo se for zero ele escolhe o mehor protocolo tcp
    if ( ( socketfd = socket ( AF_UNIX , SOCK_STREAM , 0 ) ) < 0 ) {
        printf ( "erro ao criar o socket stream servidor \n" );
    }
    // Atribuir um nome ao servidor para que os clientes o possam identificar
    bzero((char*) & end_serv , sizeof (end_serv));        // mete todos os valor do buffer para zero  primeiro argumento e o pointer to the buffer, e o segundo é o tamanho 
    end_serv.sun_family = AF_UNIX;                       // qual é o tipo de socket a sua familia 
    strcpy ( end_serv . sun_path , UNIXSTR_PATH );
    dim_serv = strlen ( end_serv.sun_path ) + sizeof (end_serv.sun_family );
    // Elimina um ficheiro socket, se criado anteriormente
    unlink ( UNIXSTR_PATH );
    // Atribuição do nome
    if ( bind ( socketfd , ( struct sockaddr * ) & end_serv , dim_serv ) < 0 ) {
        printf ( "Erro ao atribuir nome socket servidor! \n" );
    }
    // O socket passa a ouvir um máximo de 1 ligações
    printf ( "À espera do simulador... \n" );
    listen ( socketfd , 1 );
    // Indica a dimensão inicial (alternável) da estrutura
    dim_cli = sizeof (end_cli);
    // Aceita ligação com o cliente
    novo_socket = accept ( socketfd , ( struct sockaddr * ) & end_cli , & dim_cli );
    if ( novo_socket < 0 ) {
        printf ( "Erro ao criar ligação dedicada - accpet! \n" );
    }
    // Cria processo-filho para tratar com o cliente
    if ( ( pid_filho = fork ( ) ) < 0 ) {
        printf ( "Erro ao criar processo-filho para atender ao cliente! \n" );
    } else if ( pid_filho == 0 ) {
        // Já não é utilizado por s
        close ( socketfd );
        trata_cliente_stream ( novo_socket );
    }
    // Processo-pai: fechar novo_socket já que não é utilizado pelo processo-pai
    close ( novo_socket );
}

int main ( int argc , char const * argv[] ) {
    piscina . pessoas_dentro_da_piscina = 0;
    printf ( "******** Bem vindo! ******** \n" );
    printf ( "* 1- Começar simulação     * \n" );
    printf ( "**************************** \n" );
    int opcao = 0;
    while ( 1 ) {
        opcao = 0;        // Até inserir a uma opção correta
        while ( opcao != 1 ) {
            printf ( "Introduza uma opção: \n" );
            scanf ( "%d" , & opcao );       // vê o que o utilizador introduz 
        }
        if ( opcao == 1 ) {            // Criar socket e fica à espera
            reinicia_simulacao ();      // introduz os valores a zero 
            cria_servidor ();            // cria o servidor
        }
    }
}
