#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "imagen.h"
#include "procesamiento.h"
#include "servidor.h"
#include "defs.h"


int width, height;
int sockfd;


void servidor(){
	pid_t pid;
   	int cliente_sockfd;
	
	iniSignals();
	sockfd = iniServidor();

	for(;EVER;)
	{
	   	printf ("En espera de peticiones de conexión %d...\n",pid);
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

int iniServidor(){
	int sockfd;
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
   	return sockfd;
}

void atiendeCliente( int cliente_sockfd ){
	//Tomar foto
	system("raspistill -w 300 -h 300 -o foto.bmp");
	system("convert foto.bmp foto.bmp");

	//Leer imagen
	bmpInfoHeader info;
	unsigned char *imagenGray, *imagenRGB, *imagenSobel;
	int tamImagen;
	

	imagenRGB = abrirBMP("foto.bmp", &info);
	displayInfo( &info );


	imagenGray = reservarMemoria ( info.width, info.height );
	imagenSobel = reservarMemoria ( info.width, info.height );
	memset( imagenSobel, 255, info.width*info.height);
	RGBToGray( imagenRGB , imagenGray, info.width, info.height );
	
	width = info.width;
	height = info.height;
	Sobel(imagenGray, imagenSobel, info.width, info.height);

	tamImagen = info.width * info.height;
	printf("Se acepto un cliente, enviando imagen.. \n\n\n");
	if( write (cliente_sockfd, &info, sizeof(bmpInfoHeader)) < 0 )
	{
		perror("Ocurrio un problema en el envio del header de la imagen");
		exit(EXIT_FAILURE);
   	}
	
	if( write (cliente_sockfd, imagenSobel, tamImagen) < 0 )
	{
		perror("Ocurrio un problema en el envio de la imagen");
		exit(EXIT_FAILURE);
   	}
   	//kill( getppid(), SIGUSR1 );
	free(imagenSobel);
	free(imagenRGB);
	free(imagenGray);
	close(cliente_sockfd);
	exit(0);
}

void manejador( int sig ){
	int estado;
	pid_t pid;
	if( sig == SIGCHLD ){
		printf("Se recibio la senal SIGCHLD en el servidor \n");
		pid=wait(&estado);
		printf("Termino el proceso %d con estado: %d\n", pid, estado>>8);
	}else if( sig == SIGINT ){
		printf("Se recibio la senal SIGINT en el servidor \n");
		printf("Concluimos la ejecucion de la aplicacion Servidor \n");
		close (sockfd);
		exit(0);
	}
}
