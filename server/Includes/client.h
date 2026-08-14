#ifndef CLIENT_H
#define CLIENT_H
typedef struct client{
	
	pthread_t threadid;
	fd_set readfd;
	char* peerbuff;
	int socket, is_tls;
	SSL* ctx_if_ssl;
	char username[FIELDSIZE];
	struct sockaddr_in client_addr;
	
}client;
extern u_int64_t request_id;

extern char tmpFile[PATHSIZE*3],
	tmpFile2[PATHSIZE*3],
	tmpDir[PATHSIZE*4],
	tmpDir2[PATHSIZE*4];
	
void runClientConnection(int is_tls, int sock);

#endif
