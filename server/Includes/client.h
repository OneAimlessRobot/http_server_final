#ifndef CLIENT_H
#define CLIENT_H
typedef struct client{
	
	char peerbuff[BUFFSIZE],
		filepath[PATHSIZE*4],
		target[PATHSIZE],
		sent_header_buff[PATHSIZE];
	int socket, fd, is_tls;
	SSL* ctx_if_ssl;
	struct sockaddr_in client_addr;
	
}client;
extern u_int64_t request_id;
extern client curr_client;
extern char tmpFile[PATHSIZE*3],
	tmpFile2[PATHSIZE*3],
	tmpDir[PATHSIZE*4];
	
void runClientConnection(int is_tls, int sock);

#endif
