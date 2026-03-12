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
#include "../Includes/send_resource_func.h"
#include "../Includes/handlecustom.h"
#include "../Includes/sock_ops.h"
#include "../Includes/session_ops.h"
#include "../Includes/load_html.h"
#include <limits.h>
#include <sys/wait.h>

#define SITE_PATH_HEADER_CLASS "header_for_site_path"

#define DIR_LISTING_HEADER_CLASS "header_for_dir_listing"

#define PAGE_DEBUG_HEADER_CLASS "header_for_page_debug"

#define DIR_LISTING_BODY_CLASS "dir_listing"

#define DIR_REF_CLASS "ref_for_dir"

#define FILE_REF_CLASS "ref_for_file"



#define DIR_LISTING_FORMAT ("\n<a class='%s' href='%s/%s'>%s</a> <h1>%d<h1>\n",get_file_extension(curr_listing_buff)?"ref_for_file":"ref_for_dir")

#define NUM_OF_FIELDS_TO_PRINT 2

typedef enum {PERMISSIONS,NUM_LINKS,OWNER_NAME,OWNER_GROUP,SIZE,DATE_MONTH,DATE_DAY,DATE_DAY_TIME,FILE_NAME,CATEGORY_ID_MAX}category_id;



static char* category_names[CATEGORY_ID_MAX]={
			"Permissions",
			"num_links",
			"Owner_name",
			"Owner_group",
			"Size",
			"Date_month",
			"Date_day",
			"Date_day_time",
			"Actual_file_name"

};

static category_id print_order[NUM_OF_FIELDS_TO_PRINT]={
	FILE_NAME,
	SIZE
};

static uint16_t category_max_sizes[CATEGORY_ID_MAX]={0};
static uint16_t category_curr_sizes[CATEGORY_ID_MAX]={0};

static char* FREE_ALL_APPS_COUNTDOWN = "<script src=\"/webcrookedstuff/.keep_android_open_please.js\"></script>";

static char* tmpOne="/.tmp.html",* tmpTwo="/.tmp1.html";

static const char* dir_listing_css_file_path= "/webcrookedstuff/.dir_listing_style.css";

static char tmpDir[PATHSIZE*2]={0},tmpDir2[PATHSIZE*2]={0},currSearchedDir[PATHSIZE*2]={0};
static char resolved_abs_path[PATH_MAX]={0};
static char resolved_rel_path[PATH_MAX]={0};
static char resolved_tmp_one_path[PATH_MAX]={0};
static char resolved_tmp_two_path[PATH_MAX]={0};
static char header_buff[4+(PATHSIZE+4)*CATEGORY_ID_MAX]={0};
static char* curr_listing_argv[ARGVMAX]={0};
static char* header_argv[ARGVMAX]={0};
static char curr_listing_buff[4+(PATHSIZE+8)*CATEGORY_ID_MAX]={0};



