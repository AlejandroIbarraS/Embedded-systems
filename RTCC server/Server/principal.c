#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "servidor.h"
#include "lectura.h"
#include <syslog.h>

int pipefd[2];
int edo_pipe;

int main(){
	iniDemonio();
	syslog(LOG_INFO, "Servidor iniciado\n");
	pthread_t tid,tid2;
	int nh=0,nh2=1;
	edo_pipe = pipe( &pipefd[0] );
	if( edo_pipe == -1 )
	{
		perror("Error al crear la tuberia...\n");
		syslog(LOG_INFO, "Error al crear la tuberia...\n");
		exit(EXIT_FAILURE);
	}
	
	pthread_create(&tid,NULL,lectura,(void *)&nh);
	pthread_create(&tid2,NULL,servidor,(void *)&nh2);
	pthread_join(tid,NULL);
	pthread_join(tid2,NULL);
	 
		

	return 0;
}
