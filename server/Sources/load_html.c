#include "../Includes/preprocessor.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/resource_consts.h"
#include "../Includes/request_consts.h"
#include "../Includes/template_funcs.h"

#define DIR_LISTING_HEADER_CLASS "header_for_dir_listing"

#define DIR_LISTING_PATH_CLASS "dir_path"

#define DIR_LISTING_FILE_COUNT_CLASS "file_count"

#define DIR_LISTING_TABLE_LINE_CLASS "dir_listing_table"

#define DIR_LISTING_BODY_CLASS "dir_listing"

#define DIR_REF_CLASS "ref_for_dir"

#define PREV_DIR_REF_CLASS "ref_for_prev"

#define FILE_REF_CLASS "ref_for_file"

#define NUM_OF_FIELDS_TO_PRINT 4

static u_int64_t entry_count = 0;

static struct stat the_stat = {0};


static category_id print_order[NUM_OF_FIELDS_TO_PRINT]={
	IS_DIRECTORY,
	SIZE,
	FULL_DATE,
	FILE_NAME,
};





static char currSearchedDir[PATHSIZE*3]={0};
static char resolved_abs_path[PATH_MAX]={0};

//ls -1hu --sort=time --time=mtime --group-directories-first 

static char* the_dir_command[]={"ls","-1hu","--sort=time", "--time=mtime", "--group-directories-first",resolved_abs_path,NULL};


static char resolved_rel_path[PATH_MAX]={0},
		resolved_default_path[PATH_MAX]={0},
		header_buff[4+(PATHSIZE+4)*CATEGORY_ID_MAX]={0},
		the_date_buff[MICROTIMESIZESTRINGTHING]= {0},
		the_size_buff[MICROTIMESIZESTRINGTHING]= {0},
		curr_listing_buff[4+(PATHSIZE+8)*CATEGORY_ID_MAX]={0},
		* curr_listing_argv[ARGVMAX]={NULL,the_size_buff,the_date_buff,curr_listing_buff},
		* category_names[CATEGORY_ID_MAX]={
			"Is Directory?",
			"Size",
			"Full_Date",
			"Actual file_name"

		};

