#ifndef RCON_PROXY_H
#define RCON_PROXY_H

int isRconRequest(char* resourceTarget);


char* generateRconResult(char* the_request_parameters);

void deleteRconResult(char* the_request_parameters);

#endif
