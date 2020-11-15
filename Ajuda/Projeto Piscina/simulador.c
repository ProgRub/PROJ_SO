
#include <math.h>

#define MAX_ARG 20                      // Podem ser importados até 10 argumentos
#define TAREFAS 99999                   // NUMERO DE TAREFAS A SER EXECUTADO 
#define RECURSOS 100                  // numero de recursos 

//int jp;
int id = 0;
int socketfd = 0;

typedef sem_t Semaphore;
Semaphore * mutex_socket;                 // make semaphore (1)
time_t tempo_inicial_simulacao;

int tempo_decrescente = 0;               // necessario para o tempo de simulação
int tempo_atual_simulacao;
int segundos_simulacao = 0;
int valor_segundo = 0;
int tempo_medio_normal = 0;
int tempo_medio_vip = 0;
int tmp_simulacao = 0;
int n_clientes = 0;
int tempo_passado = 0;
int valor_segundo_con = 0;
// Gerador de tempos de chegada de normais e de VIPs
int tempo_aparece_normal = 0;
int tempo_aparece_vip = 0;
int tempo_decorrido = 0;

// Estruturas
// Estrutura de um argumento
struct argumento {
    char nome[100];
    int valor;
}; 
// Estrutura de um cliente
struct cliente {
    int id;
    int tipo; // 1 -> VIP, 0 -> NORMAL
} cliente; 
// Clientes
struct cliente clientes[TAREFAS];
// Argumentos
struct argumento argumentos[MAX_ARG]; // Array que possui todos os argumentos importados

// Tarefas
pthread_t thr[TAREFAS];
pthread_t recursos[TAREFAS];

// ############################ Sockets ############################
int cria_socket ( ) {
    struct sockaddr_un end_serv;
    int dim_serv;
    // 1. Criar um socket
    int socketfd = socket ( AF_UNIX , SOCK_STREAM , 0 );
    if ( socketfd < 0 ) {
        printf ( "Erro na criação do socket! \n" );
    }
    // 2. Atribuir um nome ao socket (colocando tudo a zero)
    // void bzero(void *s, size_t n);
    bzero ((char *) & end_serv , sizeof (end_serv));
    // 3. Indica o domínio' do socket
    end_serv.sun_family = AF_UNIX;
    // 4. No domínio UNIX a estrutura recebe um nome de ficheiro
    strcpy (end_serv.sun_path,UNIXSTR_PATH);
    dim_serv = strlen (end_serv.sun_path) + sizeof ( end_serv . sun_family );
    // 5. Estabelecer uma ligação (socket + nome associado)
    if ( connect ( socketfd , ( struct sockaddr * ) & end_serv , dim_serv ) < 0 ) {
        printf ( "Erro em estabelecer a ligação! \n" );
    }
    return socketfd;
}

void envia_recebe_stream ( char * fp , int sockfd ) {            // irá ver o que contem no buffer do socket  [0=0=0=0]
    sem_wait ( mutex_socket );                 // trinco do socket esperar (WAIT) 
    // região critica();
	usleep ( 200 );  //shall cause the calling thread to be suspended from execution until either the number of realtime microseconds specified by the argument useconds has elapsed or a signal is delivered to the calling thread and its action is to invoke a signal-catching function or to terminate the process. The suspension time may be longer than requested due to the scheduling of other activity by the system.
    int n;                                                       
    char sendline[MAXLINHA] , recvline[MAXLINHA + 1];  // Cria um buffer com o MAXLINHA + 1 para o caractere de terminação [1024]
    if ( strcpy ( sendline , fp ) != 0 ) { 
        n = strlen ( sendline ) + 1;           //function takes a string as an argument and returns its length
        if ( write ( sockfd , sendline , n ) != n ) {  // The C library function char *strcpy(char *dest, const char *src) copies the string pointed to, by src to dest.
            printf ( "Erro cliente no write! \n" );
        }
    }                                                                      
    sem_signal ( mutex_socket );          //trinco do socket signal é assinalar 
} 
// ------------------------------------------------ Socket------------------ 

