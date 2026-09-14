#include "../Includes/preprocessor.h"
#include "../Includes/client.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/handlecustom.h"


char* custompostreqs[]={RCON_REQUEST,FIND_REQUEST, NULL},
	* custom_specificpostreqs[]={UPLOAD_SPECIFIC_REQUEST_STRING,NULL};


static void handleRconRequest(http_request* req){
	char* argv2[ARGVMAX]={0};
	int size=makeargvdelim(req->form,"&",argv2,ARGVMAX,0);
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
	int size=makeargvdelim(req->form,"&",argv2,ARGVMAX,0);
	char* the_ptr = req->processed_form;
	for(int i=0;i<size;i++){
		char* pair[2]={0};
		splitString(argv2[i],"=",pair);
		if(!strcmp(pair[0],"query")
				||
		   !strcmp(pair[0],"take_me_there")){
		
		replaceStringCharacter(pair[1],'+',' ');
		the_ptr+=snprintf(the_ptr,
				sizeof(req->processed_form)-1,
				"%s%s",pair[1],(i==(size-1))?" ":"|");
		}
	}
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
}

