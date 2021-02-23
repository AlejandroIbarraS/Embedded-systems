#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "servidor.h"
#include "defs.h"


int main(int argc, char * argv[]){
	
	int opcion;
	if (argc != 2 )
		opcion = 1;	//Se ejecuta el servidor si no se introduce num
	else
		opcion = atoi(argv[1]);
	if(opcion < 1 && opcion > 2)
		opcion=1;

	if(opcion==1)
		servidor(); //La opcion 1 ejecuta el servidor
	else if (opcion==2)
		cliente();  //La opcion 2 ejecuta el cliente
		

	printf("La opcion es: %d\n",opcion);

	return 0;
}
