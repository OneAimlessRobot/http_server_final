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

#define tmpOne "%s.tmp.php"
#define tmpTwo "%s.tmp1.php"

static char tmpFile[PATHSIZE*3]={0};
static char tmpFile2[PATHSIZE*3]={0};

static char tmpDir[PATHSIZE*2]={0},
		tmpDir2[PATHSIZE*2]={0},
		currFetchedPHP[PATHSIZE*2]={0},
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

	int outfd= open(tmpDir,O_TRUNC|O_WRONLY|O_CREAT,0777);
        snprintf(currFetchedPHP,PATHSIZE*2-1,"%s%s",currDir,target);

        snprintf(full_php_target,PATHSIZE*2-1,"%s%s",target,query);

        snprintf(full_php_root,PATHSIZE*2-1,"%s/",defaultTarget);


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



char* generatePHPResult(char* request){

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
        
	snprintf(tmpFile,PATHSIZE*3-1,tmpOne,request);
	snprintf(tmpFile2,PATHSIZE*3-1,tmpTwo,request);
	
	snprintf(tmpDir,PATHSIZE*4-1,"%s%s",currDir,tmpFile);
	snprintf(tmpDir2,PATHSIZE*4-1,"%s%s",currDir,tmpFile2);
	
        int fd= open(tmpDir2,O_TRUNC|O_WRONLY|O_CREAT,0777);
        if(fd<0){
                if(logging){
                fprintf(logstream,"ERRO NO PHP PROXY %s\nErro: %s\n",tmpDir2,strerror(errno));
                }
                return NULL;
        }
        generatePHPResultPrimitive(request);
        FILE* fstream;
        if(!(fstream=fopen(tmpDir,"r"))){
                if(logging){
                fprintf(logstream,"ERRO NO PHP PROXY %s\n",tmpDir);
                }
                close(fd);
                return NULL;
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
		dprintf(fd, "%s", currListing);
	}
	fclose(fstream);
	close(fd);
	return tmpFile2;

}

void deletePHPResult(char* request){
char buff[PATHSIZE*3]={0};
snprintf(buff,PATHSIZE*3-1,tmpTwo,request);
snprintf(tmpDir2,PATHSIZE*4-1,"%s%s",currDir,buff);
remove(tmpDir2);

}