void generate_header_sizes_data(FILE* fstream){

	memset(curr_listing_buff,0,PATHSIZE);
	fgets(curr_listing_buff,PATHSIZE,fstream);
	while(fgets(curr_listing_buff,PATHSIZE,fstream)){
		memset(header_argv,0,ARGVMAX*sizeof(char*));
		makeargv(curr_listing_buff, header_argv);
		for(uint32_t j=0;j<NUM_OF_FIELDS_TO_PRINT;j++){
			uint32_t i=print_order[j];
			//printf("The indexes: i= %u, j= %u\nheader_argv[i]= %s\n",i,j,header_argv[i]);
			category_curr_sizes[i]=header_argv[i]?strnlen(header_argv[i],PATHSIZE-1):0;
			if(category_curr_sizes[i]>category_max_sizes[i]){
				category_max_sizes[i]=max(strlen(category_names[i]),min(PATHSIZE,category_curr_sizes[i]));
			}
		}
		memset(curr_listing_buff,0,PATHSIZE);
	}

}
void print_header(int fd){

	memset(header_buff,0,4+(PATHSIZE+4)*CATEGORY_ID_MAX);
	char* curr_ptr=header_buff;
	for(uint32_t j=0;j<NUM_OF_FIELDS_TO_PRINT;j++){
		uint32_t i=print_order[j];
		char curr_printed_category_header_buff[category_max_sizes[i]+PATHSIZE];
		memset(curr_printed_category_header_buff,0,category_max_sizes[i]+PATHSIZE);
		/*char spaces_buff[PATHSIZE*strlen("&nbsp")];
		memset(spaces_buff,0,PATHSIZE*strlen("&nbsp"));
		char* ptr=spaces_buff;
//		printf("%lu, %d, %lu, %s\n",category_max_sizes[FILE_NAME]-strlen(curr_listing_argv[FILE_NAME]),category_max_sizes[FILE_NAME],strlen(curr_listing_argv[FILE_NAME]),curr_listing_argv[FILE_NAME]);
		for(uint32_t k=0;k<category_max_sizes[i]-strlen(category_names[i]);k++){

			ptr+=snprintf(ptr,PATHSIZE-1," ");

		}*/
		snprintf(curr_printed_category_header_buff,PATHSIZE,"| %s %d  ",category_names[i],category_max_sizes[i]);
		curr_ptr+=snprintf(curr_ptr,PATHSIZE-1,"%s",curr_printed_category_header_buff);

	}
	dprintf(fd,"\n<h2 class='%s'><span>%s|</span></h2>\n",DIR_LISTING_HEADER_CLASS,header_buff);

}
void process_lines_and_print_to_dest_file(int fd,FILE* fstream, char* dir){


	memset(curr_listing_buff,0,PATHSIZE);
	memset(curr_listing_argv,0,ARGVMAX*sizeof(char*));
	fgets(curr_listing_buff,PATHSIZE,fstream);
	while(fgets(curr_listing_buff,PATHSIZE,fstream)){
		makeargv(curr_listing_buff, curr_listing_argv);
		/*char spaces_buff[PATHSIZE*strlen("&nbsp")];
		memset(spaces_buff,0,PATHSIZE*strlen("&nbsp"));
		char* ptr=spaces_buff;
//		printf("%lu, %d, %lu, %s\n",category_max_sizes[FILE_NAME]-strlen(curr_listing_argv[FILE_NAME]),category_max_sizes[FILE_NAME],strlen(curr_listing_argv[FILE_NAME]),curr_listing_argv[FILE_NAME]);
		for(uint32_t i=0;i<category_max_sizes[FILE_NAME]-strlen(curr_listing_argv[FILE_NAME]);i++){

			ptr+=snprintf(ptr,PATHSIZE-1,"&nbsp");

		}
		*/
		char full_path_buff_to_test[PATH_MAX*4]={0};
		snprintf(full_path_buff_to_test,PATH_MAX*3-1,"%s/%s",dir,curr_listing_argv[FILE_NAME]);
		int is_directory_this_one=isDirectory(full_path_buff_to_test);
		dprintf(fd,"\n| <a class='%s' href='%s/%s'>%s</a> %s |<br>\n",is_directory_this_one?DIR_REF_CLASS:FILE_REF_CLASS,dir,curr_listing_argv[FILE_NAME],curr_listing_argv[FILE_NAME],is_directory_this_one?"directory!":curr_listing_argv[SIZE]);
		memset(curr_listing_buff,0,PATHSIZE);
		memset(curr_listing_argv,0,ARGVMAX*sizeof(char*));
	}

}
void resolve_canonical_path(char raw_path[PATH_MAX],char dst_for_resolved_path[PATH_MAX]){

	if (realpath(raw_path, dst_for_resolved_path))
	{
		printf("Canonical path: de %s:\n%s\n",raw_path, dst_for_resolved_path);
	}
	else
	{
                if(logging){
	                fprintf(logstream,"Canonical path (relative) de %s?\nErro: %s\n",raw_path,strerror(errno));
                }
	}
}
void resolve_canonical_path_relative(char raw_path[PATH_MAX],char dst_for_resolved_path[PATH_MAX]){
	char new_raw_path[PATH_MAX]={0};
	snprintf(new_raw_path,PATH_MAX-1,".%s",raw_path);

	if (realpath(new_raw_path, dst_for_resolved_path))
	{
		printf("Canonical path (relative): de %s:\n%s\n",new_raw_path, dst_for_resolved_path);
	}
	else
	{
                if(logging){
	                fprintf(logstream,"Canonical path (relative) de %s?\nErro: %s\n",new_raw_path,strerror(errno));
                }
	}
}

