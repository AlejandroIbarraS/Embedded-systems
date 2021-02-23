#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"

void guardarArchivo(char * datos, char * nombre)
{
	FILE *ap_arch;

	ap_arch=fopen(nombre,"w");
	if(!ap_arch)
	{
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}

	fwrite(datos, sizeof(char), strlen(datos), ap_arch);

	fclose(ap_arch);
}

void leerArchivo(char * datos, char * nombre){
	FILE *ap_arch;
	char linea[100];
	ap_arch=fopen(nombre,"r");
	if(!ap_arch)
	{
		perror("Error al abrir el archivo");
		exit(EXIT_FAILURE);
	}

	while(fgets(linea,1024,ap_arch)){
		strcat(datos,linea);
	}
	fclose(ap_arch);
}