// ############################ Parque aquático ############################
// ----------------- Zona 1 -----------------  
int pessoas_por_boia = 0;
int max_fila_zona_1 = 0;
int fila_espera_zona_1 = 0;
int tempo_maximo_espera_zona_1 = 0;
int passageiros = 0;
int nao_passageiros = 0;
Semaphore * mutex;
Semaphore * mutex1;
Semaphore * fila_espera;
Semaphore * fila_nao_espera;
Semaphore * todos_a_bordo;
Semaphore * todos_em_terra;
Semaphore * mutex_max_fila_zona_1;

void * boia () {
    for ( ;; ) { // enquanto estiver a correr a simulação, coloca a boia ao dispor
        printf ( "ZONA 1: boia pronta. \n" );
        int i = 0;
        //1. Sinalizar X clientes de acordo com a capacidade máxima por boia
            for ( i = 0 ; i < pessoas_por_boia ; ++ i ) {         // maximo de pessoas na boia são 2 
                sem_signal ( fila_espera );                // asinala   aumenta da fila de espera começou a zero 
            }
        //2. Aguarda pela boia cheia
            sem_wait (todos_a_bordo);                     // decrementa a capacidade da boia 
            envia_recebe_stream ( "5=0=0=0" , socketfd ); // informa ao socket que foi enviado uma boia
        //3. A boia anda pelo tobogã
            usleep ( 120 * valor_segundo_con );
        //4. A boia chegou ao fim do tobogã
            printf ( "ZONA 1: chegou a boia. \n" );
            //4.1 Sinaliza os clientes para saírem 
                i = 0;
                for ( i = 0 ; i < pessoas_por_boia ; ++ i ) {
                    sem_signal ( fila_nao_espera );
                } 
        //5. Espera que todos os clientes saiam
            sem_wait ( todos_em_terra );
    }
}

