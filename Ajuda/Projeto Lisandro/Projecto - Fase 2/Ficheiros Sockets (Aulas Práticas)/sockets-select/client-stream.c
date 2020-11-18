
/* Envia o que estiver em argv[1] para o servidor por liga‡ƒo */

#include "names.h"

#define USAGEMSG "usage: client-stream msg"
#define ERRORMSG1 "client-stream: cannot open stream socket"
#define ERRORMSG2 "client-stream: cannot connect to server"
#define ERRORMSG3 "client-stream: write msg error"

int main(int argc, char *argv[])
{
  int sockfd, len;
  struct sockaddr_un servstrmaddr;

  if(argc < 2) {
    perror(USAGEMSG);
    exit(1);
  }

  /* Cria socket stream */

  if ((sockfd= socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    perror(ERRORMSG1);
    exit(1);
  }

  /* Dados para o socket stream: tipo + nome do ficheiro 
     (depois de uma limpeza preventiva!) */

  bzero((char *)&servstrmaddr,sizeof(servstrmaddr));
  servstrmaddr.sun_family = AF_UNIX;
  strcpy(servstrmaddr.sun_path,UNIXSTR_PATH);

  /* Tenta estabelecer uma liga‡ƒo. S funciona se o servidor tiver 
     sido lan‡ado primeiro (o servidor tem de criar o ficheiro e associar
     o socket ao ficheiro). 
     A dimensao a indicar ao connect nƒo ‰ a da estrutura, pois depende
     do nome do ficheiro */

  len = strlen(servstrmaddr.sun_path) + sizeof(servstrmaddr.sun_family);
  if(connect(sockfd,(struct sockaddr *)&servstrmaddr,len) < 0) {
    perror(ERRORMSG2);
    exit(1);
  }

  /* Escreve no socket */

  if(write(sockfd,argv[1],strlen(argv[1])) < strlen(argv[1]))  {
    perror(ERRORMSG3);
    exit(1);
  } 

  /* Fecha liga‡ƒo */
  
  close(sockfd);
  exit(0);
}


