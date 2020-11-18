
/* Envia o que estiver em argv[1] para o servidor por datagrama */

#include "names.h"

#define USAGEMSG "usage: client-dgram msg"
#define ERRORMSG1 "client-dgram: cannot open dgram socket"
#define ERRORMSG2 "client-dgram: cannot bind dgram socket"
#define ERRORMSG3 "client-dgram: write msg error"

int main(int argc, char *argv[])
{
  int sockfd,clilen,servlen;
  struct sockaddr_un servdgrmaddr,clidgrmaddr;
  char *mktemp();

  if(argc < 2) {
    perror(USAGEMSG);
    exit(1);
  }

  /* Cria socket datagram */

  if ((sockfd= socket(AF_UNIX, SOCK_DGRAM,0)) < 0) {
    perror(ERRORMSG1);
    exit(1);
  }

  /* Dados para o socket datagram: tipo + nome do ficheiro temporßrio
     (depois de uma limpeza preventiva!)
     Note-se que no caso datagram nƒo ‰ criada uma liga‡ƒo permanente
     com o servidor. Por isso se vai inicializar o socket com um nome
     de ficheiro temporßrio, apenas conhecido pelo cliente */

  bzero((char *)&clidgrmaddr,sizeof(clidgrmaddr));
  clidgrmaddr.sun_family = AF_UNIX;
  strcpy(clidgrmaddr.sun_path,UNIXDG_TMP);

  /* O cliente tem de criar um ficheiro temporßrio.
     Limpa ficheiro temporßrio, para o caso de algo ter ficado pendurado.
     Em seguida associa o socket ao ficheiro temporßrio.
     Temos agora um socket funcional mas nƒo ligado ao servidor.
     A dimensao a indicar ao bind nao ‰ a da estrutura, pois depende
     do nome do ficheiro */

  unlink(UNIXDG_TMP);
  mktemp(clidgrmaddr.sun_path);
  clilen = sizeof(clidgrmaddr.sun_family)+strlen(clidgrmaddr.sun_path);
  if(bind(sockfd,(struct sockaddr *)&clidgrmaddr,clilen) < 0) {
    perror(ERRORMSG2);
    exit(1);
  }
  
  /* Dados para o socket datagram: tipo + nome do ficheiro
     (depois de uma limpeza preventiva!)
     Agora vai ser indicado qual o socket do lado do servidor para onde
     enviar mensagens. Note-se que nƒo ‰ estabelecida liga‡ƒo com o socket do
     lado do servidor */

  bzero((char *)&servdgrmaddr,sizeof(servdgrmaddr));
  servdgrmaddr.sun_family = AF_UNIX;
  strcpy(servdgrmaddr.sun_path,UNIXDG_PATH);
  servlen = strlen(servdgrmaddr.sun_path) + sizeof(servdgrmaddr.sun_family);

  /* Escreve no socket */ 

  if(sendto(sockfd,argv[1],strlen(argv[1]),0,(struct sockaddr *)&servdgrmaddr,servlen) < strlen(argv[1]))  {
    perror(ERRORMSG3);
    exit(1);
  } 
  
  /* Fecha liga‡ƒo.
     Nƒo esquecer de eliminar ficheiro temporßrio */

  close(sockfd);
  unlink(clidgrmaddr.sun_path);
  exit(0);
}
