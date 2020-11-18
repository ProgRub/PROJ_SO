
/* Cliente do tipo socket udp (datagram).
   Lê linhas de stdin, envia para o servidor, recebe-as de novo
   do servidor e envia-as para stdout */

#include "inet.h"

main(void)
{
	int sockfd;
	struct sockaddr_in cli_addr, serv_addr;
	struct hostent *hp;

	/* Primeiro uma limpeza preventiva!
	   Dados para o socket udp (datagram): tipo */

	bzero((char*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;

	/* Obter endereço do servidor a partir do seu nome */

	hp = gethostbyname(SERV_HOSTNAME);

	/* Dados para o socket udp (datagram): informação sobre o servidor */

	bcopy (hp->h_addr_list[0], (char*)&serv_addr.sin_addr.s_addr, 
	       hp->h_length);
	serv_addr.sin_port = htons(SERV_UDP_PORT);

	/* Cria socket udp (datagram) */

        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
                err_dump("client: can't open datagram socket");

	/* Primeiro uma limpeza preventiva!
	   Associa o socket a qualquer endereço. Esta associação serve
	   para ter um socket funcional mas não ligado a um servidor */

	bzero((char*)&cli_addr, sizeof(cli_addr));
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	cli_addr.sin_port = htons(0);
	if (bind(sockfd, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) < 0)
		err_dump("client: can't bind local address");

	/* Lê linha do stdin e envia para o servidor. Recebe a linha do 
	   servido e envia-a para stdout */

	dg_cli(stdin, sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	/* Fecha o socket */

	close(sockfd);
	exit(0);
}
