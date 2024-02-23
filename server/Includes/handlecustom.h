#ifndef HANDLECUSTOM_H
#define HANDLECUSTOM_H
//custom requests
#define WRITE_VENT_REQ "/writevent"
#define SEE_FILES_REQ "/seefiles"
#define SIGN_IN_REQ "/signin"
#define SIGN_OUT_REQ "/signout"
#define KICK_CLIENT_REQ "/kickclient"
#define WRITE_VIDEO_REQ "/writevideo"
#define SEE_CLIENTS_REQ "/seeclients"

extern char* customgetreqs[];

extern char* custompostreqs[];

int isCustomGetReq(char* nulltermedtarget);

int isCustomPostReq(char* nulltermedtarget);

void handleCustomGetReq(client*c,char*target,char* body,char targetinout[PATHSIZE]);

void handleCustomPostReq(client*c,char* target,char* contents,char targetinout[PATHSIZE]);

#endif
