#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <syslog.h>


#include <netdb.h>

#include "defs.h"
#include "servidor.h"

extern int pipefd[2];
extern int edo_pipe;

int sockfd;

void* servidor(void* arg){
	pid_t pid;
   	int cliente_sockfd;
   	
   	
	iniSignals();
	iniServidor();
	
	for(;EVER;)
	{
	   	printf ("En espera de peticiones de conexión ...\n");
		syslog(LOG_INFO, "En espera de peticiones de conexión ...\n");
   		if( (cliente_sockfd = accept(sockfd, NULL, NULL)) < 0 )
		{
			perror("Ocurrio algun problema al atender a un cliente");
			syslog(LOG_INFO, "Ocurrio algun problema al atender a un cliente");
			exit(1);
   		}
		pid = fork();
		if( !pid ){
   			atiendeCliente(cliente_sockfd);
		}
	}
}



void iniSignals(){
	
	if (signal( SIGCHLD, manejador) == SIG_ERR ){
		perror("Error en el manejador");
		exit(EXIT_FAILURE);
	}
	if (signal( SIGINT, manejador) == SIG_ERR ){
		perror("Error en el manejador");
		exit(EXIT_FAILURE);
	}
}

void iniServidor(){
	struct sockaddr_in direccion_servidor; 
	
   	memset (&direccion_servidor, 0, sizeof (direccion_servidor));
   	direccion_servidor.sin_family 		= AF_INET;
   	direccion_servidor.sin_port 		= htons(PUERTO);
   	direccion_servidor.sin_addr.s_addr 	= INADDR_ANY;

   	printf("Creando Socket ....\n");
   	if( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("Ocurrio un problema en la creacion del socket");
		exit(1);
   	}
	   
   	printf("Configurando socket ...\n");
   	if( bind(sockfd, (struct sockaddr *) &direccion_servidor, sizeof(direccion_servidor)) < 0 )
	{
		perror ("Ocurrio un problema al configurar el socket");
		exit(1);
   	}
	   
   	printf ("Estableciendo la aceptacion de clientes...\n");
	syslog(LOG_INFO, "Estableciendo la aceptacion de clientes...\n");
	if( listen(sockfd, COLA_CLIENTES) < 0 )
	{
		perror("Ocurrio un problema al crear la cola de aceptar peticiones de los clientes");
		exit(1);
   	}
}

void atiendeCliente( int cliente_sockfd){
	
	char msg[TAM_BUFFER];
	
	printf("Se acepto un cliente, atendiendolo \n");
	syslog(LOG_INFO, "Se acepto un cliente, atendiendolo \n");
	register int cont = 0;
	while(1)
	{
		read( pipefd[0], msg, sizeof(char)*TAM_BUFFER);
		if( write (cliente_sockfd, msg, TAM_BUFFER) < 0 )
		{
			perror("Ocurrio un problema en el envio de un mensaje al cliente");
			syslog(LOG_INFO, "Ocurrio un problema en el envio de un mensaje al cliente");
			exit(EXIT_FAILURE);
		}
		//printf("%s", msg);
		cont++;
		
		sleep(1);
	}
	
	
	close (cliente_sockfd);
	exit(0);
}

void manejador(int sig){
	int estado;
	pid_t pid;
	if(sig == SIGCHLD){
		printf("Se recibio la señal de SIGCHLD\n ");
		pid = wait(&estado);
		printf("Termino el proceso %d con estado: %d\n",pid,estado>>8);
	}else if(sig == SIGINT){
		printf("Se recibio la señal de SIGINT \n ");
		syslog(LOG_INFO, "Se recibio la señal de SIGTERM \n");
		printf("Concluimos la ejecución de la aplicacion Servidor \n");
		syslog(LOG_INFO, "Concluimos la ejecución de la aplicacion Servidor \n");
		close (sockfd); 
		exit(0);
	}else if(sig == SIGKILL){
		printf("Se recibio la señal de SIGKILL \n ");
		syslog(LOG_INFO, "Se recibio la señal de SIGKILL \n");
		printf("Concluimos la ejecución de la aplicacion Servidor \n");
		syslog(LOG_INFO, "Concluimos la ejecución de la aplicacion Servidor \n");
		close (sockfd); 
		exit(0);
	}
}


void iniDemonio()
{
    FILE *apArch;

    pid_t pid = 0;
    pid_t sid = 0;
    pid = fork();
    if( pid == -1 )
    {
		perror("Error al crear el primer proceso hijo\n");
		exit(EXIT_FAILURE);
    }

    if( pid )
    {
		printf("Se termina proceso padre, PID del proceso hijo %d \n", pid);
		exit(0);
    }
    umask(0);
    sid = setsid();
    if( sid < 0 )
    {
		perror("Error al iniciar sesion");
		exit(EXIT_FAILURE);
    }
    pid = fork( );
    if( pid == -1 )
    {
		perror("Error al crear el segundo proceso hijo\n");
		exit(EXIT_FAILURE);
    }
    if( pid )
    {
		printf("PID del segundo proceso hijo %d \n", pid);
		apArch = fopen("/home/pi/serverrtcc.pid", "w");
		fprintf(apArch, "%d", pid);
		fclose(apArch);

		exit(0);
    }
    chdir("/");
    close( STDIN_FILENO  );
    close( STDOUT_FILENO );
    close( STDERR_FILENO );
}
