#ifndef HANDLECUSTOM_H
#define HANDLECUSTOM_H
//custom requests
#define RCON_REQUEST "/rcon.rcon"
#define FIND_REQUEST "/find.find"

extern char* custompostreqs[];

int isCustomPostReq(http_request* req);

void handleCustomPostReq(http_request* req);

#endif
