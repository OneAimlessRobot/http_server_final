#include "../Includes/preprocessor.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/resource_consts.h"
#include "../Includes/template_funcs.h"

#define SITE_PATH_HEADER_CLASS "header_for_site_path"

#define DIR_LISTING_HEADER_CLASS "header_for_dir_listing"

#define DIR_LISTING_TABLE_LINE_CLASS "dir_listing_table"

#define DIR_LISTING_BODY_CLASS "dir_listing"

#define DIR_REF_CLASS "ref_for_dir"

#define FILE_REF_CLASS "ref_for_file"

#define NUM_OF_FIELDS_TO_PRINT 4

static char* category_names[CATEGORY_ID_MAX]={
			"Permissions",
			"num_links",
			"Owner_name",
			"Owner_group",
			"Size",
			"Date",
			"time",
			"Actual_file_name"

};

static category_id print_order[NUM_OF_FIELDS_TO_PRINT]={
	SIZE,
	DIR_DATE,
	TIME,
	FILE_NAME,
};

static uint16_t category_max_sizes[CATEGORY_ID_MAX]={0};
static uint16_t category_curr_sizes[CATEGORY_ID_MAX]={0};



static const char* dir_listing_css_file_path= "/webcrookedstuff/.dir_listing_style.css";

static char currSearchedDir[PATHSIZE*3]={0};
static char resolved_abs_path[PATH_MAX]={0};

//ls -lhu --sort=time --time=mtime --time-style=long-iso --group-directories-first 

static char* the_dir_command[]={"ls","-lhu","--sort=time", "--time=mtime", "--time-style=long-iso", "--group-directories-first",resolved_abs_path,NULL};


static char resolved_rel_path[PATH_MAX]={0},
		resolved_default_path[PATH_MAX]={0},
		header_buff[4+(PATHSIZE+4)*CATEGORY_ID_MAX]={0},
		* curr_listing_argv[ARGVMAX]={0},
		* header_argv[ARGVMAX]={0},
		curr_listing_buff[4+(PATHSIZE+8)*CATEGORY_ID_MAX]={0};


int isDirectory(char* resourceTarget){
       
    
    char path[PATHSIZE]={0};
	char* ptr=path;
	ptr+=snprintf(ptr,PATHSIZE,"%s%s",currDir,resourceTarget);
	return does_dir_exist_aux(path);

}



