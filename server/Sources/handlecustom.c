
#include "../Includes/preprocessor.h"
#include <errno.h>
#include "../Includes/buffSizes.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/server_innards.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/io_ops.h"
#include "../Includes/load_logins.h"
#include "../Includes/handlecustom.h"
#include "../Includes/sock_ops.h"
#include "../Includes/session_ops.h"
#include "../Includes/load_html.h"

static int currvent=0;

char* customgetreqs[]={WRITE_VENT_REQ,SEE_FILES_REQ,SIGN_IN_REQ,NULL};

char* custompostreqs[]={WRITE_VENT_REQ,SIGN_IN_REQ,SEE_CLIENTS_REQ,SIGN_OUT_REQ,KICK_CLIENT_REQ,WRITE_VIDEO_REQ,NULL};


static void handleVentReq(char* fieldmess,char targetinout[PATHSIZE]){
	char* pathbuff=malloc(PATHSIZE*2);
	memset(pathbuff,0,PATHSIZE*2);
	char* argv2[ARGVMAX]={0};
	snprintf(pathbuff,PATHSIZE*2,"%s/resources/vents/ventNumber%d",currDir,currvent);
	FILE* stream=NULL;
	if(!(stream=fopen(pathbuff,"wb+"))){

		fprintf(logstream,"ERRO A ABRIR VENT!!!!!\n%s\n",strerror(errno));
		return;
	}
	int size=makeargvdelim(fieldmess,"&",argv2,ARGVMAX);
	for(int i=0;i<size;i++){
		char* pair[2]={0};
		splitString(argv2[i],"=",pair);
		if(!strcmp(pair[0],"venttitle")||!strcmp(pair[0],"ventcontent")){
		replaceStringCharacter(pair[1],'+',' ');
		fwrite(pair[0], 1, strlen(pair[0]),stream);
		fwrite(": ",1,2,stream);
		fwrite(pair[1],1,strlen(pair[1]),stream);
		fwrite("\n",1,1,stream);
		}
		
	}
	free(pathbuff);
	memcpy(targetinout,defaultTarget,strlen(defaultTarget));
	fclose(stream);
	currvent++;
}

static void handleVideoPostReq(void){
	
	printf("Video posted!!!!\n");

}

int isCustomGetReq(char* nulltermedtarget){
	int result=0;
	char* targetcopy= strdup(nulltermedtarget);
	char* argv2[2];
	memset(argv2,0,2*sizeof(char*));
	splitString(targetcopy,"?",argv2);
	
	result=(findInStringArr(customgetreqs,argv2[0])>=0);
	//printf("Sera que %s e um get req custom? A resposta a isso e %d\n",argv2[0],result);
	free(targetcopy);
	return result;
}

int isCustomPostReq(char* nulltermedtarget){
	int result=0;
	result=(findInStringArr(custompostreqs,nulltermedtarget)>=0);
	return result;
}

void handleCustomGetReq(client* c,char*target,char* body,char targetinout[PATHSIZE]){
	char* targetcopy= strdup(target);
	char* argv2[2];
	memset(argv2,0,2*sizeof(char*));
	splitString(targetcopy,"?",argv2);
	
	if(!strcmp(argv2[0],SIGN_IN_REQ)){
		handleLogin(c,body,targetinout);

	}
	/*
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}*/
	free(targetcopy);
	
}
void handleCustomPostReq(client*c,char* target,char* contents,char targetinout[PATHSIZE]){
	if(!strcmp(target,WRITE_VENT_REQ)){
		handleVentReq(contents,targetinout);
		
	}
	else if(!strcmp(target,SIGN_IN_REQ)){
		handleLogin(c,contents,targetinout);
		
	}
	else if(!strcmp(target,SEE_CLIENTS_REQ)){
		//generateClientListing(targetinout);

	}
	else if(!strcmp(target,SIGN_OUT_REQ)){
		handleLogout(c,targetinout);

	}
	else if(!strcmp(target,WRITE_VIDEO_REQ)){
		
		handleVideoPostReq();

	}
	/*else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}
	else if(!strcmp(nulltermedtarget,writeventreq)){


	}*/
}

