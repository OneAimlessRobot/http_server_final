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

#define tmpOne "%s.tmp.txt"
#define tmpTwo "%s.tmp1.txt"

static char tmpFile[PATHSIZE*3]={0};
static char tmpFile2[PATHSIZE*3]={0};

static const char* marked_js_lib_path= "/webcrookedstuff/js/markdown-it.js";
    
static const char* txt_prettify_css_file_path= "/webcrookedstuff/.styles_file_exts/.stylesheet_txt_files.css";

static char tmpDir[PATHSIZE*2]={0},tmpDir2[PATHSIZE*2]={0},currFetchedText[PATHSIZE*2]={0};




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
	snprintf(tmpFile,PATHSIZE*3-1,tmpOne,dir);
	snprintf(tmpFile2,PATHSIZE*3-1,tmpTwo,dir);
	
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
	
	/*
	 * https://stackoverflow.com/questions/72263107/how-do-i-use-markdown-it-without-node-js
	 *
	*/
	
	dprintf(fd,TXT_HTML_PREFIX
		"<br>\n<script>\n"
		"window.onload = function() {"
		"var md = window.markdownit();"
		"var div = document.getElementsByClassName('markdown');"
		
		"for(var i = 0; i < div.length; i++) {"
		
		"	var content = div[i].innerHTML;"
		"	document.getElementsByClassName('markdown')[i].innerHTML = md.render(content);"
		
		" }"
		
		"}"
		"</script>"
		"\n<body class='%s'>\n<br>%s\n<br>\n\n<br>\n<br><div class=\"markdown\">\n\n",marked_js_lib_path,txt_prettify_css_file_path,TXT_PRETTIFY_BODY_CLASS,FREE_ALL_APPS_COUNTDOWN);
	

	char currLine[PATHSIZE*2]={0};
	while(fgets(currLine,PATHSIZE,fstream)){
		dprintf(fd,"%s",currLine);
	}
	dprintf(fd,"\n</div>\n\n<br>\n<br>\n"
				"</body>\n</html>\n");
	fclose(fstream);
	close(fd);
	return tmpFile2;

}

void deleteTextHtml(char* filename){
char buff[PATHSIZE*3]={0};
snprintf(buff,PATHSIZE*3-1,tmpTwo,filename);
snprintf(tmpDir2,PATHSIZE*4-1,"%s%s",currDir,buff);
remove(tmpDir2);

}
