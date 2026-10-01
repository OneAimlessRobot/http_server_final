#include "../Includes/preprocessor.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/resource_consts.h"
#include "../Includes/request_consts.h"
#include "../Includes/template_funcs.h"
#include "../Includes/load_date.h"

char LE_TIMEUREUR_DATE[MICROTIMESIZESTRINGTHING] = {0},

		LE_TIMEUREUR_LAST_MODIFIED_DATE[MICROTIMESIZESTRINGTHING] = {0},
		MD5Hash[EVP_MAX_MD_SIZE] = {0},
		MD5HashTextForm[ETAG_SIZE/2] = {0},
		ETag[ETAG_SIZE] = {0};

timer_struct_t struct_for_date = {0},
				struct_for_mod_date = {0},
				struct_for_access_date = {0};

static struct stat last_modified_stat_struct = {0};

int isDateReq(char* target){
	
	return stringsAreEqual(target,"/date.date");
		
}

void build_date_buff(void){
	
	
	time(&struct_for_date.timeur);

	struct_for_date.time_struct_ptr = gmtime(&struct_for_date.timeur);	

	strftime(LE_TIMEUREUR_DATE, sizeof(LE_TIMEUREUR_DATE), "%a, %d %b %Y %H:%M:%S %Z", struct_for_date.time_struct_ptr);

}

int build_file_stat(char* full_file_path, char* modified_date_buff, struct stat* the_stat){
	
	if(!full_file_path){
		
		return -1;
		
	}
	int ret = lstat(full_file_path, the_stat);
	if (ret == -1) {
		perror(full_file_path);
		return -1;
	}
    timer_struct_t the_struct_for_time = {0};
    
    the_struct_for_time.timeur = the_stat->st_mtime;
    
	the_struct_for_time.time_struct_ptr = gmtime(&the_struct_for_time.timeur);	
	
	strftime(modified_date_buff, sizeof(LE_TIMEUREUR_LAST_MODIFIED_DATE), 
				"%a, %d %b %Y %H:%M:%S %Z", the_struct_for_time.time_struct_ptr);

	
	return 0;
}

void build_last_modified_buff(char* full_file_path){
	
	
	int result = build_file_stat(full_file_path, LE_TIMEUREUR_LAST_MODIFIED_DATE, &last_modified_stat_struct);
	
	if(result){
		return;
	}

	/*
	 
	 https://stackoverflow.com/questions/4533/http-generating-etag-header
	
	*/
    FILE *inFile = fopen (full_file_path, "rb");
    EVP_MD_CTX* mdContext = EVP_MD_CTX_create();
    int bytes;
    unsigned char data[1024];
	EVP_DigestInit(mdContext, EVP_md5());
    while ((bytes = fread (data, 1, 1024, inFile)) != 0){
        EVP_DigestUpdate(mdContext, data, bytes);
    }
    fclose (inFile);
    EVP_DigestFinal_ex(mdContext,(unsigned char*)MD5Hash,NULL);
    EVP_MD_CTX_reset(mdContext);
    EVP_MD_CTX_free(mdContext);
    
	for(int i = 0; i < EVP_MAX_MD_SIZE; i++) {
		snprintf(&MD5HashTextForm[i*2],10, "%02x", (unsigned int)MD5Hash[i]);
	}
	
	snprintf(ETag,sizeof(ETag), "%s-%lx-%lx",
			MD5HashTextForm,
			curr_client.req->header->range_start,
			curr_client.req->header->range_end<=0
				?
			(last_modified_stat_struct.st_size-1)
				:
			curr_client.req->header->range_end);
	
}
void generateDate(char* unused){
	
	if(unused){
		unused[0]=0;
	}
	
	const char* first_format = get_fetch_parser_handler_first_format(curr_client.fetch_request_table_cell);
	
	snprintf(curr_client.relative_file_path,sizeof(curr_client.relative_file_path)-1,first_format, request_id);
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
