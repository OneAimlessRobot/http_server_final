#include "../Includes/preprocessor.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/resource_consts.h"
#include "../Includes/template_funcs.h"
#include "../Includes/load_date.h"

char LE_TIMEUREUR_DATE[MICROTIMESIZESTRINGTHING] = {0},

		LE_TIMEUREUR_LAST_MODIFIED_DATE[MICROTIMESIZESTRINGTHING] = {0},
		
		ETag[MICROTIMESIZESTRINGTHING] = {0};

	
static time_t date_timeur = {0};
static struct stat last_modified_stat_struct = {0};
static struct tm* time_struct_for_date =  NULL;

int isDateReq(char* target){
	
	return stringsAreEqual(target,"/date.date");
		
}

void build_date_buff(void){
	
	
	time(&date_timeur);

	time_struct_for_date= localtime(&date_timeur);	

	strftime(LE_TIMEUREUR_DATE, sizeof(LE_TIMEUREUR_DATE), "%a, %d %b %Y %H:%M:%S %Z", time_struct_for_date);

}
void build_last_modified_buff(char* full_file_path){
	
	if(!full_file_path){
		
		return;
		
	}
	int ret = lstat(full_file_path, &last_modified_stat_struct);
	if (ret == -1) {
		perror("lstat");
		return;
	}
    
    
    ctime_r( &last_modified_stat_struct.st_mtim.tv_sec , LE_TIMEUREUR_LAST_MODIFIED_DATE);
	LE_TIMEUREUR_LAST_MODIFIED_DATE[strnlen(LE_TIMEUREUR_LAST_MODIFIED_DATE,sizeof(LE_TIMEUREUR_LAST_MODIFIED_DATE)-1)-1]=0;
	
	/*
	 
	 https://stackoverflow.com/questions/4533/http-generating-etag-header
	
	*/
	
	snprintf(ETag,sizeof(ETag)-1, "%ld-%ld-%ld", last_modified_stat_struct.st_mtime,
			last_modified_stat_struct.st_size,
			last_modified_stat_struct.st_ino);
    
	printf("The path to be dated: %s\n", full_file_path);
	printf("The date: %s\n", LE_TIMEUREUR_LAST_MODIFIED_DATE);
	
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
    
    dprintf(curr_client.fd,LOAD_DATE_PREFIX,(u_int64_t)1000,LE_TIMEUREUR_DATE);
    
	lseek(curr_client.fd,0,SEEK_SET);
    
}
