#include <stdio.h>
#define MAXLINE 512

/* Cliente do tipo socket datagram.
   Lê string de fp e envia para sockfd.
   Lê string de sockfd e envia para stdout */

#include <sys/types.h>
#include <sys/socket.h>

dg_cli(fp, sockfd, pserv_addr, servlen)
FILE *fp;
int sockfd;
struct sockaddr *pserv_addr;
int servlen;
{
	int n;
	char sendline[MAXLINE], recvline[MAXLINE+1];
	struct sockaddr x;
	int xx = servlen;

	while (fgets(sendline, MAXLINE, fp) != NULL) {
		n = strlen(sendline);

		/* Envia string para sockfd. Note-se que o \0 não 
               é enviado */

		if (sendto(sockfd, sendline, n, 0, pserv_addr, servlen) != n)
			err_dump("dg_cli: sendto error on socket");

		/* Tenta ler string de sockfd. Note-se que tem de 
		   terminar a string com \0 */

		n = recvfrom(sockfd, recvline, MAXLINE, 0,
			     (struct sockaddr *)&x, &xx);
		if (n < 0)
			err_dump("dg_cli: recvfrom error");
		recvline[n] = 0;

		/* Envia a string para stdout */

		fputs(recvline, stdout);
	}
	if (ferror(fp))
		err_dump("dg_cli: error reading file");
}
