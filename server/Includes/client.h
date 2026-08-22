#ifndef CLIENT_H
#define CLIENT_H
extern u_int64_t request_id;
extern client curr_client;
extern char tmpFile[PATHSIZE*3],
	tmpDir[PATHSIZE*5];
	
void runClientConnection(u_int8_t is_tls, int32_t sock);


#endif
