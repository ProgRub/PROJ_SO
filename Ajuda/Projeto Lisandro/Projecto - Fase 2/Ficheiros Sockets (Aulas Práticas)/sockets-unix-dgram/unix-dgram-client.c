
/* Cliente do tipo socket datagram.
   L� linhas de stdin, envia para o servidor, recebe-as de novo
   do servidor e envia-as para stdout */

#include "unix.h"

main(void)
{
	int sockfd, clilen, servlen;
	char *mktemp();
	struct sockaddr_un cli_addr, serv_addr;

	/* Primeiro uma limpeza preventiva!
	   Dados para o socket datagram: tipo + nome do ficheiro.
         O ficheiro identifica o servidor */

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, UNIXDG_PATH);
	servlen = sizeof(serv_addr.sun_family) + strlen(serv_addr.sun_path);

	/* Cria socket datagram */

	if((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
		err_dump("client: can't open datagram socket");

	/* Primeiro uma limpeza preventiva!
	   Dados para o socket datagram: tipo + nome do ficheiro tempor�rio.
         O ficheiro tempor�rio serve para ter um socket funcional mas
	   n�o ligado ao servidor */

	bzero((char *) &cli_addr, sizeof(cli_addr));
	cli_addr.sun_family = AF_UNIX;
	strcpy(cli_addr.sun_path, UNIXDG_TMP);

	/* Elimina o ficheiro, para o caso de algo ter ficado pendurado */

	unlink(UNIXDG_TMP);

	/* Associa o socket ao ficheiro tempor�rio */

	mktemp(cli_addr.sun_path);
	clilen = sizeof(cli_addr.sun_family) + strlen(cli_addr.sun_path);
	if (bind(sockfd, (struct sockaddr *) &cli_addr, clilen) < 0)
		err_dump("client: can't bind local address");

	/* L� linha do stdin e envia para o servidor. Recebe a linha do 
	   servido e envia-a para stdout */

	dg_cli(stdin, sockfd, (struct sockaddr *) &serv_addr, servlen);

	/* Fecha o socket e elimina ficheiro tempor�rio */

	close(sockfd);
	unlink(cli_addr.sun_path);
	exit(0);
}
