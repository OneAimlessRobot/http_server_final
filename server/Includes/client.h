#ifndef CLIENT_H
#define CLIENT_H
typedef struct client{
	
	pthread_t threadid;
	fd_set readfd;
	char* peerbuff;
	int socket;
	SSL* ctx_if_ssl;
	char username[FIELDSIZE];
	struct sockaddr_in client_addr;
	
}client;

void runClientConnection(int is_tls, int sock);

#endif
