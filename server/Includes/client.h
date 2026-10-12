#ifndef CLIENT_H
#define CLIENT_H
extern u_int64_t request_id;
extern client curr_client;
extern char tmpFile[PATHSIZE*3],
	tmpDir[PATHSIZE*5];
	
void send_internal_error_response(void);

void runClientConnection(int32_t sock);

#endif
