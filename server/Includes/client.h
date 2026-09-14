#ifndef CLIENT_H
#define CLIENT_H
extern u_int64_t request_id;
extern client curr_client;
extern char tmpFile[PATHSIZE*3],
	tmpDir[PATHSIZE*5];
	
void runClientConnection(int32_t sock);


#endif
