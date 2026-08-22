#ifndef HANDLECUSTOM_H
#define HANDLECUSTOM_H
//custom requests
#define RCON_REQUEST "/rcon.rcon"
#define FIND_REQUEST "/find.find"

#define UPLOAD_SPECIFIC_REQUEST_STRING "upload"

extern char* custompostreqs[],
		* custom_specificpostreqs[];

int isCustomPostReq(http_request* req);

void handleCustomPostReq(http_request* req);

#endif
