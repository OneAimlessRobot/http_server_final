#include "../Includes/preprocessor.h"
#include "../Includes/buffSizes.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/server_vars.h"
#include "../Includes/load_logins.h"
#include <sys/socket.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#define LOGINS_DIR "/creds/logins.txt"
#define ADMINS_DIR "/creds/admins.txt"
login** currLogins=NULL;
login** currAdmins=NULL;

static void print_login_arr(FILE* fstream,login** logins){

	for (int i=0;logins[i];i++){
		
		fprintf(fstream,"Login no. %d:\nLogin name: %s\nLogin password: %s\n",i,logins[i]->fields[0],logins[i]->fields[1]);

	}

}
char* find_login_pw_in_login_arr(char* user_name,login** logins){
	if(logins){
	for (int i=0;logins[i];i++){
		
		if(!strncmp(logins[i]->fields[0],user_name,FIELDSIZE)){
			return logins[i]->fields[1];
		}
	}
	return "NO_SUCH_LOGIN";
	}
	return "NO_LOGINS_LOADED";

}
void loadAdmins(void)
{
	int numOfLogins=0;
	FILE* loginStream;
	char path[PATHSIZE*2]={0};
	snprintf(path,PATHSIZE*2,"%s%s",currDir,ADMINS_DIR);
	if(!(loginStream=fopen(path,"r"))){
		
		if(logging){

			fprintf(logstream,"ADMINS NAO CARREGADOS!!!!!\nPATH INVALIDA!!!!\nPath: %s\nErro: %s\n",path,strerror(errno));
		}
		return;
	}
	char* buff=malloc(FIELDSIZE+1);
	memset(buff,0,FIELDSIZE+1);
	while(fgets(buff,FIELDSIZE,loginStream)){
	
		numOfLogins++;
	
	}
	if(!numOfLogins){
		fclose(loginStream);
		return;
	}
	memset(buff,0,FIELDSIZE+1);
	rewind(loginStream);
	currAdmins=malloc(sizeof(login*)*(numOfLogins+1));
	int i=0;
	while(fgets(buff,FIELDSIZE,loginStream)){
		
		currAdmins[i]=malloc(sizeof(login));
		currAdmins[i]->mem=strdup(buff);
		splitString(currAdmins[i]->mem," ",currAdmins[i]->fields);
		int passwordSize=strlen(currAdmins[i]->fields[1]);
		currAdmins[i]->fields[1][passwordSize-1]=0;
		memset(buff,0,FIELDSIZE+1);
		i++;
		

	}
	currAdmins[i]=NULL;
	if(logging){
		fprintf(logstream,"\nAdmins carregados:\n");
		print_login_arr(logstream,currAdmins);
	}
	fclose(loginStream);
	free(buff);
	return;


}
void loadLogins(void)
{
	int numOfLogins=0;
	FILE* loginStream;
	char path[PATHSIZE*2]={0};
	snprintf(path,PATHSIZE*2,"%s%s",currDir,LOGINS_DIR);
	if(!(loginStream=fopen(path,"r"))){
		
		if(logging){

			fprintf(logstream,"LOGINS NAO CARREGADOS!!!!!\nPATH INVALIDA!!!!\nPath: %s\nErro: %s\n",path,strerror(errno));
		}
		return;
	}
	char* buff=malloc(FIELDSIZE+1);
	memset(buff,0,FIELDSIZE+1);
	while(fgets(buff,FIELDSIZE,loginStream)){
	
		numOfLogins++;
	
	}
	if(!numOfLogins){
		fclose(loginStream);
		return;
	}
	memset(buff,0,FIELDSIZE+1);
	rewind(loginStream);
	currLogins=malloc(sizeof(login*)*(numOfLogins+1));
	int i=0;
	while(fgets(buff,FIELDSIZE,loginStream)){
		
		currLogins[i]=malloc(sizeof(login));
		currLogins[i]->mem=strdup(buff);
		splitString(currLogins[i]->mem," ",currLogins[i]->fields);
		int passwordSize=strlen(currLogins[i]->fields[1]);
		currLogins[i]->fields[1][passwordSize-1]=0;
		memset(buff,0,FIELDSIZE+1);
		i++;
		

	}
	currLogins[i]=NULL;
	if(logging){
		fprintf(logstream,"\nLogins carregados:\n");
		print_login_arr(logstream,currLogins);
	}
	fclose(loginStream);
	free(buff);
	return;


}


void freeLogins(login*** logins)
{
	if(*logins){
		
	for (int i=0;(*logins)[i];i++){
		
		free((*logins)[i]->mem);
		free((*logins)[i]);
	}
	free((*logins));
	*logins=NULL;
	}



}
