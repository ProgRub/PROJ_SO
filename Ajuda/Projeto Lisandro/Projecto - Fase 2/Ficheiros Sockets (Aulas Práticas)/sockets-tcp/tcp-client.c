
/* Cliente do tipo socket tcp (stream).
   Lê linhas do teclado e envia-as para o servidor */

#include "inet.h"

main(void)
{
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *hp;

	/* Primeiro uma limpeza preventiva!
	   Dados para o socket stream: tipo */

	bzero((char*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;

	/* Obter endereço do servidor a partir do seu nome */

	hp = gethostbyname(SERV_HOSTNAME);

	/* Dados para o socket stream: informação sobre o servidor */

	bcopy (hp->h_addr_list[0], (char*)&serv_addr.sin_addr.s_addr, 
	       hp->h_length);
	serv_addr.sin_port = htons(SERV_TCP_PORT);

	/* Cria socket tcp (stream) */

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_dump("client: can't open datagram socket");

	/* Estabelece ligação com o servidor */

	if (connect(sockfd, (struct sockaddr*) &serv_addr, 
		    sizeof(serv_addr)) < 0)
		err_dump("client:can't connect to server");

	/* Envia as linhas lidas do teclado para o socket */

	str_cli(stdin, sockfd);

	/* Fecha o socket e termina */

	close (sockfd);
	exit(0);
}

