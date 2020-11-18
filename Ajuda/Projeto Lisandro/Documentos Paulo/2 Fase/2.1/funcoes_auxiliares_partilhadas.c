#include "unix.h"



//Converte Valor Int em Array Char Para Ficar Com o Formato [015] em Vez de [15]
char *Converte_Em_Numero_De_Tres_Digitos(int Valor){
	static char String [3];
	if (Valor>99)
	{
		 sprintf(String, "%d", Valor);
	}
	else if (Valor>9)
	{
		sprintf(String, " %d", Valor);
	}
	else	
	{
		sprintf(String, "  %d", Valor); 
	}
	return String;
}







//Funcao Que Converte "Tempo de Execucao" em "Tempo Real"
char * Converte_Tempo_Para_Horas(int Tempo)
{
		static char String[8];
		int Horas,Minutos,Segundos,Aux;
		
		Horas=Tempo/3600;
		Aux=Tempo%3600;
		Minutos=Aux/60;
		Segundos=Aux%60;	

/*
		if(min>=60)
		{
			hr=min/60;
			min=min%60;
		}
*/
		
		if(Tempo==0)
		{
			Horas=0;
			Minutos=0;
			sprintf(String,"%d:%d:%d",Horas,Minutos,Segundos);	
		}
		else if(Tempo!=0)
		{
			sprintf(String,"%d:%d:%d",Horas,Minutos,Segundos);	
		}
			return String;
}

