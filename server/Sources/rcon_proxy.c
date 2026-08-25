#include "../Includes/preprocessor.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/resource_consts.h"



#define DEF_RCON_ADDRESS "192.168.0.100"

#define DEF_RCON_PORT "27015"

#define DEF_RCON_PASS "SATAANDAGI"

#define DEF_RCON_CMD "stats"



static char* the_rcon_script="/webcrookedstuff/cstrike_folder/scripts/.send_command.sh";

static char currFetchedRCON[PATHSIZE*2]={0};


static rcon_arg_line parameters_arr[MAX_NUM_RCON_ARGS]={

	{.default_value=DEF_RCON_ADDRESS,NULL},

	{.default_value=DEF_RCON_PORT,NULL},

	{.default_value=DEF_RCON_PASS,NULL},

	{.default_value=DEF_RCON_CMD,NULL}
};



int isRconRequest(char* resourceTarget){
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
		return stringsAreEqual(ext,"rcon");
	}
	else{
		return 0;

	}

}

static int make_rcon_arg_arr(char* the_separated_parameters,rcon_arg_line arg_arr[],uint64_t max_len){
	char* argv[ARGVMAX]={0};
	int num_tags=make_str_arr(the_separated_parameters,"|",argv,max_len);
	if(num_tags<=0){

		return -1;
	}
	
	if(max_len - 2 > 0){
		argv[max_len-2][strlen(argv[max_len-2])-1]=0;
	}
	for(uint64_t i=0;i<max_len-1;i++){
		arg_arr[i].the_actual_value=((strlen(argv[i]))?argv[i]:arg_arr[i].default_value);
	}
	return 0;
}


static void generateRconResultPrimitive(char* the_separated_parameters){

	make_rcon_arg_arr(the_separated_parameters,parameters_arr,MAX_NUM_RCON_ARGS+1);

	int outfd= open(tmpDir,O_WRONLY|O_CREAT,0777);
		
        snprintf(currFetchedRCON,sizeof(currFetchedRCON)-1,"%s%s", currDir,the_rcon_script);

        char* args_cmd[]={"bash",currFetchedRCON,
						parameters_arr[ADDR].the_actual_value,
						parameters_arr[PORT].the_actual_value,
						parameters_arr[PASS].the_actual_value,
						parameters_arr[CMD].the_actual_value,
						NULL};
		if(logging>6){
			
			print_string_arr(logstream,args_cmd);
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
                        execvp(args_cmd[0],args_cmd);
                break;
                default:
                        wait(NULL);
                break;

        }
        close(outfd);

}
void generateRconResult(char* the_request_parameters){

	logging=1;
    logstream=stderr;

	snprintf(tmpFile,sizeof(tmpFile)-1,tmpOneRCON_HTML, request_id);
	replaceStringCharacter(&tmpFile[1],'/','.');
	snprintf(curr_client.relative_file_path,sizeof(curr_client.relative_file_path)-1,tmpTwoRCON_HTML, request_id);
	replaceStringCharacter(&curr_client.relative_file_path[1],'/','.');
	snprintf(tmpDir,sizeof(tmpDir)-1,"%s%s",tmp_file_dir,tmpFile);
	snprintf(curr_client.filepath,sizeof(curr_client.filepath)-1,"%s%s",tmp_file_dir,curr_client.relative_file_path);
	
	curr_client.fd= open(curr_client.filepath,O_WRONLY|O_CREAT,0777);
	if(curr_client.fd <0){
			if(logging){
			fprintf(logstream,"ERRO NO RCON|!|\nErro: %s\n",strerror(errno));
			}
			curr_client.fd = -1;
			return;
	}
	else if(logging){
		fprintf(logstream,"Ficheiro criado!\n%s\n",curr_client.filepath);
	}
	generateRconResultPrimitive(the_request_parameters);
	FILE* fstream;
	if(!(fstream=fopen(tmpDir,"r"))){
			if(logging){
			fprintf(logstream,"ERRO NO RCON %s\n",tmpDir);
			}
			close(curr_client.fd);
			curr_client.fd = -1;
			return;
	}
	remove(tmpDir);
	dprintf(curr_client.fd ,"<!DOCTYPE html>"
			"\n<html>"
			"\n<head>"
			"\n<base href=''>"
			"\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
			"\n</head>\n<body>\n<pre>\n");

	char currListing[PATHSIZE*2]={0};
	while(fgets(currListing,PATHSIZE,fstream)){
		dprintf(curr_client.fd,"%s",currListing);
	}
	dprintf(curr_client.fd,"</pre>\n</body>\n</html>\n");
	fclose(fstream);
	lseek(curr_client.fd,0,SEEK_SET);

}
