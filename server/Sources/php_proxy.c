#include "../Includes/preprocessor.h"
#include <errno.h>
#include "../Includes/buffSizes.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/server_innards.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/io_ops.h"
#include "../Includes/handlecustom.h"
#include "../Includes/sock_ops.h"
#include "../Includes/php_proxy.h"
#include <sys/wait.h>




static char* phpCmd = "php-cgi8.3 -f %s";
static char* query=NULL;



static char currFetchedPHP[PATHSIZE*2]={0},
		full_php_target[PATHSIZE*2]={0},
		full_php_root[PATHSIZE*2]={0},
		full_php_command[PATHSIZE*2]={0};


int isPHPRequest(char* resourceTarget){
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
		if(stringsAreEqual(ext,"php")||stringsAreEqual(ext,"php2")){
			if(logging){
				fprintf(logstream,"PHP request pedido!!: %s (PHP)\n",path);
			}
			return 1;
		}
		else{
			if(logging){
				fprintf(logstream,"Nepia... não é php ou php2...: %s (PHP)\n",path);
			}
			return 0;

		}
	}
	else{
		if(logging){
			fprintf(logstream,"Nepia... não é php ou php2...: %s (PHP)\n",path);
		}
		return 0;

	}

}

void generatePHPResultPrimitive(char* target){

	int outfd= open(tmpDir,O_WRONLY|O_CREAT,0777);
        snprintf(currFetchedPHP,sizeof(currFetchedPHP)-1,"%s%s",currDir,target);

        snprintf(full_php_target,sizeof(full_php_target)-1,"%s%s",target,query);

        snprintf(full_php_root,sizeof(full_php_root)-1,"%s/",defaultTarget);


	printf("(PHP) Output file path: %s\n",currFetchedPHP);

	snprintf(full_php_command,sizeof(full_php_command)-1,phpCmd,currFetchedPHP);
	printf("%s\n",full_php_command);
	char* argv[ARGVMAX]={"php-cgi8.3",NULL};
	int pid=fork();
	switch(pid){
		case -1:
			close(outfd);
			return;
		case 0:
			dup2(outfd,1);
			close(outfd);
			setenv("QUERY_STRING", query, 1);
			setenv("REQUEST_METHOD", "GET", 1);
			setenv("SCRIPT_FILENAME", currFetchedPHP, 1);
			setenv("REDIRECT_STATUS", "200", 1);
			setenv("HTTP_X_REQUESTED_WITH", "XMLHttpRequest", 1);
			setenv("REQUEST_URI", full_php_target, 1);
			setenv("APP_ROOT_PATH", full_php_root, 1);
			setenv("BASE_DIR_PATH_ABSOLUTE", currDir, 1);
			execvp(argv[0],argv);
			perror("exec na compressao\n");
			exit(-1);

		default:
			wait(NULL);
			break;


	}
        chdir("..");
	close(outfd);
}



void generatePHPResult(char* request){

	query = strchr(request, '?');
	if (query) {
	    *query = 0;
	    query++;
		if(logging){
			fprintf(logstream,"PHP! Query string: %s\n!!!!",query);
		}
	}
	else{

		query="";
	}

	logging=1;
        logstream=stderr;
        
	snprintf(tmpFile,sizeof(tmpFile)-1,tmpOnePHP_HTML , request, request_id);
	replaceStringCharacter(&tmpFile[1],'/','.');
	snprintf(curr_client.relative_file_path,sizeof(curr_client.relative_file_path)-1,tmpTwoPHP_HTML , request, request_id);
	replaceStringCharacter(&curr_client.relative_file_path[1],'/','.');
	snprintf(tmpDir,sizeof(tmpDir)-1,"%s%s",tmp_file_dir,tmpFile);
	snprintf(curr_client.filepath,sizeof(curr_client.filepath)-1,"%s%s",tmp_file_dir,curr_client.relative_file_path);
	
	curr_client.fd= open(curr_client.filepath,O_WRONLY|O_CREAT,0777);
	if(curr_client.fd <0){
			if(logging){
			fprintf(logstream,"ERRO NO PHP PROXY %s\nErro: %s\n",curr_client.filepath,strerror(errno));
			}
			curr_client.fd = -1;
			return;
	}
	else if(logging){
		fprintf(logstream,"Ficheiro criado!\n%s\n",curr_client.filepath);
	}
	generatePHPResultPrimitive(request);
	FILE* fstream;
	if(!(fstream=fopen(tmpDir,"r"))){
			if(logging){
			fprintf(logstream,"ERRO NO PHP PROXY %s\n",tmpDir);
			}
			close(curr_client.fd);
			curr_client.fd = -1;
			return;
	}
	remove(tmpDir);

	char currListing[PATHSIZE*2]={0};
	int seen_blank = 0;
	while(fgets(currListing,PATHSIZE,fstream)){
		if (!seen_blank) {
			if (
				strcmp(currListing, "\n") == 0 ||
				strcmp(currListing, "\r\n") == 0
			) {
				seen_blank = 1;
			}
			continue;
		}
		dprintf(curr_client.fd, "%s", currListing);
	}
	fclose(fstream);
	lseek(curr_client.fd,0,SEEK_SET);

}

