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
#include "defs.h"
#include "servidor.h"


int main()
{
	servidor();
   	
	return 0;
}

