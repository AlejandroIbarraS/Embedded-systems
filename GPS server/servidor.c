#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "defs.h"
#include "servidor.h"
#include "archivos.h"
#include "procesamiento.h"

int sockfd;

void cliente(){
	char leer_mensaje[TAM_BUFFER];
	iniCliente();

	if (read (sockfd, &leer_mensaje, TAM_BUFFER) < 0)
	{	
		perror ("Ocurrio algun problema al recibir datos del cliente");
		exit(1);
	}
	
	printf ("El servidor dice: \n%s\n", leer_mensaje);


	close(sockfd);

}


void servidor(){
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



void iniCliente(){
	struct sockaddr_in direccion_servidor;
	/*	
	 *	AF_INET - Protocolo de internet IPV4
	 *  htons - El ordenamiento de bytes en la red es siempre big-endian, por lo que
	 *  en arquitecturas little-endian se deben revertir los bytes
	 */	
	memset (&direccion_servidor, 0, sizeof (direccion_servidor));
	direccion_servidor.sin_family = AF_INET;
	direccion_servidor.sin_port = htons(PUERTO);
	/*	
	 *	inet_pton - Convierte direcciones de texto IPv4 en forma binaria
	 */	
	if( inet_pton(AF_INET, DIR_IP, &direccion_servidor.sin_addr) <= 0 )
	{
		perror("Ocurrio un error al momento de asignar la direccion IP");
		exit(1);
	}
	/*
	 *	Creacion de las estructuras necesarias para el manejo de un socket
	 *  SOCK_STREAM - Protocolo orientado a conexión
	 */
	printf("Creando Socket ....\n");
	if( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("Ocurrio un problema en la creacion del socket");
		exit(1);
	}
	/*
	 *	Inicia el establecimiento de una conexion mediante una apertura
	 *	activa con el servidor
	 *  connect - ES BLOQUEANTE
	 */
	printf ("Estableciendo conexion ...\n");
	if( connect(sockfd, (struct sockaddr *)&direccion_servidor, sizeof(direccion_servidor) ) < 0) 
	{
		perror ("Ocurrio un problema al establecer la conexion");
		exit(1);
	}

}


void iniSignals(){
	//if (signal( SIGUSR1, manejador) == SIG_ERR ){
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
	struct sockaddr_in direccion_servidor; //Estructura de la familia AF_INET, que almacena direccion
	
	/*
 	*	AF_INET - Protocolo de internet IPV4
 	*  htons - El ordenamiento de bytes en la red es siempre big-endian, por lo que
 	*  en arquitecturas little-endian se deben revertir los bytes
 	*  INADDR_ANY - Se elige cualquier interfaz de entrada
 	*/
   	memset (&direccion_servidor, 0, sizeof (direccion_servidor));	//se limpia la estructura con ceros
   	direccion_servidor.sin_family 		= AF_INET;
   	direccion_servidor.sin_port 		= htons(PUERTO);
   	direccion_servidor.sin_addr.s_addr 	= INADDR_ANY;


	/*
 	*	Creacion de las estructuras necesarias para el manejo de un socket
 	*  SOCK_STREAM - Protocolo orientado a conexión
 		*/
   	printf("Creando Socket ....\n");
   	if( (sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("Ocurrio un problema en la creacion del socket");
		exit(1);
   	}
	/*
 	*  bind - Se utiliza para unir un socket con una dirección de red determinada
 	*/
   	printf("Configurando socket ...\n");
   	if( bind(sockfd, (struct sockaddr *) &direccion_servidor, sizeof(direccion_servidor)) < 0 )
	{
		perror ("Ocurrio un problema al configurar el socket");
		exit(1);
   	}
	/*
 	*  listen - Marca al socket indicado por sockfd como un socket pasivo, esto es, como un socket
 	*  que será usado para aceptar solicitudes de conexiones de entrada usando "accept"
 	*  Habilita una cola asociada la socket para alojar peticiones de conector procedentes
 	*  de los procesos clientes
 	*/
   	printf ("Estableciendo la aceptacion de clientes...\n");
	if( listen(sockfd, COLA_CLIENTES) < 0 )
	{
		perror("Ocurrio un problema al crear la cola de aceptar peticiones de los clientes");
		exit(1);
   	}
}

void atiendeCliente( int cliente_sockfd ){
	//char leer_mensaje[TAM_BUFFER];
	
	char datos[10000];
	char resultado[5][tamResultados];
	leerArchivo(datos,"nmea.txt");
	
	extraccionCampos(datos,resultado);
	sprintf(datos,"Tiempo: %s\nLatitud: %s\nN o S: %s\nLongitud: %s\nE o W: %s\n",resultado[0],resultado[1],resultado[2],resultado[3],resultado[4]);


	printf("Se acepto un cliente, atendiendolo \n");
   	/*if( read (cliente_sockfd, leer_mensaje, TAM_BUFFER) < 0 )
	{
		perror ("Ocurrio algun problema al recibir datos del cliente");
		exit(EXIT_FAILURE);
	}
   	printf ("El cliente nos envio el siguiente mensaje: \n %s \n", leer_mensaje);*/
	if( write (cliente_sockfd, datos, strlen(datos)) < 0 )
	{
		perror("Ocurrio un problema en el envio de un mensaje al cliente");
		exit(EXIT_FAILURE);
   	}
   	//kill( getppid(), SIGUSR1 );
	close (cliente_sockfd);
	exit(0);
}

void manejador( int sig ){
	int estado;
	pid_t pid;
	
	//if( sig == SIGUSR1 ){
	if( sig == SIGCHLD ){
		printf("Se recibio la senal SIGCHLD en el servidor \n");
		pid=wait(&estado);
		printf("Termino el proceso %d con estado: %d\n", pid, estado>>8);
	}else if( sig == SIGINT ){
		printf("Se recibio la senal SIGINT en el servidor \n");
		printf("Concluimos la ejecucion de la aplicacion Servidor \n");
		/*
		 *	Cierre de las conexiones
		 */
   		close (sockfd);
   		exit(0);
	}
}