static void generate_header_sizes_data(FILE* fstream){

	memset(curr_listing_buff,0,PATHSIZE);
	fgets(curr_listing_buff,PATHSIZE,fstream);
	while(fgets(curr_listing_buff,PATHSIZE,fstream)){
		memset(header_argv,0,ARGVMAX*sizeof(char*));
		makeargvdelim(curr_listing_buff," \t\n", header_argv, -1,0);
		for(uint32_t j=0;j<NUM_OF_FIELDS_TO_PRINT;j++){
			uint32_t i=print_order[j];
			category_curr_sizes[i]=header_argv[i]?strnlen(header_argv[i],PATHSIZE-1):0;
			if(category_curr_sizes[i]>category_max_sizes[i]){
				category_max_sizes[i]=max(strlen(category_names[i]),min(PATHSIZE,category_curr_sizes[i]));
			}
		}
		memset(curr_listing_buff,0,PATHSIZE);
	}

}
static void print_header(int fd){

	memset(header_buff,0,4+(PATHSIZE+4)*CATEGORY_ID_MAX);
	char* curr_ptr=header_buff;
	for(uint32_t j=0;j<NUM_OF_FIELDS_TO_PRINT;j++){
		uint32_t i=print_order[j];
		char curr_printed_category_header_buff[category_max_sizes[i]+PATHSIZE];
		memset(curr_printed_category_header_buff,0,category_max_sizes[i]+PATHSIZE);
		snprintf(curr_printed_category_header_buff,PATHSIZE,"| %s  ",category_names[i]);
		curr_ptr+=snprintf(curr_ptr,PATHSIZE-1,"%s",curr_printed_category_header_buff);

	}
	dprintf(fd,"\n<h2 class='%s'><span>%s|</span></h2>\n",DIR_LISTING_HEADER_CLASS,header_buff);

}
static void print_dir_entry(int fd, int is_directory_this_one, char** curr_listing_argv,char* dir){
	
	
	dprintf(fd,"\n<span class='%s'>| %-30s | %-10s | %-10s |<a class='%s' href='%s/%s'>%-128s</a>|</span><br>\n",
			DIR_LISTING_TABLE_LINE_CLASS,
			is_directory_this_one?"directory!":curr_listing_argv[SIZE],
			curr_listing_argv[DIR_DATE],
			curr_listing_argv[TIME],
			is_directory_this_one?DIR_REF_CLASS:FILE_REF_CLASS,
			dir,
			curr_listing_argv[FILE_NAME],
			curr_listing_argv[FILE_NAME]
			);
}
static void process_lines_and_print_to_dest_file(int fd,FILE* fstream, char* dir){


	memset(curr_listing_buff,0,PATHSIZE);
	memset(curr_listing_argv,0,ARGVMAX*sizeof(char*));
	fgets(curr_listing_buff,PATHSIZE,fstream);
	while(fgets(curr_listing_buff,PATHSIZE,fstream)){
		makeargvdelim(curr_listing_buff," \t\n", curr_listing_argv, FILE_NAME,1);
		
		char full_path_buff_to_test[PATH_MAX*4]={0};
		
		snprintf(full_path_buff_to_test,PATH_MAX*3-1,"%s/%s",dir,curr_listing_argv[FILE_NAME]);
		int is_directory_this_one=isDirectory(full_path_buff_to_test);
		if(is_directory_this_one){
			print_dir_entry(fd, is_directory_this_one,curr_listing_argv, dir);
		}
		else
		{
			char* extension= get_file_extension(curr_listing_argv[FILE_NAME]);
			if(extension){
				if(findInStringArr(excluded_xtensions,extension)<0){
					print_dir_entry(fd, 0,curr_listing_argv, dir);
					
				}
			}
		}
		memset(curr_listing_buff,0,PATHSIZE);
		memset(curr_listing_argv,0,ARGVMAX*sizeof(char*));
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
	
    snprintf(curr_client.og_filepath,sizeof(curr_client.og_filepath)-1,"%s",resolved_abs_path);
	
    build_last_modified_buff(curr_client.og_filepath);
    
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

	snprintf(tmpFile,sizeof(tmpFile)-1,tmpOneDIR_HTML ,dir, request_id);
	replaceStringCharacter(&tmpFile[1],'/','.');
	snprintf(curr_client.relative_file_path,sizeof(curr_client.relative_file_path)-1,tmpTwoDIR_HTML, dir, request_id);
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
		"\n<body class='%s'>\n<br>%s\n\n<div class='%s'>\n<br>\n",dir_listing_css_file_path,DIR_LISTING_BODY_CLASS,FREE_ALL_APPS_COUNTDOWN,DIR_LISTING_BODY_CLASS);
	if(atRoot){

		dprintf(curr_client.fd,"\n<h1 class='%s'>DIRETORIA ROOT DO SERVER!!!!!</h1>\n<br>\n",SITE_PATH_HEADER_CLASS);
	}
	else{
		char prev_dir_rel_path[PATHSIZE*2]={0};
		char prev_dir_to_use[PATHSIZE*2]={0};
		snprintf(prev_dir_rel_path,PATHSIZE*2-1,"%s",dir);
		snprintf(prev_dir_to_use,PATHSIZE*2-1,"%s",dir);
		char* argv_for_slashes[ARGVMAX]={0};
		uint32_t num_tokens=make_str_arr(prev_dir_rel_path,"/",argv_for_slashes,ARGVMAX)-1;
		uint32_t wanted_slash_idx=num_tokens;
		uint64_t ptr_to_path_num=(uint64_t)prev_dir_rel_path;
		uint64_t ptr_to_wanted_slash_num=(uint64_t)argv_for_slashes[wanted_slash_idx];
		uint64_t subtraction_result=ptr_to_wanted_slash_num-ptr_to_path_num;
		prev_dir_to_use[subtraction_result-1]=0;
		dprintf(curr_client.fd,"\n<a class='%s' href='%s'>Prev</a><br><br><br>\n",DIR_REF_CLASS,prev_dir_to_use);

	}
	generate_header_sizes_data(fstream);
	fseek(fstream,0,SEEK_SET);
	print_header(curr_client.fd);
	process_lines_and_print_to_dest_file(curr_client.fd,fstream,dir);
	dprintf(curr_client.fd,"</div>\n</body>\n</html>\n");
	fclose(fstream);

}