void * passageiro_boia ( void * ptr ) {
    int * id_local = (int *) ptr;
    int tempo_atual_local = tempo_decorrido;           // tempo atual é igual ao tempo já percorrido

    envia_recebe_stream ( "3=0=0=0" , socketfd );      // informa ao socket que entrou uma pessoa   // ver case de acontecimentos no monitor.c
    printf("ZONA 1: o cliente %d entrou na zona 2.\n", (*id_local));

    // 0. Verificar se tem mais do que 'n' pessoas na filha:
        sem_wait ( mutex_max_fila_zona_1 );
        if ( fila_espera_zona_1 >= max_fila_zona_1 ) {
            envia_recebe_stream ( "6=0=0=0" , socketfd ); 
            printf("ZONA 1: o cliente %d desistiu da zona 2 (fila espera).\n", (*id_local));           
            sem_signal ( mutex_max_fila_zona_1 );
        } else {
            //1. Incrementa a fila de espera:
                fila_espera_zona_1 ++;
                envia_recebe_stream ( "8=0=0=0" , socketfd ); // informa ao socket para incrementar o número de clientes na fila de espera
                sem_signal ( mutex_max_fila_zona_1 );
            //2. Espera até que chegue uma boia:
                sem_wait ( fila_espera );    
            //3. Verifica se esteve muito tempo à espera:
                sem_wait (mutex);
                int tempo_espera = tempo_decorrido - tempo_atual_local;
                if ( tempo_espera > tempo_maximo_espera_zona_1 ) {
                    //3.1 Como esteve muito tempo à espera:
                        //3.1.2 Decrementa fila de espera
                            sem_wait ( mutex_max_fila_zona_1 );
							fila_espera_zona_1--;
                            envia_recebe_stream ( "9=0=0=0" , socketfd ); // informa o socket que tem menos um cliente na fila de espera
                            envia_recebe_stream ( "6=0=0=0" , socketfd ); // informa o socket um cliente desistiu
                            printf("ZONA 1: o cliente %d desistiu da zona 2 (%d à espera).\n", (*id_local), tempo_espera);
                            sem_signal ( mutex_max_fila_zona_1 );
                             //3.1.1 Sinaliza um cliente para entrar
                            sem_signal ( fila_espera );
                            sem_signal ( mutex );
                } else {
                    char mensagem_a_enviar[50];
                    sprintf ( mensagem_a_enviar , "7=%d=0=0" , tempo_espera ); // cria a mensagem a enviar
                    envia_recebe_stream ( mensagem_a_enviar , socketfd );
                    //4. Incrementa o número de clientes na boia
                        passageiros ++;
                    //5. Verifica se a boia já tem a quantidade máxima de clientes
                        if ( passageiros == pessoas_por_boia ) {
                            //5.1 Informa a boia que está pronta para avançar
                                sem_signal ( todos_a_bordo );
                                passageiros = 0;
                            //5.2 Atualiza o número de pessoas na fila de espera
                                sem_wait ( mutex_max_fila_zona_1 );
                                fila_espera_zona_1 -= pessoas_por_boia;
                                int i = 0;
                                for ( i = 0 ; i < pessoas_por_boia ; ++ i ) {
                                    envia_recebe_stream ( "9=0=0=0" , socketfd ); // informa o socket que estão menos n pessoas à espera
                                }
                                sem_signal ( mutex_max_fila_zona_1 );
                        }
                        sem_signal ( mutex );
                        printf ( "ZONA 1: o cliente %d segue na boia, esteve %d segundos à espera! \n" , (*id_local) , tempo_espera );
                        envia_recebe_stream ( "4=0=0=0" , socketfd ); // informa o socket que um cliente já usufruiu da atração
                        //6. Espera até a boia chegar
                            sem_wait ( fila_nao_espera );
                            sem_wait ( mutex1 );

                            nao_passageiros ++;

                            if ( nao_passageiros == pessoas_por_boia ) {

                                //7. Indica à boia que já foram descarregados os clientes e que pode voltar ao início do tobogã

                                sem_signal ( todos_em_terra );

                                nao_passageiros = 0;
                            }
                        sem_signal ( mutex1 );
                }
        }
}
// ----------------- Zona 2 -----------------

int capacidade_da_piscina = 0;
int fila_espera_zona_2 = 0;
int max_espera_zona_2 = 0;
Semaphore * pode_entrar_piscina;
Semaphore * mutex_fila_de_espera_piscina;
Semaphore * mutex_socket_pessoas_usufruir;

