
/* Servidor do tipo socket udp (datagram).
   Recebe linhas do cliente e devolve-as para o cliente */

#include "inet.h"

main(void)
{
	int sockfd;
	struct sockaddr_in serv_addr, cli_addr;

	/* Cria socket udp (datagram) */

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		err_dump("server: can't open datagram socket");

	/* Primeiro uma limpeza preventiva!
	   Dados para o socket udp (datagram): tipo + qualquer cliente */

	bzero((char*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERV_UDP_PORT);

	/* Associa o socket a qualquer cliente */

	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		err_dump("server: can't bind local address");

	/* Fica à espera de mensagens do cliente. As mensagens recebidas
	   são reenviadas para o cliente */

	dg_echo(sockfd, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
}
