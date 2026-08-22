#include "../Includes/preprocessor.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/resource_consts.h"
#include "../Includes/template_funcs.h"

#define FIND_LISTING_HEADER_CLASS "header_for_find_listing"

#define FIND_LISTING_LINE_CLASS "find_listing_line"

#define FIND_LISTING_DIV_CLASS "find_listing_table"

#define FIND_LISTING_BODY_CLASS "find_listing"

#define DEF_FIND_REXEXP "*"

#define FIND_CMD_TO_USE (exec_find_arr[use_fancy_find])


#define DEF_FIND_ROOTDIR defaultTarget


static const char* find_listing_css_file_path= "/webcrookedstuff/.find_listing_style.css";

u_int64_t num_files_found=0;



static char* regexp_arg = DEF_FIND_REXEXP;


static char aux_buff[PATH_MAX*2]={0},
			aux_exp_buff[PATH_MAX*2]={0},
	curr_listing_buff[4+(PATH_MAX+8)*MAX_NUM_FIND_ARGS]={0};

static char * fancy_find_command[]=

									{"find",
										"-L",
										".",
										"-type",
										"f",
										"-iregex",
										aux_exp_buff,
										NULL
										},

		   * the_normal_person_find_command[]=

									{"find",
										"-L",
										".",
										"-type",
										"f",
										"-iwholename",
										aux_exp_buff,
										NULL
										};

				

argv_type exec_find_arr[2] = { 
								the_normal_person_find_command,
								fancy_find_command
							  };


int isFindRequest(char* resourceTarget){
    char path[PATHSIZE]={0};
	char* ptr=path;
	ptr+=snprintf(ptr,PATHSIZE,"%s",resourceTarget);
	char* query = strchr(path, '?');
	if (query) {
	    *query = 0;
	    query++;
	}
	else{

		query="";
	}
	char* ext=get_file_extension(path);
	if(ext){
		return stringsAreEqual(ext,"find");
			
	}
	else{
		return 0;

	}

}



static void print_dir_entry(int fd, char* curr_listing_arg){
	
	
	dprintf(fd,"\n<span class='%s'>|<a class='%s' href='%s/%s'>%s</a>|</span><br>\n",
			FIND_LISTING_DIV_CLASS,
			FIND_LISTING_LINE_CLASS,
			DEF_FIND_ROOTDIR,
			curr_listing_arg,
			curr_listing_arg
			);
}
static void process_lines_and_print_to_dest_file_find(int fd,FILE* fstream){

	
	memset(curr_listing_buff,0,sizeof(curr_listing_buff));
	while(fgets(curr_listing_buff,sizeof(curr_listing_buff)-1,fstream)){
		curr_listing_buff[strnlen(curr_listing_buff,sizeof(curr_listing_buff))-1]='\0';
		/*
		 * if we have a dot at the beginning AND we have a forward slash there, too
		 * it means that we have a hidden folder!
		 * 
		 * (We have that second condition
		 * In case some bizarro file decides to name themselves
		 * "[nothing].[file extension]")
		 *
		 */
		 
		if((curr_listing_buff[2]=='.')&&strrchr(&curr_listing_buff[2], '/')){
			/*
			 * we memset and skip!
			 */
			memset(curr_listing_buff,0,sizeof(curr_listing_buff));
			continue;
			
		}
		char* extension= get_file_extension(&curr_listing_buff[2]);
		if(extension){
			
			if(findInStringArr(excluded_xtensions,extension)<0){
				
				print_dir_entry(fd, &curr_listing_buff[2]);
				num_files_found++;
				
			}
		}
		else{		
	
			print_dir_entry(fd, &curr_listing_buff[2]);
			num_files_found++;
		}
		memset(curr_listing_buff,0,sizeof(curr_listing_buff));
	}

}
static void generateFindResultPrimitive(char* the_request_parameters){

	regexp_arg=the_request_parameters;

	int outfd= open(tmpDir,O_WRONLY|O_CREAT,0777);
	

    snprintf(aux_buff,sizeof(aux_buff)-1,"%s%s",currDir,DEF_FIND_ROOTDIR);
    replaceStringCharacter(regexp_arg,' ','*');
    snprintf(aux_exp_buff,sizeof(aux_exp_buff)-1,use_fancy_find?"%s":"*%s*",regexp_arg);
    
    chdir(aux_buff);
    if(logging>40){
		
		fprintf(logstream,"Changing into %s directory for find command!\n",aux_buff);
	}
    
	if(logging>40){
		
		fprintf(logstream,"Here is the issued command:\n");
		print_string_arr(logstream,FIND_CMD_TO_USE);
	}
	pid_t pid_fork_find_cmd=fork();
	switch(pid_fork_find_cmd){

			case -1:
					close(outfd);
					exit(-1);
			break;
			case 0:
					dup2(outfd,STDOUT_FILENO);
					close(outfd);
					execvp(FIND_CMD_TO_USE[0],FIND_CMD_TO_USE);
			break;
			default:
					wait(NULL);
			break;

	}
	close(outfd);

}
void generateFindResult(char* the_request_parameters){


	snprintf(tmpFile,sizeof(tmpFile)-1,tmpOneFIND_HTML , request_id);
	replaceStringCharacter(&tmpFile[1],'/','.');
	snprintf(curr_client.relative_file_path,sizeof(curr_client.relative_file_path)-1,tmpTwoFIND_HTML, request_id);
	replaceStringCharacter(&curr_client.relative_file_path[1],'/','.');
	snprintf(tmpDir,sizeof(tmpDir)-1,"%s%s",tmp_file_dir,tmpFile);
	snprintf(curr_client.filepath,sizeof(curr_client.filepath)-1,"%s%s",tmp_file_dir,curr_client.relative_file_path);
	
	curr_client.fd= open(curr_client.filepath,O_WRONLY|O_CREAT,0777);
	if(curr_client.fd<0){
			if(logging){
				fprintf(logstream,"ERRO NO FIND %s\nErro: %s\n",curr_client.filepath,strerror(errno));
			}
			curr_client.fd = -1;
			return;
	}
	else if(logging){
		fprintf(logstream,"Ficheiro criado!\n%s\n",curr_client.filepath);
	}
	generateFindResultPrimitive(the_request_parameters);
	FILE* fstream;
	if(!(fstream=fopen(tmpDir,"r"))){
			if(logging){
				fprintf(logstream,"ERRO NO FIND %s\n",tmpDir);
			}
			close(curr_client.fd);
			curr_client.fd = -1;
			return;
	}
	remove(tmpDir);
	dprintf(curr_client.fd,FIND_HTML_PREFIX
		"\n<body class=\"%s\">\n<br>\n\n\n<div class='%s'>\n<br>\n"
		"\n<h1>The files found on the server with the expression: |%s| </h1>\n<br>\n<br>\n",
			find_listing_css_file_path,
			FIND_LISTING_BODY_CLASS,
			FIND_LISTING_DIV_CLASS,
			aux_exp_buff);
	
	fseek(fstream,0,SEEK_SET);
	process_lines_and_print_to_dest_file_find(curr_client.fd,fstream);
	
	dprintf(curr_client.fd,"\n<h1>%ld files were found! </h1>\n<br>\n<br>\n"
			,num_files_found);
	dprintf(curr_client.fd,"</div>\n</body>\n</html>\n");
	fclose(fstream);

}