void * nadador ( void * ptr ) {

    int * id_local = (int *) ptr;

    int tempo_local = tempo_decorrido;

    printf("ZONA 2: o cliente %d entrou na zona 2.\n", (*id_local));

    envia_recebe_stream ( "10=0=0=0" , socketfd ); // informa o socket que chegou um cliente à zona

    // 0. Verificar se tem mais do que 'n' pessoas na filha

        sem_wait ( mutex_fila_de_espera_piscina );

        if ( fila_espera_zona_2 >= max_espera_zona_2 ) {

            envia_recebe_stream ( "16=0=0=0" , socketfd ); // informa o socket que desistiu um cliente

            printf("ZONA 2: o cliente %d desistiu da zona 2.\n", (*id_local));

            sem_signal ( mutex_fila_de_espera_piscina );

        } else{

            sem_signal ( mutex_fila_de_espera_piscina );

            // 1. Incrementar o número de pessoas na fila:

                sem_wait ( mutex_fila_de_espera_piscina );

                    fila_espera_zona_2 ++;

                    envia_recebe_stream ( "14=0=0=0" , socketfd ); // Aumenta número de pessoas à espera

                sem_signal ( mutex_fila_de_espera_piscina );

            // 2. Esperar até poder entrar na piscina

                sem_wait ( pode_entrar_piscina );

            // 3. Entrar na piscina:

                    // 3.1 Decrementar o número de pessoas na fila:

                        sem_wait ( mutex_fila_de_espera_piscina );

                            fila_espera_zona_2 --;

                            envia_recebe_stream ( "15=0=0=0" , socketfd );

                        sem_signal ( mutex_fila_de_espera_piscina );

                    envia_recebe_stream ( "11=0=0=0" , socketfd ); // informa o socket que entrou um cliente

                    envia_recebe_stream ( "12=0=0=0" , socketfd ); // informa o socket que está um cliente dentro da piscina

                    // 3.2 Calcular o tempo de espera:
            
                        int tempo_espera = tempo_decorrido - tempo_local;

                        char mensagem_a_enviar[50];

                        sprintf ( mensagem_a_enviar , "17=%d=0=0" , tempo_espera ); 

                        envia_recebe_stream ( mensagem_a_enviar , socketfd ); // informa o socket o tempo de espera do cliente 

                    printf ( "ZONA 2: o cliente %d entrou para a piscina, esteve %d segundos à espera. \n", (*id_local), tempo_espera );

                    int tempo_de_utilizacao = 180 + valor_aleatorio(1000); // vai usufruir entre 1 a 5 minutos

                    usleep ( tempo_de_utilizacao * valor_segundo_con );

                    printf ( "ZONA 2: o cliente %d saiu da piscina, esteve %d segundos lá. \n" , (*id_local), tempo_de_utilizacao );

                    envia_recebe_stream ( "13=0=0=0" , socketfd ); // informa ao socket que saiu uma pessoa da piscina

            // 4. Sinaliza outras pessoas para poderem entrar
                sem_signal ( pode_entrar_piscina );
        }
}
// ----------------- Zona 3 -----------------

int passageiros_zona_3 = 0;
int nao_passageiros_zona_3 = 0;
int capacidade_por_carro = 0; 
int numero_de_carros = 0; 
Semaphore * mutex_passageiros;
Semaphore * mutex_nao_passageiros;
Semaphore * agente_entrada;
Semaphore * agente_saida;
Semaphore * fila_espera_zona_3;
Semaphore * fila_descarga_zona_3;
Semaphore * todos_a_bordo_carro;
Semaphore * todos_em_terra_carro;


int id_carro = 1;

void * carro () {

    char mensagem_a_enviar[50];

    int id_carro_local = id_carro;

    id_carro ++;

    while ( 1 ) {

        sem_wait ( agente_entrada );

        sprintf ( mensagem_a_enviar , "21=%d=0=0" , id_carro_local ); // cria a mensagem a enviar

        envia_recebe_stream ( mensagem_a_enviar , socketfd );

        printf ( "ZONA 3: carrega carrinho (%d). \n" , id_carro_local );

        int i = 0;

        for ( i = 0 ; i < capacidade_por_carro ; ++ i ) {
            sem_signal ( fila_espera_zona_3 );
        }
        sem_wait ( todos_a_bordo_carro );
        sem_signal ( agente_entrada );
        envia_recebe_stream ( "20=0=0=0" , socketfd );
        sprintf ( mensagem_a_enviar , "22=%d=0=0" , id_carro_local ); // cria a mensagem a enviar
        envia_recebe_stream ( mensagem_a_enviar , socketfd );
        printf ( "ZONA 3: andando carrinho (%d). \n" , id_carro_local );
        usleep ( 100 * valor_segundo_con );
        sem_wait ( agente_saida );
        printf ( "ZONA 3: descarregando carrinho(%d). \n" , id_carro_local );
        i = 0;
        for ( i = 0 ; i < capacidade_por_carro ; ++ i ) {
            sem_signal ( fila_descarga_zona_3 );
        }
        sem_wait ( todos_em_terra_carro );
        sprintf ( mensagem_a_enviar , "23=%d=0=0" , id_carro_local ); // cria a mensagem a enviar
        envia_recebe_stream ( mensagem_a_enviar , socketfd );
        sem_signal ( agente_saida );
    }
}

