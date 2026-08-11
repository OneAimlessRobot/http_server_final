#ifndef HANDLECUSTOM_H
#define HANDLECUSTOM_H
//custom requests
#define RCON_REQUEST "/rcon.rcon"

extern char* custompostreqs[];

int isCustomPostReq(char* nulltermedtarget);

void handleCustomPostReq(char* target,char* contents,char targetinout[PATHSIZE]);

#endif
