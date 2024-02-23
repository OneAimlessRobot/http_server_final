#ifndef CLIENT_H
#define CLIENT_H
typedef struct client{
	
	pthread_t threadid;
	fd_set readfd;
	char* peerbuff;
	int socket;
	char username[FIELDSIZE];
	struct sockaddr_in client_addr;
	
}client;

void runClientConnection(int sock);

#endif
