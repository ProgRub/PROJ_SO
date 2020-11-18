#define MAXLINE 512

/* Servidor do tipo socket datagram.
   Manda linhas recebidas de volta para o cliente */

#include <sys/types.h>
#include <sys/socket.h>

#define MAXMESG 2048

/* pcli_addr especifica o cliente */

dg_echo(sockfd, pcli_addr, maxclilen)
int sockfd;
struct sockaddr *pcli_addr;
int maxclilen;
{
	int n, clilen;
	char mesg[MAXMESG];

	for (;;) {
		clilen = maxclilen;

		/* Lê uma linha do socket */

		n = recvfrom(sockfd, mesg, MAXMESG, 0, pcli_addr, &clilen);
		if (n < 0)
			err_dump("dg_echo: recvfrom error");

		/* Manda linha de volta para o socket */

		if (sendto(sockfd, mesg, n, 0, pcli_addr, clilen) != n)
			err_dump("dg_echo: sendto error");
	}
}
