#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

main()
{
    int fileDialog = open("simulador", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    write(fd, "Hello, world !\n", 15);
    close(fd);
    return 0;
)
}