void * passageiro_carro (void * ptr) {

    int * id_local = (int *) ptr;

    char mensagem_a_enviar[50];

    int tempo_local = tempo_decorrido;

    sem_wait ( fila_espera_zona_3 );

    sem_wait ( mutex_passageiros );

    int tempo_espera = tempo_decorrido - tempo_local;

    sprintf ( mensagem_a_enviar , "24=%d=0=0" , tempo_espera ); // cria a mensagem a enviar

    envia_recebe_stream ( mensagem_a_enviar , socketfd );


    // ########

    passageiros_zona_3 ++;

        printf("ZONA 3: o cliente %d embarcou no carro. \n",(*id_local) );

    if ( passageiros_zona_3 == capacidade_por_carro ) {

        sem_signal ( todos_a_bordo_carro );

        passageiros_zona_3 = 0;

        int i = 0;

        for ( i = 0 ; i < capacidade_por_carro ; ++ i ) {

            envia_recebe_stream ( "19=0=0=0" , socketfd );

        }

    }

    // ########

    sem_signal ( mutex_passageiros );


    sem_wait ( fila_descarga_zona_3 );


    sem_wait ( mutex_nao_passageiros );

    // ########

    nao_passageiros_zona_3 ++;

    if ( nao_passageiros_zona_3 == capacidade_por_carro ) {

        sem_signal ( todos_em_terra_carro );

        nao_passageiros_zona_3 = 0;

    }

    // ########

    sem_signal ( mutex_nao_passageiros );

}


// ############################ Parque aquático ############################



int carrega_argumentos ( char * nome_ficheiro ) {

    char buffer[1000]; // Irá armazenar todos os dados de configuração

    FILE * ficheiro_configuracao; // Apontador para o ficheiro

    ficheiro_configuracao = fopen ( nome_ficheiro , "r" ); // Indica o ficheiro a ser carregado (passado como argumento)

    if ( ficheiro_configuracao == NULL ) { // Verifica se foi aberto ou se o ficheiro existe

        return - 1; // Retorna a indicação de erro

    }

    // ** Variáveis temporárias **

    char nome_temp[100];
    char valor_temp[100];

    int i = 0; // Número de argumentos importados

    int n_linha = 0; // Número de linhas lidas

    while ( fgets ( buffer , 1000 , ficheiro_configuracao ) !=
            NULL ) { // Lê cada linha do ficheiro até chegar ao seu fim, armazenado as linhas no 'buffer' para depois extrair os argumentos

        if ( i != MAX_ARG ) { // Se não carregou a capacidade total do 'array'

            n_linha ++; // Incrementa o número de linhas lidas

            if ( sscanf ( buffer , "%[^\n=]=%[^\n]" , nome_temp , valor_temp ) ==
                 2 ) { // Verifica se a linha está bem formatada e guarda nas variáveis temporárias

                strcpy ( argumentos[ i ] . nome , nome_temp ); // Guarda o nome do atributo

                argumentos[ i ] . valor = atoi (
                        valor_temp ); // A função 'atoi()' serve para converter texto em inteiro, neste caso também guarda o conteúdo

                i ++; // Incrementa o número de argumentos (válidos) importados

            } else {

                printf ( "AVISO: conteúdo da linha (%d) nao carregado!\n" , n_linha );

            }

        } else {

            printf ( "AVISO: só poderão ser importados %d argumentos, os restantes serao ignorados! \n\n" , MAX_ARG );

            break; // Saí do ciclo

        }

    }

    fclose ( ficheiro_configuracao ); // Fecha o ficheiro

    return i; // Retorna o número de argumentos lidos

}

void imprime_resultados ( int resultado ) {
    printf ( "************* %d argumento(s) importado(s): ************* \n\n" , resultado );
    printf ( "*******************\n" );
    printf ( "* Nome  =>  Valor *\n" );
    printf ( "*******************\n" );
    int i;
    for ( i = 0 ; i < resultado ; i ++ ) {
        printf ( "%s => %d \n" , argumentos[ i ] . nome , argumentos[ i ] . valor );
    };
}