static void generateDirListingPrimitive(char* path){



	int outfd= open(tmpDir,O_TRUNC|O_WRONLY|O_CREAT,0777);
        snprintf(currSearchedDir,PATHSIZE*2-1,"%s%s",currDir,path);

	resolve_canonical_path(currSearchedDir,resolved_abs_path);

        char* args_cmd[]={"ls","-lhu", "--group-directories-first",resolved_abs_path,NULL};
        pid_t pid_fork_find_cmd=fork();
        switch(pid_fork_find_cmd){

                case -1:
                        close(outfd);
                        exit(-1);
                break;
                case 0:
                        dup2(outfd,STDOUT_FILENO);
                        close(outfd);
                        execvp(args_cmd[0],args_cmd);
                break;
                default:
                        wait(NULL);
                break;

        }
        chdir("..");
	close(outfd);

}
char* generateDirListing(char* dir){

	logging=1;
        logstream=stderr;

	resolve_canonical_path_relative(dir,resolved_rel_path);
	int pathsize0=strlen(defaultTarget);
	int pathsize=strlen(dir);
	int pathsize2=strlen(resolved_rel_path);
	if(pathsize&&!strcmp(dir +pathsize-1,"/")){

		dir[pathsize-1]=0;
	}
	if(pathsize2&&!strcmp(resolved_rel_path +pathsize2-1,"/")){

		resolved_rel_path[pathsize2-1]=0;
	}
	char* test_str1=&resolved_rel_path[pathsize0+1];
	char* test_str2="..";
	int noRoot=strcmp(resolved_rel_path,defaultTarget)/*||!stringsAreEqual(test_str1,test_str2)*/;

        snprintf(tmpDir,PATHSIZE*2-1,"%s%s",currDir,tmpOne);
	resolve_canonical_path(tmpDir,resolved_tmp_one_path);

        snprintf(tmpDir2,PATHSIZE*2-1,"%s%s",currDir,tmpTwo);
	resolve_canonical_path(tmpDir2,resolved_tmp_two_path);

        int fd= open(resolved_tmp_two_path,O_TRUNC|O_WRONLY|O_CREAT,0777);
        if(fd<0){
                if(logging){
                fprintf(logstream,"ERRO NAS DIRETORIAS %s\nErro: %s\n",tmpDir2,strerror(errno));
                }
                return NULL;
        }
        generateDirListingPrimitive(noRoot?dir:defaultTarget);
        FILE* fstream;
        if(!(fstream=fopen(resolved_tmp_one_path,"r"))){
                if(logging){
                fprintf(logstream,"ERRO NAS DIRETORIAS %s\n",tmpDir);
                }
                close(fd);
                return NULL;
        }
        remove(resolved_tmp_one_path);
	dprintf(fd,"<!DOCTYPE html>"
			"\n<html>"
			"\n<head>"
			"\n<base href=''>"
			"\n<link rel='stylesheet' type='text/css' href='%s'>"
			"\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
			"\n</head>"
			"\n<body class='%s'>\n<br>%s\n<br>\n",dir_listing_css_file_path,DIR_LISTING_BODY_CLASS,FREE_ALL_APPS_COUNTDOWN);
	dprintf(fd,"\n<br>"
			"\n<h1 class='%s'><span>INDEX OF: %s</span></h2>\n<h1 class='%s'><span>INDEX  2 OF: %s</span></h1>"
			"<br>dir: %s\n"
			"<br>test str: %s\n"
			"<br>Strlen of dir: %lu\n"
			"<br>Strlen of test str: %lu\n"
			"<br>Result of stringsAreEqual: %d\n"
			"<br>Result of stringsAreEqualN with length 4: %d<br><br>\n",SITE_PATH_HEADER_CLASS,dir,PAGE_DEBUG_HEADER_CLASS,&dir[pathsize0],test_str1,test_str2,strlen(test_str1),strlen(test_str2),stringsAreEqual(test_str1,test_str2),stringsAreEqualN(test_str1,test_str2,4));
	if(!noRoot){

		dprintf(fd,"\n<h1 class='%s'>DIRETORIA ROOT DO SERVER!!!!!</h1>",SITE_PATH_HEADER_CLASS);
	}
	else{

		dprintf(fd,"\n<a class='%s' href='%s/..'>Prev</a><br><br><br>\n",DIR_REF_CLASS,dir);

	}
	generate_header_sizes_data(fstream);
	fseek(fstream,0,SEEK_SET);
	print_header(fd);
	process_lines_and_print_to_dest_file(fd,fstream,dir);
	dprintf(fd,"</body>\n</html>\n");
	fclose(fstream);
	close(fd);
	return tmpTwo;

}

void deleteDirListingHTML(void){
char buff[PATHSIZE*2]={0};
snprintf(buff,PATHSIZE*2,"%s%s",currDir,tmpTwo);
remove(buff);

}
