
/* Servidor do tipo socket datagram.
   Recebe linhas do cliente e devolve-as para o cliente */

#include "unix.h"

main (void)
{
	int sockfd, servlen;
	struct sockaddr_un serv_addr, cli_addr;

	/* Cria socket datagram */

	if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
		err_dump("server: can't open datagram socket");

	/* O servidor ‰ quem cria o ficheiro que identifica o socket.
         Elimina o ficheiro, para o caso de algo ter ficado pendurado */

	unlink(UNIXDG_PATH);

	/* Primeiro uma limpeza preventiva!
	   Dados para o socket datagram: tipo + nome do ficheiro.
         O ficheiro serve para que os clientes possam identificar o servidor */

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, UNIXDG_PATH);
	servlen = sizeof(serv_addr.sun_family) + strlen(serv_addr.sun_path);

	/* Associa o socket ao ficheiro */

	if (bind(sockfd, (struct sockaddr *) &serv_addr, servlen) < 0)
		err_dump("server: can't bind local address");

	/* Fica € espera de mensagens do cliente. As mensagens recebidas
	   sƒo reenviadas para o cliente */

	dg_echo(sockfd, (struct sockaddr *) &cli_addr, sizeof(cli_addr));
}
