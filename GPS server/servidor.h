void cliente(char *dir, int puerto);
void* servidor(void* arg);
void iniCliente(char *dir, int puerto);
void iniSignals();
void iniServidor();
void atiendeCliente( int cliente_sockfd);
void manejador( int sig );
int hostname_to_ip(char *hostname , char *ip);