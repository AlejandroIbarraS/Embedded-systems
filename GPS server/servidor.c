#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <netdb.h>

#include "defs.h"
#include "servidor.h"
#include "procesamiento.h"
#include "serial.h"

extern int pipefd[2];
extern int edo_pipe;

int sockfd;
void cliente(char *dir, int puerto){
	char leer_mensaje[TAM_BUFFER];
	iniSignals();
	iniCliente(dir,puerto);
	while(1)
	{
		if (read (sockfd, &leer_mensaje, TAM_BUFFER) < 0)
		{	
			perror ("Ocurrio algun problema al recibir datos del cliente");
			exit(1);
		}
		printf ("El servidor envio el siguiente mensaje: \n\n%s\n", leer_mensaje);
	}
	printf ("Cerrando la aplicacion cliente\n");


	close(sockfd);

}


void* servidor(void* arg){
	pid_t pid;
   	int cliente_sockfd;
   	
   	
	iniSignals();
	iniServidor();
	
	for(;EVER;)
	{
	   	printf ("En espera de peticiones de conexión ...\n");
   		if( (cliente_sockfd = accept(sockfd, NULL, NULL)) < 0 )
		{
			perror("Ocurrio algun problema al atender a un cliente");
			exit(1);
   		}
		pid = fork();
		if( !pid ){
   			atiendeCliente(cliente_sockfd);
		}
	}
}



void iniCliente(char *dir, int puerto){
	struct sockaddr_in direccion_servidor;

	memset (&direccion_servidor, 0, sizeof (direccion_servidor));
	direccion_servidor.sin_family = AF_INET;
	direccion_servidor.sin_port = htons(puerto);
	
	char ip[100];
	hostname_to_ip(dir,ip);
	
	
	if( inet_pton(AF_INET, ip, &direccion_servidor.sin_addr) <= 0 )
	{
		perror("Ocurrio un error al momento de asignar la direccion IP");
		exit(1);
	}
	
	printf("Creando Socket ....\n");
	if( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("Ocurrio un problema en la creacion del socket");
		exit(1);
	}
	
	printf ("Estableciendo conexion ...\n");
	if( connect(sockfd, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor) ) < 0) 
	{
		perror ("Ocurrio un problema al establecer la conexion");
		exit(1);
	}

}


int hostname_to_ip(char *hostname , char *ip)
{  
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_in *h;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
	hints.ai_socktype = SOCK_STREAM;

	if ( (rv = getaddrinfo( hostname , "http" , &hints , &servinfo)) != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		h = (struct sockaddr_in *) p->ai_addr;
		strcpy(ip , inet_ntoa( h->sin_addr ) );
	}
	
	freeaddrinfo(servinfo); // all done with this structure
	return 0;
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
	if( listen(sockfd, COLA_CLIENTES) < 0 )
	{
		perror("Ocurrio un problema al crear la cola de aceptar peticiones de los clientes");
		exit(1);
   	}
}

void atiendeCliente( int cliente_sockfd){
	
	char msg[TAM_BUFFER];
	
	printf("Se acepto un cliente, atendiendolo \n");
	register int cont = 0;
	while(1)
	{
		read( pipefd[0], msg, sizeof(char)*TAM_BUFFER);
		if( write (cliente_sockfd, msg, TAM_BUFFER) < 0 )
		{
			perror("Ocurrio un problema en el envio de un mensaje al cliente");
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
		printf("Concluimos la ejecución de la aplicacion Servidor \n");
		close (sockfd); 
		exit(0);
	}
}
