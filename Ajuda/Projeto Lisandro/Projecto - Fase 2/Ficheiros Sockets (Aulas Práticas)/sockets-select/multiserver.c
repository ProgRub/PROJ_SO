
/* Servidor que utiliza sockets stream e datagram em simult‚neo.
   O servidor recebe caracteres e envia-os para stdout */

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define MAXLINE 80

#define MAXSOCKS 32

#define ERRORMSG1 "server: cannot open stream socket"
#define ERRORMSG2 "server: cannot bind stream socket"
#define ERRORMSG3 "server: cannot open datagram socket"
#define ERRORMSG4 "server: cannot bind datagram socket"

#include "names.h"

/* LŠ caracteres do socket e envia para stdout
   (Para obter uma string ‰ necessßrio terminar com \0) */

void echo(int sockfd)
{
  char buffer[MAXLINE];
  int n;
  n = read(sockfd,buffer,MAXLINE);
  buffer[n] = 0;
  puts(buffer);
}

int main(void)
{
  int strmfd,dgrmfd,newfd;
  struct sockaddr_un servstrmaddr,servdgrmaddr,clientaddr;
  int len,clientlen;
  fd_set testmask,mask;

  /* Cria socket stream */

  if((strmfd = socket(AF_UNIX,SOCK_STREAM,0)) < 0) {
    perror(ERRORMSG1);
    exit(1);
  }

  /* Dados para o socket stream: tipo + nome do ficheiro 
     (depois de uma limpeza preventiva!) */

  bzero((char *)&servstrmaddr,sizeof(servstrmaddr));
  servstrmaddr.sun_family = AF_UNIX;
  strcpy(servstrmaddr.sun_path,UNIXSTR_PATH);

  /* O servidor ‰ quem cria o ficheiro que identifica o socket.
     Elimina o ficheiro, para o caso de algo ter ficado pendurado.
     Em seguida associa o socket ao ficheiro. 
     A dimensƒo a indicar ao bind nƒo ‰ a da estrutura, pois depende
     do nome do ficheiro */

  len = sizeof(servstrmaddr.sun_family)+strlen(servstrmaddr.sun_path);
  unlink(UNIXSTR_PATH);
  if(bind(strmfd,(struct sockaddr *)&servstrmaddr,len) < 0) {
    perror(ERRORMSG2);
    exit(1);
  }

  /* Servidor pronto a aceitar 5 clientes para o socket stream */

  listen(strmfd,5);

  /* Cria socket datagram */

  if((dgrmfd = socket(AF_UNIX,SOCK_DGRAM,0)) < 0) {
    perror(ERRORMSG3);
    exit(1);
  }  

  /* Inicializa socket datagram: tipo + nome do ficheiro 
     (depois de uma limpeza preventiva!) */

  bzero((char *)&servdgrmaddr,sizeof(servdgrmaddr));
  servdgrmaddr.sun_family = AF_UNIX;
  strcpy(servdgrmaddr.sun_path,UNIXDG_PATH);

  /* O servidor ‰ quem cria o ficheiro que identifica o socket.
     Elimina o ficheiro, para o caso de algo ter ficado pendurado.
     Em seguida associa o socket ao ficheiro. 
     A dimensƒo a indicar ao bind nƒo ‰ a da estrutura, pois depende
     do nome do ficheiro */

  len = sizeof(servdgrmaddr.sun_family)+strlen(servdgrmaddr.sun_path);
  unlink(UNIXDG_PATH);
  if(bind(dgrmfd,(struct sockaddr *)&servdgrmaddr,len) < 0) {
    perror(ERRORMSG4);
    exit(1);
  }

  /* Pronto. Nƒo existe listen para sockets datagram */

  /* Primeiro limpa-se tudo.
     Em seguida, mascaram-se os 2 sockets stream e datagram.
     A mascara ‰ limpa pelo sistema de cada vez que algo ‰ recebido 
     no socket. Por isso se utiliza uma mascara auxiliar */
  
  FD_ZERO(&testmask);
  FD_SET(strmfd,&testmask);
  FD_SET(dgrmfd,&testmask);

  for(;;) {
    mask = testmask;

    /* Bloqueia servidor at‰ que aconte‡a algo. Nƒo necessariamente 
       chegarem dados aos sockets */

    select(MAXSOCKS,&mask,0,0,0);

    /* Aconteceu algo. Verificar se chegaram clientes para o socket stream */

    if(FD_ISSET(strmfd,&mask)) {
      
      /* Aceitar o cliente e associa-lo a newfd. Ler dados do cliente e 
         matar o cliente. Este cdigo nƒo ‰ muito bom, porque fica bloqueado
         € espera de dados do cliente */
      clientlen = sizeof(struct sockaddr_un);
      newfd = accept(strmfd,(struct sockaddr *)&clientaddr,&clientlen);
      echo(newfd);
      close(newfd);
    }

    /* Verificar se chegaram dados ao socket datagram. Ler dados */

    if(FD_ISSET(dgrmfd,&mask)) 
      echo(dgrmfd);

    /* Pronto. Voltar ao ciclo mas nƒo esquecer da mascara! */
  }    
  exit(0);
}
