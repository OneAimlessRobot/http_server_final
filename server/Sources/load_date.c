#include "../Includes/preprocessor.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/resource_consts.h"
#include "../Includes/template_funcs.h"
#include "../Includes/load_date.h"

char LE_TIMEUREUR[MICROTIMESIZESTRINGTHING] = {0};

static time_t timeur = {0};
static struct tm* time_struct =  NULL;

int isDateReq(char* target){
	
	return stringsAreEqual(target,"/date.date");
		
}
void the_time_func(char* unused){
	
	if(unused){
		unused[0]=0;
	}
	
	snprintf(curr_client.relative_file_path,sizeof(curr_client.relative_file_path)-1,tmpTwoDATE_HTML, request_id);
	replaceStringCharacter(&curr_client.relative_file_path[1],'/','.');
	snprintf(curr_client.filepath,sizeof(curr_client.filepath)-1,"%s%s%s",tmp_file_dir,curr_client.relative_file_path,unused);
	
	curr_client.fd= open(curr_client.filepath,O_WRONLY|O_CREAT,0777);
	if(curr_client.fd <0){
			if(logging){
			fprintf(logstream,"ERRO NA DATE|!|\nErro: %s\n",strerror(errno));
			}
			curr_client.fd = -1;
			return;
	}
	else if(logging){
		fprintf(logstream,"Ficheiro criado!\n%s\n",curr_client.filepath);
	}
	time(&timeur);

	time_struct= localtime(&timeur);	

	strftime(LE_TIMEUREUR, sizeof(LE_TIMEUREUR), "+%a, %d %b %Y %H:%M:%S %Z", time_struct);
    
    dprintf(curr_client.fd,LOAD_DATE_PREFIX,(u_int64_t)1000,LE_TIMEUREUR);
    
	lseek(curr_client.fd,0,SEEK_SET);
    
}
