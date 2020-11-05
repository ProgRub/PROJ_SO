#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/*
int main()
{
	/
    //int fileDialog = open("test", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    //write(fileDialog, "Hello, world !\n", 15);
    //close(fileDialog);
    //return 0;
	
	int a = 2;
	FILE *fp;
	char str[] = "This is tutorialspoint.com" + a;
	fp = fopen( "file.txt" , "w" );
	//sfp = fopen("test", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	fwrite(str , 1 , sizeof(str) , fp );

	fclose(fp);
  
	return(0);
}*/
main(int argc, char *argv[])
{
    FILE *registoFile;
    registoFile=fopen(argv[1],"w");
    //escrever mensagem
    fclose(registoFile);
	// int num;
	// FILE *f;
	// int a = 2;
	// f = fopen("test.txt","w");

	// if(f == NULL)
	// {
	// 	perror("Error!");   
	// 	exit(1);             
	// }

	// printf("Numero: " );
	// scanf("%d",&num);
	
	// //fwrite(&num, sizeof(), 1, f);
	
	// fprintf(f,"%d", a);
	// fprintf(f,"%d",num + a);
	// fclose(f);

	return 0;
}