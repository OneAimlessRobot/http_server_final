#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../Includes/buffSizes.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/server_innards.h"
#include "../Includes/sock_ops.h"
#include "../Includes/load_logins.h"
#include "../Includes/resource_consts.h"

void handleLogin(client* c,char* fieldmess,char targetinout[PATHSIZE]){
	
	char* argv2[ARGVMAX]={0};
	int size=makeargvdelim(fieldmess,"&",argv2,ARGVMAX);
	if(!size){

	memcpy(targetinout,defaultLoginTarget,strlen(defaultLoginTarget));
	return;
	}
		char* username[2]={0};
		splitString(argv2[0],"=",username);
		if(!strlen(username[1])){

			memcpy(targetinout,defaultLoginTarget,strlen(defaultLoginTarget));
			return;
		}
		char* password[2]={0};
		char* correctPassword;
		splitString(argv2[1],"=",password);
		memset(c->username,0,FIELDSIZE);
		//int clientExists=clientIsRepeated(username[1]);
		  int clientExists=0;
			if(strcmp((correctPassword=find_login_pw_in_login_arr(username[1],currLogins)),"NO_SUCH_LOGIN")){
				
				if(stringsAreEqual(correctPassword,password[1])&&!clientExists){
					strncpy(c->username,username[1],FIELDSIZE);
					memcpy(targetinout,defaultTarget,strlen(defaultTarget));
					printf("Bem vindo %s\n",username[1]);
					return;
				}
				
			}
	if(clientExists){
		printf("Cliente repetido ok %s?\n",username[1]);
		memcpy(targetinout,doubleSessionTarget,strlen(doubleSessionTarget));
		return;
	}
	else{
	memcpy(targetinout,incorrectLoginTarget,strlen(incorrectLoginTarget));
	}
}
void handleLogout(client* c,char targetinout[PATHSIZE]){

		memset(c->username,0,FIELDSIZE);
		memcpy(targetinout,defaultLoginTarget,strlen(defaultLoginTarget));

}