int isDirectory(char* resourceTarget){
       
    
    char path[PATHSIZE]={0};
	char* ptr=path;
	ptr+=snprintf(ptr,PATHSIZE,"%s%s",currDir,resourceTarget);
	return (does_dir_exist_aux(path));

}
static void print_header(int fd){

	memset(header_buff,0,4+(PATHSIZE+4)*CATEGORY_ID_MAX);
	char* curr_ptr=header_buff;
	for(uint32_t j=0;j<NUM_OF_FIELDS_TO_PRINT;j++){
		uint32_t i=print_order[j];
		char curr_printed_category_header_buff[PATHSIZE];
		memset(curr_printed_category_header_buff,0,PATHSIZE);
		snprintf(curr_printed_category_header_buff,PATHSIZE,"| %s  ",category_names[i]);
		curr_ptr+=snprintf(curr_ptr,PATHSIZE-1,"%s",curr_printed_category_header_buff);

	}
	dprintf(fd,"\n<h2 class='%s'><span>%s|</span></h2>\n",DIR_LISTING_HEADER_CLASS,header_buff);

}
static void print_dir_entry(int fd, int is_directory_this_one, char** curr_listing_argv,char* dir){
	
	char fucking_buffer_idfk[PATHSIZE*8]={0},
			fucking_buffer_idfk2[PATHSIZE*8]={0};
	snprintf(fucking_buffer_idfk,sizeof(fucking_buffer_idfk),"%s/%s",dir,
															curr_listing_argv[FILE_NAME]);
	
	url_encode(fucking_buffer_idfk,fucking_buffer_idfk2, reserved_characters,sizeof(fucking_buffer_idfk));
	
	dprintf(fd,"\n<span class='%s'>| %-30s | %-30s | %-30s |<a class='%s' href='%s' >%-128s</a>|</span><br>\n",
			DIR_LISTING_TABLE_LINE_CLASS,
			is_directory_this_one?"directory!":"normal file",
			curr_listing_argv[SIZE],
			curr_listing_argv[FULL_DATE],
			is_directory_this_one?DIR_REF_CLASS:FILE_REF_CLASS,
			fucking_buffer_idfk2,
			curr_listing_argv[FILE_NAME]
			);
}
static void process_lines_and_print_to_dest_file(int fd,FILE* fstream, char* dir){


	memset(curr_listing_buff,0,PATHSIZE);
	while(fgets(curr_listing_buff,PATHSIZE,fstream)){
		
		b_zero(the_date_buff);
		b_zero(the_size_buff);
		
		
		
		int strlen_of_dirent_curr = strnlen(curr_listing_buff,PATHSIZE);
		
		
		/*
		  
		  remove the newline
		 
		 
		 */
		
		
		curr_listing_buff[strlen_of_dirent_curr-1]=0;

		char full_target_path_to_test[PATH_MAX*6]={0},
		full_file_path_to_test[PATH_MAX*6]={0};
		
		snprintf(full_target_path_to_test,PATH_MAX*5-1,"%s/%s",dir,curr_listing_buff);
		snprintf(full_file_path_to_test,PATH_MAX*5-1,"%s%s/%s",currDir,dir,curr_listing_buff);

		build_file_stat(full_file_path_to_test,the_date_buff,&the_stat);
		
		humanSize(the_stat.st_size,the_size_buff);
		
		int is_directory_this_one=isDirectory(full_target_path_to_test);
		if(is_directory_this_one){
			print_dir_entry(fd, is_directory_this_one,curr_listing_argv, dir);
			entry_count++;
		}
		else
		{
			char* extension= get_file_extension(curr_listing_argv[FILE_NAME]);
			if(extension){
				if(findInStringArr(excluded_xtensions,extension)<0){
					print_dir_entry(fd, 0,curr_listing_argv, dir);
					entry_count++;
					
				}
			}
		}
		memset(curr_listing_buff,0,PATHSIZE);
	}

}
static void resolve_canonical_path(char raw_path[PATH_MAX],char dst_for_resolved_path[PATH_MAX]){

	if (realpath(raw_path, dst_for_resolved_path))
	{
		if(logging){
			fprintf(logstream,"Canonical path: de %s:\n%s\n",raw_path, dst_for_resolved_path);
		}
	}
	else
	{
		if(logging){
			fprintf(logstream,"Canonical path (relative) de %s?\nErro: %s\n",raw_path,strerror(errno));
		}
	}
}
static void resolve_canonical_path_relative(char raw_path[PATH_MAX],char dst_for_resolved_path[PATH_MAX]){
	char new_raw_path[PATH_MAX]={0};
	snprintf(new_raw_path,PATH_MAX-1,".%s",raw_path);

	if (realpath(new_raw_path, dst_for_resolved_path))
	{
		if(logging){
			fprintf(logstream,"Canonical path (relative): de %s:\n%s\n",new_raw_path, dst_for_resolved_path);
		}
	}
	else
	{
		if(logging){
			fprintf(logstream,"Canonical path (relative) de %s?\nErro: %s\n",new_raw_path,strerror(errno));
		}
	}
}
/*
	returns whether or not file has changed!
*/
static void generateDirListingPrimitive(char* path){


	int outfd= open(tmpDir,O_WRONLY|O_CREAT,0777);
	
    snprintf(currSearchedDir,sizeof(currSearchedDir)-1,"%s%s",currDir,path);
	
	resolve_canonical_path(currSearchedDir,resolved_abs_path);

	pid_t pid_fork_find_cmd=fork();
	switch(pid_fork_find_cmd){

			case -1:
					close(outfd);
					exit(-1);
			break;
			case 0:
					dup2(outfd,STDOUT_FILENO);
					close(outfd);
					execvp(the_dir_command[0],the_dir_command);
			break;
			default:
					wait(NULL);
			break;

	}
	close(outfd);

}
void generateDirListing(char* dir){


	resolve_canonical_path_relative(dir,resolved_rel_path);
	resolve_canonical_path_relative(defaultTarget,resolved_default_path);
	int pathsize=strlen(dir);
	int pathsize2=strlen(resolved_rel_path);
	int pathsize3=strlen(resolved_default_path);
	if(pathsize&&!strcmp(dir +pathsize-1,"/")){

		dir[pathsize-1]=0;
	}
	if(pathsize2&&!strcmp(resolved_rel_path +pathsize2-1,"/")){

		resolved_rel_path[pathsize2-1]=0;
	}
	int outOfRoot=strncmp(resolved_rel_path,resolved_default_path,pathsize3)&&pathsize2<pathsize3;
	if(outOfRoot){

		dir=defaultTarget;
		pathsize2=pathsize3;
	}
	int atRoot=(pathsize2==pathsize3);
	
	const char* first_format = get_fetch_parser_handler_first_format(curr_client.fetch_request_table_cell),
		* second_format = get_fetch_parser_handler_second_format(curr_client.fetch_request_table_cell);
	
	snprintf(tmpFile,sizeof(tmpFile)-1,first_format ,dir, request_id);
	replaceStringCharacter(&tmpFile[1],'/','.');
	snprintf(curr_client.relative_file_path,sizeof(curr_client.relative_file_path)-1,second_format, dir, request_id);
	replaceStringCharacter(&curr_client.relative_file_path[1],'/','.');
	snprintf(tmpDir,sizeof(tmpDir)-1,"%s%s",tmp_file_dir,tmpFile);
	snprintf(curr_client.filepath,sizeof(curr_client.filepath)-1,"%s%s",tmp_file_dir,curr_client.relative_file_path);
	
	curr_client.fd= open(curr_client.filepath,O_WRONLY|O_CREAT,0777);
	if(curr_client.fd<0){
			if(logging){
			fprintf(logstream,"ERRO NAS DIRETORIAS %s\nErro: %s\n",curr_client.filepath,strerror(errno));
			}
			curr_client.fd = -1;
			return;
	}
	else if(logging){
		fprintf(logstream,"Ficheiro criado!\n%s\n",curr_client.filepath);
	}
	
	generateDirListingPrimitive(dir);
	
	
	FILE* fstream;
	if(!(fstream=fopen(tmpDir,"r"))){
			if(logging){
				fprintf(logstream,"ERRO NAS DIRETORIAS %s\n",tmpDir);
			}
			close(curr_client.fd);
			curr_client.fd = -1;
			return;
	}
	remove(tmpDir);
	
	dprintf(curr_client.fd,STYLESHEET_FORMAT
		"\n<body class='%s'>\n\n<div class='%s'>\n<br>\n"
		"\n<h1 class=\"%s\">\n<br>"
		"<span>",
					defaultTarget,
					defaultTarget,
					DIR_LISTING_BODY_CLASS,
					DIR_LISTING_BODY_CLASS,
					DIR_LISTING_PATH_CLASS);
					
		(atRoot
				?
		dprintf(curr_client.fd,"ROOT</span></h1><br>\n")
				:
		dprintf(curr_client.fd,"%s</span></h1><br>\n",dir)
		);
	if(!atRoot){
		char prev_dir_rel_path[PATHSIZE*2]={0};
		char prev_dir_to_use[PATHSIZE*2]={0};
		snprintf(prev_dir_rel_path,PATHSIZE*2-1,"%s",dir);
		snprintf(prev_dir_to_use,PATHSIZE*2-1,"%s",dir);
		char* argv_for_slashes[ARGVMAX]={0};
		uint32_t num_tokens=make_str_arr(prev_dir_rel_path,"/",argv_for_slashes,ARGVMAX,0)-1;
		uint32_t wanted_slash_idx=num_tokens;
		uint64_t ptr_to_path_num=(uint64_t)prev_dir_rel_path;
		uint64_t ptr_to_wanted_slash_num=(uint64_t)argv_for_slashes[wanted_slash_idx];
		uint64_t subtraction_result=ptr_to_wanted_slash_num-ptr_to_path_num;
		prev_dir_to_use[subtraction_result-1]=0;
		dprintf(curr_client.fd,"\n<a class='%s' href='%s'>Prev</a><br><br><br>\n",
									PREV_DIR_REF_CLASS,
									prev_dir_to_use);

	}
	print_header(curr_client.fd);
	process_lines_and_print_to_dest_file(curr_client.fd,fstream,dir);
	dprintf(curr_client.fd,"\n<br><h2 class='%s'><span>We got %lu entries!</span></h2>\n",DIR_LISTING_FILE_COUNT_CLASS, entry_count);
	dprintf(curr_client.fd,"</div>\n<br>" AUDIO_PLAY_SCRIPT_TO_LOAD "\n\n</body>\n</html>\n",defaultTarget);
	fclose(fstream);
	lseek(curr_client.fd,0,SEEK_SET);

}