int argumento ( char * nome ) {
    int i = 0;
    int length = sizeof ( argumentos ) / sizeof ( argumentos[ 0 ] );

    for ( i = 0 ; i < length ; i ++ ) {

        if ( strcmp ( nome , argumentos[ i ] . nome ) == 0 ) {

            return argumentos[ i ] . valor;

        }

    };


    return - 1;

}

int valor_aleatorio ( int max ) {
    unsigned int iseed = ( unsigned int ) time ( NULL );
    srand ( iseed );
    return rand ( ) % max + 1;
}

void gerador_tempo_medio_chegada ( int tipo ) {
    if ( tipo == 0 ) {
        int valor_max_normal = tempo_medio_normal + 3;
        int valor_min_normal = tempo_medio_normal - 3;
        tempo_aparece_normal =
                tempo_decorrido + ( valor_aleatorio ( valor_max_normal - valor_min_normal ) + valor_min_normal );
    }

    if ( tipo == 1 ) {
        int valor_max_vip = tempo_medio_vip + 4;
        int valor_min_vip = tempo_medio_vip - 4;
        tempo_aparece_vip = tempo_decorrido + ( valor_aleatorio ( valor_max_vip - valor_min_vip ) + valor_min_vip );
    }
}

void make_thread ( int tipo ) {     // cria tarefa
    id ++;         // incrementa o id
    int n;         
    int probabilidade = valor_aleatorio ( 100 );        // probabilidade e um numero aleatoria de 0-99
    if ( tipo == 0 ) {                // caso o tipo de cliente seja normal
        envia_recebe_stream ( "1=0=0=0" , socketfd );
    }
    if ( tipo == 1 ) {                  // caso o tipo de cliente seja VIP
        envia_recebe_stream ( "2=0=0=0" , socketfd );
    }
	//----------
    if ( probabilidade <= 33 ) { //20% probabilidade de entrar nesta atracao
        pthread_create ( & thr[ n_clientes ] , NULL , passageiro_boia , &id );
    }
    if ( probabilidade > 33 && probabilidade <= 66) {
        pthread_create ( & thr[ n_clientes ] , NULL , nadador , &id );
    }
    if ( probabilidade > 66) {
        envia_recebe_stream ( "18=0=0=0" , socketfd );
        int n = pthread_create ( & thr[ n_clientes ] , NULL , passageiro_carro , &id );
    }
    clientes[ n_clientes ] . tipo = tipo;
}

