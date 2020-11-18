#include <stdio.h>
#include <string.h>

#define READ  0
#define WRITE 1

#define WRITER

main()
{
  int fd[2];
  char buffer[] = "Hello world.\n";

  pipe(fd);

#ifdef WRITER
  close(fd[READ]);
  write(fd[WRITE], buffer, strlen(buffer));
#endif

#ifdef READER
  if (fork() == 0) {
    /* Processo filho */
     printf ("Processo Filho\n");
    read(fd[READ], buffer, strlen(buffer));
  }
#endif
}

