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
#include "../Includes/handlecustom.h"
#include "../Includes/sock_ops.h"
#include "../Includes/session_ops.h"
#include "../Includes/rcon_proxy.h"
#include <sys/wait.h>

typedef enum {ADDR=0,PORT,PASS,CMD,MAX_NUM_RCON_ARGS}rcon_args;

typedef struct rcon_arg_line{

	const char* default_value;
	char* the_actual_value;

}rcon_arg_line;


#define DEF_RCON_ADDRESS "192.168.0.100"

#define DEF_RCON_PORT "27015"

#define DEF_RCON_PASS "SATAANDAGI"

#define DEF_RCON_CMD "stats"


static char* tmpOne="/.tmp.rcon",* tmpTwo="/.tmp1.rcon";

static char* the_rcon_script="/webcrookedstuff/cstrike_folder/scripts/.send_command.sh";

static char tmpDir[PATHSIZE*2]={0},tmpDir2[PATHSIZE*2]={0},currFetchedRCON[PATHSIZE*2]={0};


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
		if(logging){
			fprintf(logstream,"Some request of sorts!\nThe request: %s\n",query);
		}
	}
	else{

		query="";
	}
	char* ext=get_file_extension(path);
	if(ext){
		if(stringsAreEqual(ext,"rcon")){
			if(logging){
				fprintf(logstream,"RCON Request pedido!!: %s (RCON)\n",path);
			}
			return 1;
		}
		else{
			if(logging){
				fprintf(logstream,"Nepia... não é rcon...: %s (RCON)\n",path);
			}
			return 0;

		}
	}
	else{
		if(logging){
			fprintf(logstream,"Nepia... não é rcon...: %s (RCON)\n",path);
		}
		return 0;

	}

}

static int make_rcon_arg_arr(char* the_separated_parameters,rcon_arg_line arg_arr[],uint64_t max_len){
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


//static void generateRconResultPrimitive(char* path){
static void generateRconResultPrimitive(char* the_separated_parameters){

	make_rcon_arg_arr(the_separated_parameters,parameters_arr,MAX_NUM_RCON_ARGS+1);

	int outfd= open(tmpDir,O_TRUNC|O_WRONLY|O_CREAT,0777);

        snprintf(currFetchedRCON,PATHSIZE*2-1,"%s%s",currDir,the_rcon_script);

        char* args_cmd[]={"bash",currFetchedRCON,
						parameters_arr[ADDR].the_actual_value,
						parameters_arr[PORT].the_actual_value,
						parameters_arr[PASS].the_actual_value,
						parameters_arr[CMD].the_actual_value,
						NULL};
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
char* generateRconResult(char* dir,char* the_request_parameters){

	logging=1;
        logstream=stderr;

	snprintf(tmpDir,PATHSIZE*2-1,"%s%s",currDir,tmpOne);

        snprintf(tmpDir2,PATHSIZE*2-1,"%s%s",currDir,tmpTwo);

        int fd= open(tmpDir2,O_TRUNC|O_WRONLY|O_CREAT,0777);
        if(fd<0){
                if(logging){
                fprintf(logstream,"ERRO NO RCON|!|\nErro: %s\n",strerror(errno));
                }
                return NULL;
        }
        generateRconResultPrimitive(the_request_parameters);
        FILE* fstream;
        if(!(fstream=fopen(tmpDir,"r"))){
                if(logging){
                fprintf(logstream,"ERRO NO RCON %s\n",tmpDir);
                }
                close(fd);
                return NULL;
        }
        remove(tmpDir);
	dprintf(fd,"<!DOCTYPE html>"
			"\n<html>"
			"\n<head>"
			"\n<base href=''>"
			"\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
			"\n</head>\n<body>\n<pre>\n");

	char currListing[PATHSIZE*2]={0};
	while(fgets(currListing,PATHSIZE,fstream)){
		dprintf(fd,"%s",currListing);
	}
	dprintf(fd,"</pre>\n</body>\n</html>\n");
	fclose(fstream);
	close(fd);
	return tmpTwo;

}

void deleteRconResult(void){
char buff[PATHSIZE*2]={0};
snprintf(buff,PATHSIZE*2,"%s%s",currDir,tmpTwo);
remove(buff);

}
