
/* Servidor do tipo socket tcp (stream).
   Recebe linhas do cliente e reenvia-as para o cliente */

#include "inet.h"

main(void)
{
	int sockfd, newsockfd, clilen, childpid;
	struct sockaddr_in cli_addr, serv_addr;

	/* Cria socket stream */

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_dump("server: can't open stream socket");

	/* Primeiro uma limpeza preventiva!
	   Dados para o socket stream: tipo + qualquer endereço.
	   Note-se que o servidor aceita qualquer endereço de cliente */

	bzero((char*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(SERV_TCP_PORT);

	/* Associa o socket (a qualquer endereço de cliente) */

	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		err_dump("server: can't bind local address");

	/* Servidor pronto a aceitar 5 clientes para o socket tcp (stream) */

	listen(sockfd, 5);

	for ( ; ; ) {

		/* Não esquecer que quando o servidor aceita um cliente cria um
		   socket para comunicar com ele. O primeiro socket (sockfd) fica 
		   à espera de mais clientes */

		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr,
				   &clilen);
		if (newsockfd < 0)
			err_dump("server: accept error");

		/* Lança processo filho para lidar com o cliente */

		else if ((childpid = fork()) < 0)
			err_dump("server: fork error");

		else if (childpid == 0) {

			/* Processo filho que vai atender o cliente. 
			   Fechar sockfd é sanitário, já que não é
			   utilizado pelo processo filho.
			   Os dados recebidos do cliente são reenviados 
			   para o cliente */

			close (sockfd);
			str_echo(newsockfd);
			exit(0);
		}

		/* Processo pai.
		   Fechar newsockfd é sanitário, já que não é
		   utilizado pelo processo pai */

		close (newsockfd);
	}
}
