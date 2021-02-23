#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "servidor.h"
#include "serial.h"

int pipefd[2];
int edo_pipe;

int main(){
	
	int sc;
	pthread_t tid,tid2;
	int nh=0,nh2=1;
	char dir[15];
	int iden=0;
	int puerto;
	edo_pipe = pipe( &pipefd[0] );
	if( edo_pipe == -1 )
	{
		perror("Error al crear la tuberia...\n");
		exit(EXIT_FAILURE);
	}
	
	printf("Seleccione la opcion de su preferencia\n    1. Ejecutar Servidor\n    2. Ejecutar Cliente\n    ");
    scanf("%d", &sc);
	
	if(sc==1){
		pthread_create(&tid,NULL,serial,(void *)&nh);
		pthread_create(&tid2,NULL,servidor,(void *)&nh2);
		pthread_join(tid,NULL);
		pthread_join(tid2,NULL);
		
	}
	else if (sc==2){
		printf("Ingresa el identificador del tunel: \t");
		scanf("%d",&iden);
		sprintf(dir,"%d.tcp.ngrok.io",iden);
		printf("Ingresa el puerto \t\t\t");
		scanf("%d", &puerto);
		cliente(dir,puerto);
	}
		 
		

	return 0;
}
