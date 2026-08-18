#include "../Includes/preprocessor.h"
#include <errno.h>
#include "../Includes/buffSizes.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/server_innards.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/io_ops.h"
#include "../Includes/send_resource_func.h"
#include "../Includes/handlecustom.h"
#include "../Includes/sock_ops.h"
#include "../Includes/load_html.h"
#include "../Includes/template_funcs.h"
#include <dirent.h>
#include <limits.h>
#include <sys/wait.h>

#define DEF_FIND_ROOTDIR defaultTarget

#define DEF_FIND_REXEXP "*"

u_int64_t num_files_found=0;

typedef enum {REGEXP,MAX_NUM_FIND_ARGS}find_args;

typedef struct rcon_arg_line{

	char* default_value;
	char* the_actual_value;

}find_arg_line;




static find_arg_line parameters_arr[MAX_NUM_FIND_ARGS]={

	{.default_value=DEF_FIND_REXEXP,NULL}
};


static char aux_buff[PATH_MAX*2]={0},
			aux_exp_buff[PATH_MAX*2]={0},
	curr_listing_buff[4+(PATH_MAX+8)*MAX_NUM_FIND_ARGS]={0};


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


static int make_find_arg_arr(char* the_separated_parameters,find_arg_line arg_arr[],uint64_t max_len){
	char* argv[ARGVMAX]={0};
	int num_tags=make_str_arr(the_separated_parameters,"|",argv,max_len);
	if(num_tags<=0){

		return -1;
	}
	argv[max_len-2][strlen(argv[max_len-2])-1]=0;
	for(uint64_t i=0;i<max_len-1;i++){
		arg_arr[i].the_actual_value=((strlen(argv[i]))?argv[i]:arg_arr[i].default_value);
	}
	return 0;
}

static void print_dir_entry(int fd, char* curr_listing_arg){
	
	
	dprintf(fd,"\n<a href='%s/%s'>%s</a><br>\n",
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



	int outfd= open(tmpDir,O_WRONLY|O_CREAT,0777);
	
	make_find_arg_arr(the_request_parameters,parameters_arr,MAX_NUM_FIND_ARGS+1);

    snprintf(aux_buff,sizeof(aux_buff)-1,"%s%s",currDir,DEF_FIND_ROOTDIR);
    replaceStringCharacter(parameters_arr[REGEXP].the_actual_value,' ','*');
    snprintf(aux_exp_buff,sizeof(aux_exp_buff)-1,"*%s*",parameters_arr[REGEXP].the_actual_value);
    
    chdir(aux_buff);
    if(logging>40){
		
		fprintf(logstream,"Changing into %s directory for find command!\n",aux_buff);
	}
    
    char* the_find_command[]={"find","-L",".","-type","f","-iwholename",aux_exp_buff,NULL};
	if(logging>40){
		
		fprintf(logstream,"Here is the issued command:\n");
		print_string_arr(logstream,the_find_command);
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
					execvp(the_find_command[0],the_find_command);
			break;
			default:
					wait(NULL);
			break;

	}
	close(outfd);

}
void generateFindResult(char* the_request_parameters){

	logging=1;
    logstream=stderr;

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
		"\n<body>\n<br>\n\n<br>\n");
	dprintf(curr_client.fd,"\n<h1>The files found on the server with the expression: |*%s*| </h1>\n<br>\n<br>\n"
			,parameters_arr[REGEXP].the_actual_value);
	
	fseek(fstream,0,SEEK_SET);
	process_lines_and_print_to_dest_file_find(curr_client.fd,fstream);
	
	dprintf(curr_client.fd,"\n<h1>%lu files were found! </h1>\n<br>\n<br>\n"
			,num_files_found);
	dprintf(curr_client.fd,"</div>\n</body>\n</html>\n");
	fclose(fstream);

}

