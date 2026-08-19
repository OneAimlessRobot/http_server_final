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


char* custompostreqs[]={RCON_REQUEST,FIND_REQUEST,NULL},
	* custom_specificpostreqs[]={UPLOAD_SPECIFIC_REQUEST_STRING,NULL};


static void handleRconRequest(http_request* req){
	char* argv2[ARGVMAX]={0};
	int size=makeargvdelim(req->form,"&",argv2,ARGVMAX);
	char* the_ptr=req->processed_form;
	for(int i=0;i<size;i++){
		char* pair[2]={0};
		splitString(argv2[i],"=",pair);
		if(!strcmp(pair[0],"addr")||
				!strcmp(pair[0],"port")||
				!strcmp(pair[0],"rcon_pass")||
				!strcmp(pair[0],"cmd")){
		replaceStringCharacter(pair[1],'+',' ');
		the_ptr+=snprintf(the_ptr,
				sizeof(req->processed_form)-1,
				"%s%s",pair[1],(i==(size-1))?" ":"|");
		}
	}
}

static void handleFindRequest(http_request* req){
	char* argv2[ARGVMAX]={0};
	int size=makeargvdelim(req->form,"&",argv2,ARGVMAX);
	char* the_ptr=req->processed_form;
	for(int i=0;i<size;i++){
		char* pair[2]={0};
		splitString(argv2[i],"=",pair);
		if(!strcmp(pair[0],"query")){
		replaceStringCharacter(pair[1],'+',' ');
		the_ptr+=snprintf(the_ptr,
				sizeof(req->processed_form)-1,
				"%s%s",pair[1],(i==(size-1))?" ":"|");
		}
	}
}
static void handleUploadRequest(http_request* req){
	printf("Unimplemented function (%s)\n",req->header->specific);
}


int isCustomPostReq(http_request* req){
	int result=0;
	result=(findInStringArr(custompostreqs,req->is_a_form?req->header->target:req->header->specific)>=0);
	return result;
}

void handleCustomPostReq(http_request* req){
	if(req->is_a_form){
		if(!strncmp(req->header->target,RCON_REQUEST,sizeof(req->header->target))){
			handleRconRequest(req);

		}
		else if(!strncmp(req->header->target,FIND_REQUEST,sizeof(req->header->target))){
			handleFindRequest(req);

		}
	}
	else if(!strncmp(req->header->specific,UPLOAD_SPECIFIC_REQUEST_STRING,sizeof(req->header->specific))){
		
		handleUploadRequest(req);

	}
}

