#include "../Includes/preprocessor.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/resource_consts.h"
#include "../Includes/request_consts.h"
#include "../Includes/template_funcs.h"

#define RCON_BODY_CLASS "rcon_body"

#define RCON_FRAMING_CLASS "rcon_framing"

#define RCON_PRE_CLASS "rcon_pre"

#define DEF_RCON_ADDRESS "192.168.0.100"

#define DEF_RCON_PORT "27015"

#define DEF_RCON_PASS "SATAANDAGI"

#define DEF_RCON_CMD "stats"



static char currFetchedRCON[PATHSIZE*3]={0};


static arg_line parameters_arr[MAX_NUM_RCON_ARGS]={

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


static void generateRconResultPrimitive(char* the_separated_parameters){

	build_arg_line_arr(the_separated_parameters,parameters_arr,MAX_NUM_RCON_ARGS);

	int outfd= open(tmpDir,O_WRONLY|O_CREAT,0777);
		
	snprintf(currFetchedRCON,sizeof(currFetchedRCON)-1,"%s" RCON_SCRIPT_PATH, currDir,defaultTarget);
	
	int stat_result = check_stat_of_file(currFetchedRCON,curr_client.the_stat_struct,logging,logstream);
	if(stat_result<0){
		if(logging){
		fprintf(logstream,"ERRO NO RCON SENDING %s\nErro: %s\n",
														tmpDir,
														strerror(errno));
		}
		close(curr_client.fd);
		curr_client.fd=-1;
		return;
		
	}
	
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

	const char* first_format = get_fetch_parser_handler_first_format(curr_client.fetch_request_table_cell),
		* second_format = get_fetch_parser_handler_second_format(curr_client.fetch_request_table_cell);
	
	snprintf(tmpFile,sizeof(tmpFile)-1,first_format, request_id);
	replaceStringCharacter(&tmpFile[1],'/','.');
	snprintf(curr_client.relative_file_path,sizeof(curr_client.relative_file_path)-1,second_format, request_id);
	replaceStringCharacter(&curr_client.relative_file_path[1],'/','.');
	snprintf(tmpDir,sizeof(tmpDir)-1,"%s%s",tmp_file_dir,tmpFile);
	snprintf(curr_client.filepath,sizeof(curr_client.filepath)-1,"%s%s",tmp_file_dir,curr_client.relative_file_path);
	
	curr_client.fd= open(curr_client.filepath,O_WRONLY|O_CREAT,0777);
	if(curr_client.fd <0){
			if(logging){
			fprintf(logstream,"ERRO NO RCON!\nErro: %s\n",strerror(errno));
			}
			curr_client.fd = -1;
			return;
	}
	else if(logging){
		fprintf(logstream,"Ficheiro criado!\n%s\n",curr_client.filepath);
	}
	generateRconResultPrimitive(the_request_parameters);
	
	if(curr_client.fd<0){
		return;
	}
	
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
	dprintf(curr_client.fd ,RCON_HTML_PREFIX "\n<body class=\"%s\">\n<div class=\"%s\">\n<br><pre  class=\"%s\">\n",defaultTarget,defaultTarget,RCON_BODY_CLASS,RCON_FRAMING_CLASS,RCON_PRE_CLASS);

	char currListing[PATHSIZE*2]={0};
	while(fgets(currListing,PATHSIZE,fstream)){
		dprintf(curr_client.fd,"%s",currListing);
	}
	dprintf(curr_client.fd,"</pre>\n<br></div>\n</body>\n</html>\n");
	fclose(fstream);
	lseek(curr_client.fd,0,SEEK_SET);

}
