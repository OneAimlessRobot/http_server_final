#include "../Includes/preprocessor.h"
#include "../Includes/http_response_aux.h"
#include "../Includes/server_vars.h"

int redirectUser(client* c,char* target){

	fillUpRedirectHeader(target);
		int size=strlen(c->sent_header_buff);
			if(send(c->socket,c->sent_header_buff,size,0)!=(size)){
				if(logging){
				fprintf(logstream,"ERRO NO SEND!!!! O GET TEM UM ARGUMENTO!!!!:\n%s\n",strerror(errno));
				}
			}

	return 0;
}

