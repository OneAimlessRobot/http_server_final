#ifndef RCON_PROXY_H
#define RCON_PROXY_H

int isRconRequest(char* resourceTarget);


char* generateRconResult(char* dir,char* the_request_parameters);

void deleteRconResult(void);

#endif
