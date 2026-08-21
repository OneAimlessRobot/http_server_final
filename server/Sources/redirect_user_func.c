#include "../Includes/preprocessor.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_response_aux.h"
#include "../Includes/server_vars.h"
#include "../Includes/redirect_user_func.h"
#include "../Includes/server_innards.h"

int redirectUser(client* c,char* target){

	char headerBuff[PATHSIZE]={0};
		fillUpRedirectHeader(headerBuff,target);
		int size=strlen(headerBuff);
			if(send(c->socket,headerBuff,size,0)!=(size)){
				if(logging){
				fprintf(logstream,"ERRO NO SEND!!!! O GET TEM UM ARGUMENTO!!!!:\n%s\n",strerror(errno));
				}
			}

	return 0;
}

