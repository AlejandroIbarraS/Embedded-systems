#ifndef SERVIDOR_H
#define SERVIDOR_H

void manejador( int sig );
void iniServidor();
void iniCliente();
void iniSignals();
void atiendeCliente( int cliente_sockfd );
void servidor();
void cliente();



#endif
