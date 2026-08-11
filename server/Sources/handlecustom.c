#include "../Includes/preprocessor.h"
#include <errno.h>
#include "../Includes/buffSizes.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/server_innards.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/io_ops.h"
#include "../Includes/handlecustom.h"
#include "../Includes/sock_ops.h"
#include "../Includes/load_html.h"


char* custompostreqs[]={RCON_REQUEST,NULL};


static void handleRconRequest(char* fieldmess,char targetinout[PATHSIZE]){
	char* argv2[ARGVMAX]={0};
	int size=makeargvdelim(fieldmess,"&",argv2,ARGVMAX);
	char* the_ptr=targetinout;
	for(int i=0;i<size;i++){
		char* pair[2]={0};
		splitString(argv2[i],"=",pair);
		if(!strcmp(pair[0],"addr")||
				!strcmp(pair[0],"port")||
				!strcmp(pair[0],"rcon_pass")||
				!strcmp(pair[0],"cmd")){
		replaceStringCharacter(pair[1],'+',' ');
		the_ptr+=snprintf(the_ptr,PATHSIZE-1,"%s%s",pair[1],(i==(size-2))?" ":"|");
		}
	}
}


int isCustomPostReq(char* nulltermedtarget){
	int result=0;
	result=(findInStringArr(custompostreqs,nulltermedtarget)>=0);
	return result;
}

void handleCustomPostReq(char* target,char* contents,char targetinout[PATHSIZE]){
	
	if(!strcmp(target,RCON_REQUEST)){
		handleRconRequest(contents,targetinout);

	}
}

