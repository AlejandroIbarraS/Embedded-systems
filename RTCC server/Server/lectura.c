#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <time.h>
#include "lectura.h"
#include "defs.h"

#define DIR_RTCC 0x68


extern int pipefd[2];
extern int edo_pipe;

void *lectura(void *arg)
{
	int segundos = 0x39, minutos = 0x43, horas = 0x14, iic_fd;
	int tempMSB = 0,tempLSB = 0;
	char msg[TAM_BUFFER];
	float temp;
	iic_fd=wiringPiI2CSetup(DIR_RTCC);
	if(iic_fd == -1){
		perror("Error al inicializar el bus I2C");
		exit(EXIT_FAILURE);
	}
	for(;EVER;){
		segundos = wiringPiI2CReadReg8(iic_fd,0);
		usleep(2000);
		minutos = wiringPiI2CReadReg8(iic_fd,1);
		usleep(2000);
		horas  = wiringPiI2CReadReg8(iic_fd,2);
		usleep(2000);
		
		tempMSB  = wiringPiI2CReadReg8(iic_fd,0x11);
		usleep(2000);
		tempLSB  = wiringPiI2CReadReg8(iic_fd,0x12);
		usleep(2000);
		
		tempLSB = tempLSB >> 6;
		temp = tempLSB * 25 / 100.0;
		temp += tempMSB;
		
		sprintf(msg,"La hora es: %x:%x:%x\nLa temperatura es: %f°\n",horas,minutos,segundos,temp);
		write( pipefd[1], msg, sizeof(char)*TAM_BUFFER);
		sleep(2);
	}
	return 0;
}
