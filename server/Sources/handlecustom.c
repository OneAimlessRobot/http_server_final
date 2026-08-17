#include "../Includes/preprocessor.h"
#include <errno.h>
#include "../Includes/buffSizes.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/server_innards.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/io_ops.h"
#include "../Includes/handlecustom.h"
#include "../Includes/sock_ops.h"
#include "../Includes/load_html.h"


char* custompostreqs[]={RCON_REQUEST,FIND_REQUEST,NULL};


static void handleRconRequest(http_request* req){
	char* argv2[ARGVMAX]={0};
	int size=makeargvdelim(req->data,"&",argv2,ARGVMAX);
	char* the_ptr=req->processed_data;
	for(int i=0;i<size;i++){
		char* pair[2]={0};
		splitString(argv2[i],"=",pair);
		if(!strcmp(pair[0],"addr")||
				!strcmp(pair[0],"port")||
				!strcmp(pair[0],"rcon_pass")||
				!strcmp(pair[0],"cmd")){
		replaceStringCharacter(pair[1],'+',' ');
		the_ptr+=snprintf(the_ptr,
				sizeof(req->processed_data)-1,
				"%s%s",pair[1],(i==(size-2))?" ":"|");
		}
	}
}

static void handleFindRequest(http_request* req){
	char* argv2[ARGVMAX]={0};
	int size=makeargvdelim(req->data,"&",argv2,ARGVMAX);
	char* the_ptr=req->processed_data;
	for(int i=0;i<size;i++){
		char* pair[2]={0};
		splitString(argv2[i],"=",pair);
		if(!strcmp(pair[0],"rootdir")||
				!strcmp(pair[0],"regexp")){
		replaceStringCharacter(pair[1],'+',' ');
		the_ptr+=snprintf(the_ptr,
				sizeof(req->processed_data)-1,
				"%s%s",pair[1],(i==(size-2))?" ":"|");
		}
	}
}


int isCustomPostReq(http_request* req){
	int result=0;
	result=(findInStringArr(custompostreqs,req->header->target)>=0);
	return result;
}

void handleCustomPostReq(http_request* req){
	
	if(!strncmp(req->header->target,RCON_REQUEST,sizeof(req->header->target))){
		handleRconRequest(req);

	}
	else if(!strncmp(req->header->target,FIND_REQUEST,sizeof(req->header->target))){
		handleFindRequest(req);

	}
}

