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
#include "../Includes/txt_sending.h"
#include "../Includes/template_funcs.h"
#include <sys/wait.h>

#define TXT_PRETTIFY_BODY_CLASS "txt_prettify_body"
#define TXT_PRETTIFY_PRE_CLASS TXT_PRETTIFY_BODY_CLASS
#define TXT_PRETTIFY_DIV_CLASS TXT_PRETTIFY_BODY_CLASS

#define tmpOne "%s.tmp%lx.txt"
#define tmpTwo "%s.tmp1%lx.txt"


    
static const char* txt_prettify_css_file_path= "/webcrookedstuff/.styles_file_exts/.stylesheet_txt_files.css";

static char currFetchedText[PATHSIZE*2]={0};




int isText(char* resourceTarget){
        char path[PATHSIZE]={0};
	char* ptr=path;
	ptr+=snprintf(ptr,PATHSIZE,"%s%s",currDir,resourceTarget);
	char* ext=get_file_extension(path);
	if(ext){
		if(stringsAreEqual(ext,"txt")){
			if(logging){
				fprintf(logstream,"Texto pedido!!: %s (TXT)\n",path);
			}
			return 1;
		}
		else{
			if(logging){
				fprintf(logstream,"Nepia... não é texto...: %s (TXT)\n",path);
			}
			return 0;

		}
	}
	else{
		if(logging){
			fprintf(logstream,"Nepia... não é texto...: %s (TXT)\n",path);
		}
		return 0;

	}

}

static void generateTextPrimitive(char* path){

	int outfd= open(tmpDir,O_TRUNC|O_WRONLY|O_CREAT,0777);
        snprintf(currFetchedText,PATHSIZE*2-1,"%s%s",currDir,path);

        char* args_cmd[]={"cat", currFetchedText,NULL};
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



char* generateText(char* dir){

	logging=1;
	logstream=stderr;
	snprintf(tmpFile,PATHSIZE*3-1,tmpOne,dir, request_id);
	snprintf(tmpFile2,PATHSIZE*3-1,tmpTwo,dir, request_id);
	
	snprintf(tmpDir,PATHSIZE*4-1,"%s%s",currDir,tmpFile);
	snprintf(tmpDir2,PATHSIZE*4-1,"%s%s",currDir,tmpFile2);
	int fd= open(tmpDir2,O_TRUNC|O_WRONLY|O_CREAT,0777);
	if(fd<0){
			if(logging){
			fprintf(logstream,"ERRO NAS DIRETORIAS %s\nErro: %s\n",tmpDir2,strerror(errno));
			}
			return NULL;
	}
	generateTextPrimitive(dir);
	FILE* fstream;
	if(!(fstream=fopen(tmpDir,"r"))){
			if(logging){
			fprintf(logstream,"ERRO NO RSS FEED %s\n",tmpDir);
			}
			close(fd);
			return NULL;
	}
	remove(tmpDir);
	int pathsize=strlen(dir);
	if(pathsize&&!strcmp(dir +pathsize-1,"/")){

		dir[pathsize-1]=0;
	}
	
	
	dprintf(fd,TXT_HTML_PREFIX
		"\n<body class='%s'>\n<br>\n<br>\n\n<br>\n<br><div class='%s'>\n<pre class='%s'>\n\n",txt_prettify_css_file_path,TXT_PRETTIFY_BODY_CLASS,TXT_PRETTIFY_DIV_CLASS,TXT_PRETTIFY_PRE_CLASS);
	

	char currLine[PATHSIZE*2]={0};
	while(fgets(currLine,PATHSIZE,fstream)){
		dprintf(fd,"%s",currLine);
	}
	dprintf(fd,"\n</pre>\n</div>\n\n<br>\n<br>\n"
				"</body>\n</html>\n");
	fclose(fstream);
	close(fd);
	return tmpFile2;

}