void simulacao ( int socketfd ) {
    tempo_medio_normal = argumento ( "tempo medio normal" );
    tempo_medio_vip = argumento ( "tempo medio vip" );
    // TEMPO DE SIMULAÇÃO
    segundos_simulacao = argumento ( "tempo simulacao" );
    tempo_decrescente = segundos_simulacao / 60;
    int segundos_reais = argumento ( "tempo real" );
    float valor_segundo = segundos_reais / ( float ) segundos_simulacao;
    valor_segundo_con = (int) ( valor_segundo * 1000000 );
    tempo_atual_simulacao = 0;
    tempo_inicial_simulacao = time ( NULL );
    // FIM - TEMPO DE SIMULAÇÃO
    int cria_normal = 1;
    int cria_vip = 1;
    pthread_t thread[TAREFAS];
    envia_recebe_stream ( "0=0=0=0" , socketfd ); // informa o socket que vai começar uma simulação
    //inicia a simulação
	while ( tempo_atual_simulacao < segundos_reais ) {
        usleep (valor_segundo_con);
        tempo_decorrido ++;
        if (cria_normal) {
            gerador_tempo_medio_chegada ( 0 );
            cria_normal = 0;
        }
        if (cria_vip) {
            gerador_tempo_medio_chegada ( 1 );
            cria_vip = 0;
        }
        if ( tempo_aparece_normal < tempo_decorrido ) {
            make_thread (0);
            n_clientes ++;
            cria_normal = 1;
        }
        if ( tempo_aparece_vip < tempo_decorrido ) {
            make_thread (1);
            n_clientes ++;
            cria_vip = 1;
        }
        tempo_atual_simulacao = difftime ( time ( NULL ) , tempo_inicial_simulacao );
    }  
    envia_recebe_stream ( "50=0=0=0" , socketfd );    // irá acbabar a simulação
    //int i = 0;
    /* for (i = 0; i < TAREFAS; i++) {
         pthread_join(thr[i], NULL); // Junta todas as 'threads' para que todas terminem
     }
 */
}
// ++++++++++++++++++++++++ Semáforos ++++++++++++++++++++++++
/********** Função que retorna um apontador para a memória alocada **********/
void * check_malloc ( int size ) {
    void * p = malloc ( size );
    if ( p == NULL ) {
        perror ( "Erro: malloc" );
        exit ( - 1 );
    }
    return p;
}
/********** Função que cria um semáforo **********/
Semaphore * make_semaphore ( int n ) {
    Semaphore * sem = check_malloc ( sizeof ( Semaphore ) );
    int ret = sem_init ( sem , 0 , n );
    if ( ret == - 1 ) {
        perror ( "Erro: sem_init" );
        exit ( - 1 );
    }
    return sem;
}
/********** Função assinalar dos semáforos **********/
int sem_signal ( Semaphore * sem ) {
    return sem_post ( sem );
}
// ++++++++++++++++++++++++ Semáforos ++++++++++++++++++++++++

int main ( int argc , char const * argv[] ) {
    int resultados = carrega_argumentos ( "simulador.conf" );
    mutex_socket = make_semaphore ( 1 );
    // Zona 1:
	//criação de semoforas e trincos para a zona 1 
    pessoas_por_boia = argumento ( "pessoas por boia" );
    tempo_maximo_espera_zona_1 = argumento ( "tempo maximo espera zona 1" );
    max_fila_zona_1 = argumento ( "pessoas fila zona 1" );
	//semaforo da zona 1
    mutex = make_semaphore ( 1 );
    mutex1 = make_semaphore ( 1 );
    fila_espera = make_semaphore ( 0 );
    fila_nao_espera = make_semaphore ( 0 );
    todos_a_bordo = make_semaphore ( 0 );
    todos_em_terra = make_semaphore ( 0 );
    mutex_max_fila_zona_1 = make_semaphore ( 1 );

    //Zona 2:
    max_espera_zona_2 = argumento ( "pessoas fila zona 2" );
    capacidade_da_piscina = argumento ( "capacidade da piscina" );
	//semaforo da zona 2 
    mutex_fila_de_espera_piscina = make_semaphore ( 1 );
    pode_entrar_piscina = make_semaphore ( capacidade_da_piscina );
    mutex_socket_pessoas_usufruir = make_semaphore(1);

    //Zona 3:
    capacidade_por_carro = argumento ( "pessoas por carro zona 3" );
    numero_de_carros = argumento ( "numero carros zona 3" );
	//semaforo da zona 3
    mutex_passageiros = make_semaphore ( 1 );
    mutex_nao_passageiros = make_semaphore ( 1 );
    fila_espera_zona_3 = make_semaphore ( 0 );
    fila_descarga_zona_3 = make_semaphore ( 0 );
    todos_a_bordo_carro = make_semaphore ( 0 );
    todos_em_terra_carro = make_semaphore ( 0 );
    agente_entrada = make_semaphore ( 1 );
    agente_saida = make_semaphore ( 1 );

    int j = 0;
    pthread_create ( & recursos[ j ] , NULL , boia , NULL );
    j ++;
    int i = 0;
    for ( i = 0 ; i < numero_de_carros ; ++ i ) {
        pthread_create ( & recursos[ j ] , NULL , carro , NULL );
        j ++;
    }
    socketfd = cria_socket ( );   //
    simulacao ( socketfd );
    close (socketfd);
    return 1;
}

