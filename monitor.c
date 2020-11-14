#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// #include <stdio.h>
// #define MAXLINE 512

// /* Cliente do tipo socket stream.
//    L▒ string de fp e envia para sockfd.
//    L▒ string de sockfd e envia para stdout */

// str_cli(fp, sockfd)
// FILE *fp;
// int sockfd;
// {
//         int n;
//         char sendline[MAXLINE], recvline[MAXLINE+1];

//         while (fgets(sendline, MAXLINE, fp) != NULL) {



// /* ########################################


// aqui ▒ o vosso trabalho

// ##########################################*/

//             /* Envia string para sockfd. Note-se que o \0 n▒o
//                ▒ enviado */

//                 n = strlen(sendline);
//                 if (writen(sockfd, sendline, n) != n)
//                         err_dump("str_cli: writen error on socket");

//                 /* Tenta ler string de sockfd. Note-se que tem de
//                    terminar a string com \0 */

//                 n = readline(sockfd, recvline, MAXLINE);
//                 if (n<0)
//                         err_dump("str_cli:readline error");
//                 recvline[n] = 0;

//                 /* Envia a string para stdout */

//                 fputs(recvline, stdout);
//         }
//         if (ferror(fp))
//                 err_dump("str_cli: error reading file");
// }

// #define MAXLINE 512

// /* Servidor do tipo socket stream.
//    Manda linhas recebidas de volta para o cliente */

// str_echo(sockfd)
// int sockfd;
// {
//         int n, i;
//         char line[MAXLINE];

//         for (;;) {

//                 /* L▒ uma linha do socket */

//                 n = readline(sockfd, line, MAXLINE);
//                 if (n == 0)
//                         return;
//                 else if (n < 0)
//                         err_dump("str_echo: readline error");

//                 int index;
//                 for (index=0;index<n-1;index++){
//                         line[index]=line[index]++;
//                 }

//                 /* Manda linha de volta para o socket. n conta com
//                    o \0 da string, caso contr▒rio perdia-se sempre
//                    um caracter! */
//                 if (writen(sockfd, line, n) != n)
//                         err_dump("str_echo: writen error");
//         }
// }



/* Fun▒^Ues utilit▒rias retiradas de "UNIX Networking Programming" */

/* L▒ nbytes de um ficheiro/socket.
   Bloqueia at▒ conseguir ler os nbytes ou dar erro */
// #include "unix.h"

// int readn(fd, ptr, nbytes)
// int fd;
// char *ptr;
// int nbytes;
// {
//         int nleft, nread;

//         nleft = nbytes;
//         while (nleft > 0) {
//                 nread = read (fd, ptr, nleft);
//                 if (nread < 0)
//                         return (nread);
//                 else if (nread == 0)
//                         break;

//                 nleft -= nread;
//                 ptr += nread;
//         }
//         return (nbytes - nleft);
// }

// /* Escreve nbytes num ficheiro/socket.
//    Bloqueia at▒ conseguir escrever os nbytes ou dar erro */

// int writen(fd, ptr, nbytes)
// int fd;
// char *ptr;
// int nbytes;
// {
//         int nleft, nwritten;

//         nleft = nbytes;
//         while (nleft > 0) {
//                 nwritten = write(fd, ptr, nleft);
//                 if (nwritten <= 0)
//                         return(nwritten);

//                 nleft -= nwritten;
//                 ptr += nwritten;
//         }
//         return(nbytes - nleft);
// }

// /* L▒ uma linha de um ficheiro/socket (at▒ \n, maxlen ou \0).
//    Bloqueia at▒ ler a linha ou dar erro.
//    Retorna quantos caracteres conseguiu ler */

// int readline(fd, ptr, maxlen)
// int fd;
// char *ptr;
// int maxlen;
// {
//         int n, rc;
//         char c;

//         for (n=1; n<maxlen; n++) {
//                 if ((rc = read(fd, &c, 1)) == 1) {
//                         *ptr++ = c;
//                         if (c == '\n')
//                                 break;
//                 } else if (rc == 0) {
//                         if (n == 1)
//                                 return(0);
//                         else
//                                 break;
//                 } else
//                         return (-1);
//         }

//         /* N▒o esquecer de terminar a string */

//         *ptr = 0;

//         /* Note-se que n foi incrementado de modo a contar
//            com o \n ou \0 */

//         return (n);
// }

// #include <stdio.h>
// #include <errno.h>

// /* Mensagem de erro */

// err_dump(msg)
// char *msg;
// {
//         perror(msg);
//         exit(1);
// }

// /* Limpa array de ponteiros */

// bzero(p, n)
// char *p;
// int n;
// {
//         for (; n>0; n--) {
//                 *p++ = 0;
//         }
// }

// /* Copia array de ponteiros */

// bcopy(p, q, n)
// char *p, *q;
// int n;
// {
//         for (; n>0; n--) {
//                 *q++ = *p++;
//         }
// }



int main(int argc, char *argv[])
{
    FILE *registoFile;
    registoFile=fopen("Relatorio.txt","w");
	char *mensagem="Estado atual => Simulacao a decorrer!\nUtilizadores: 203\nCasos positivos: 18\nCasos em estudo: 11\nTempo medio de espera: 13 min.";
	fprintf(registoFile,"%s",mensagem);
    fclose(registoFile);

	return 0;
}