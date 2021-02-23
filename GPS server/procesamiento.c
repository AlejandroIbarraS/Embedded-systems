#include <stdio.h>
#include <string.h>
#include "defs.h"

void extraccionCampos(char * datos, char resultado[5][tamResultados])
{
	char * ptr;
	register int cont=0;
	ptr=strstr(datos,"GPGGA");
	ptr = strtok(ptr,",\n\t");
	while((ptr = strtok(NULL,",\n\t"))!=NULL  && cont<5){
		strcpy(resultado[cont],ptr);
		cont++;
	}
}

