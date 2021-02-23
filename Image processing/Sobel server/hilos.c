#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include "defs.h"
#include "imagen.h"


extern pthread_mutex_t bloqueo;
int gradienteFila[DIMASK * DIMASK] = {
	1,0,-1,
	2,0,-2,
	1,0,-1
}; 
int gradienteColumna[DIMASK * DIMASK] = {
	-1,-2,-1,
	0 ,0 ,0 ,
	1 ,2 ,1
};

extern unsigned char *imagenGG, *imagenSG;
extern uint32_t width, height;

//Hilos bloque
void * funHilo( void * arg )
{
	int nucleo = *(int *)arg;
	int eleBloque = (height-DIMASK+1) / NUM_HILOS;	//tamaño de bloque
	int iniBloque = nucleo * eleBloque;
	int finBloque = iniBloque + eleBloque;

	printf("Hilo %d en ejecucion \n", nucleo);

	register int x, y, xm, ym;
	int indicem, indicei, convFil, convCol;
 
	for( y=iniBloque; y<finBloque ; y++ ){//Este ciclo es el que se paraleliza
		for(x = 0; x <= width-DIMASK; x++)
		{
			indicem = 0;
			convFil = 0;
			convCol = 0;
			for(ym = 0; ym < DIMASK; ym++)
			{
				for(xm = 0; xm < DIMASK; xm++)
				{
					indicei = (y+ym) * width + (x+xm);
					convFil += imagenGG[indicei] * gradienteFila[indicem];
					convCol += imagenGG[indicei] * gradienteColumna[indicem++];
				}
				
			}
			convFil = convFil >> 2;
			convCol = convCol >> 2;
			indicei = (y+1)*width + (x+1);
			imagenSG[indicei] = (unsigned char)sqrt( (convFil * convFil) + (convCol * convCol) );;

		}
	}
	pthread_exit( arg );
}