#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "imagen.h"


#define TAM_BUFFER 100

unsigned char * reservarMemoria( int tamImagen );
void GrayToRGB(unsigned char *imagenRGB, unsigned char *imagenGray, uint32_t width, uint32_t height);
void recibirImagen(int sockfd, unsigned char * imagen,int tamImagen);
int hostname_to_ip(char *hostname , char *ip);

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in direccion_servidor;
	unsigned char * imagen, *imagenRGB;
	
	char dir[15];
	int iden=0;
	int puerto;
	printf("Ingresa el identificador del tunel: \t");
	scanf("%d",&iden);
	sprintf(dir,"%d.tcp.ngrok.io",iden);
	printf("Ingresa el puerto \t\t\t");
	scanf("%d", &puerto);
	
	
/*	
 *	AF_INET - Protocolo de internet IPV4
 *  htons - El ordenamiento de bytes en la red es siempre big-endian, por lo que
 *  en arquitecturas little-endian se deben revertir los bytes
 */	
	memset (&direccion_servidor, 0, sizeof (direccion_servidor));
	direccion_servidor.sin_family = AF_INET;
	direccion_servidor.sin_port = htons(puerto);

	char ip[100];
	hostname_to_ip(dir,ip);

/*	
 *	inet_pton - Convierte direcciones de texto IPv4 en forma binaria
 */	
	if( inet_pton(AF_INET, ip, &direccion_servidor.sin_addr) <= 0 )
	{
		perror("Ocurrio un error al momento de asignar la direccion IP");
		exit(1);
	}
/*
 *	Creacion de las estructuras necesarias para el manejo de un socket
 *  SOCK_STREAM - Protocolo orientado a conexiÃ³n
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
/*
 *	Inicia la transferencia de datos entre cliente y servidor
 */
 	bmpInfoHeader info;
	
 	printf ("Recibiendo tamaño de imagen del servidor ...\n");
	if (read (sockfd, &info, sizeof(bmpInfoHeader)) < 0)
	{	
		perror ("Ocurrio algun problema al recibir tamImagen del cliente");
		exit(1);
	}
	displayInfo(&info);
	printf("height: %d\n",info.height);
	printf("width: %d\n",info.width);
	imagen = reservarMemoria(info.height * info.width);
	printf ("Recibiendo imagen del servidor ...\n");

	recibirImagen(sockfd, imagen,info.width * info.height);


	/*
	if (read (sockfd, imagen, info.height * info.width) < 0)
	{	
		perror ("Ocurrio algun problema al recibir imagen del cliente");
		exit(1);
	}*/
	printf("aver1");
	imagenRGB = reservarMemoria(info.height * info.width *3);
	printf("aver3");
	GrayToRGB(imagenRGB, imagen, info.width, info.height);
	guardarBMP("imagen.bmp", &info, imagenRGB);
	
	
	
	printf ("El servidor recibio la imagen\n");
	printf ("Cerrando la aplicacion cliente\n");
/*
 *	Cierre de la conexion
 */
	close(sockfd);
	free(imagen);
	free(imagenRGB);

	return 0;
}
	

void recibirImagen(int sockfd, unsigned char * imagen,int tamImagen){
	int bytesRecibidos;
	while(tamImagen > 0){
		bytesRecibidos = read (sockfd, imagen, tamImagen);
		if (bytesRecibidos < 0)
		{	
			perror ("Ocurrio algun problema al recibir imagen del cliente");
			exit(1);
		}
		printf("Bytes recibidos: %d\n", bytesRecibidos);
		tamImagen = tamImagen - bytesRecibidos;
		imagen = imagen + bytesRecibidos;
	}
}



unsigned char * reservarMemoria( int tamImagen ){
	unsigned char * imagen;
	imagen=(unsigned char *)malloc(tamImagen*sizeof(unsigned char));
	if( imagen == NULL ){
		perror("Error al asignar memoria a la imagen");
		exit(EXIT_FAILURE);
	}
	

	return imagen;
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

void GrayToRGB(unsigned char *imagenRGB, unsigned char *imagenGray, uint32_t width, uint32_t height)
{
	register int x,y;
	int indiceRGB, indiceGray;

	for(y = 0; y < height; y++)
	{
		for(x = 0; x < width; x++)
		{
			indiceGray = (y * width + x);
			indiceRGB = indiceGray * 3;
			imagenRGB[indiceRGB] = imagenGray[indiceGray];
			imagenRGB[indiceRGB+1] = imagenGray[indiceGray];
			imagenRGB[indiceRGB+2] = imagenGray[indiceGray]; 
		}
	}
}
