#ifndef HANDLECUSTOM_H
#define HANDLECUSTOM_H
//custom requests
#define RCON_REQUEST "/rcon.rcon"

extern char* custompostreqs[];

int isCustomPostReq(http_request* req);

void handleCustomPostReq(http_request* req);

#endif
