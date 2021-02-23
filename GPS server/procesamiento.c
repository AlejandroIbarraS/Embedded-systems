#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "defs.h"

extern int pipefd[2];
extern int edo_pipe;

void evaluar_linea(char * linea){
	char * app;
	char camps[5][MAXRES];
	char msg[TAM_BUFFER];
	register int cont=0;
	app=strstr(linea,"GPGGA");
	if(app != NULL){
		//printf("\n\nEncontre una GPGGA ---------------------------- %s\n\n\n",linea);
		app = strtok(app,",");
		while( (app = strtok(NULL,","))!=NULL  && cont<5 ){
			strcpy(camps[cont],app);
			cont++;
		} 
		sprintf(msg,"1. Tiempo:   %c%c:%c%c:%c%c\n2. Latitud:  %s\n3. N o S:    %s\n4. Longitud: %s\n5. E o W:    %s\n",camps[0][0],camps[0][1],camps[0][2],camps[0][3],camps[0][4],camps[0][5],camps[1],camps[2],camps[3],camps[4]);
		
		write( pipefd[1], msg, sizeof(char)*TAM_BUFFER);
		
	}
}
