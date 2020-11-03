#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

main(int argc, char *argv[])
{
    //verificar se argc == 2
    FILE *configFile;
    configFile=fopen(argv[1],"r");
    //lidar com os parâmetros
    fclose(configFile);
    return 0;
}