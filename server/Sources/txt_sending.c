#include "../Includes/preprocessor.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/resource_consts.h"
#include "../Includes/template_funcs.h"

#define TXT_PRETTIFY_BODY_CLASS "txt_prettify_body"
#define TXT_PRETTIFY_PRE_CLASS TXT_PRETTIFY_BODY_CLASS
#define TXT_PRETTIFY_DIV_CLASS TXT_PRETTIFY_BODY_CLASS

    
static const char* txt_prettify_css_file_path= "/webcrookedstuff/.styles_file_exts/.stylesheet_txt_files.css";

static char currFetchedText[PATHSIZE*2]={0};




int isText(char* resourceTarget){
        char path[PATHSIZE]={0};
	char* ptr=path;
	ptr+=snprintf(ptr,PATHSIZE,"%s%s",currDir,resourceTarget);
	char* ext=get_file_extension(path);
	if(ext){
		return stringsAreEqual(ext,"txt");
	}
	else{
		return 0;

	}

}

int isNonPrettifiable(char* resourceTarget){
    u_int8_t result1 = stringsAreEqual(resourceTarget,robotsTarget);
	char * result2 = strstr(resourceTarget,excludePrettifyTarget);
			
	return result1 || result2;

}

static void generateTextPrimitive(char* path){

	int outfd= open(tmpDir,O_WRONLY|O_CREAT,0777);
		
	snprintf(currFetchedText,sizeof(currFetchedText)-1,"%s%s",currDir,path);
	
    snprintf(curr_client.og_filepath,sizeof(curr_client.og_filepath)-1,"%s",currFetchedText);
	
    build_last_modified_buff(curr_client.og_filepath);
    
	int stat_result = check_stat_of_file(currFetchedText,curr_client.the_stat_struct,logging,logstream);
	if(stat_result<0){
		if(logging){
		fprintf(logstream,"ERRO NO TXT SENDING %s\nErro: %s\n",
														tmpDir,
														strerror(errno));
		}
		curr_client.is_not_found = 1;
		return;
		
	}
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



void generateText(char* dir){

	snprintf(tmpFile,sizeof(tmpFile)-1,tmpOneTXT_HTML,dir, request_id);
	replaceStringCharacter(&tmpFile[1],'/','.');
	snprintf(curr_client.relative_file_path,sizeof(curr_client.relative_file_path)-1,tmpTwoTXT_HTML,dir, request_id);
	replaceStringCharacter(&curr_client.relative_file_path[1],'/','.');
	snprintf(tmpDir,sizeof(tmpDir)-1,"%s%s",tmp_file_dir,tmpFile);
	snprintf(curr_client.filepath,sizeof(curr_client.filepath)-1,"%s%s",tmp_file_dir,curr_client.relative_file_path);
	
	curr_client.fd= open(curr_client.filepath,O_WRONLY|O_CREAT,0777);
	if(curr_client.fd <0){
			if(logging){
			fprintf(logstream,"ERRO NO TXT SENDING %s\nErro: %s\n",
															curr_client.filepath,
															strerror(errno));
			}
			curr_client.fd = -1;
			return;
	}
	else if(logging){
		fprintf(logstream,"Ficheiro criado!\n%s\n",curr_client.filepath);
	}
	generateTextPrimitive(dir);
	
	if(curr_client.is_not_found){
		remove(curr_client.filepath);
		return;
		
	}
	FILE* fstream;
	if(!(fstream=fopen(tmpDir,"r"))){
			if(logging){
			fprintf(logstream,"ERRO NO TEXT SENDING %s\n",tmpDir);
			}
			close(curr_client.fd);
			curr_client.fd = -1;
			return;
	}
	remove(tmpDir);
	int pathsize=strlen(dir);
	if(pathsize&&!strcmp(dir +pathsize-1,"/")){

		dir[pathsize-1]=0;
	}
	
	
	dprintf(curr_client.fd,TXT_HTML_PREFIX
		"\n<body class='%s'>\n<br>\n<br>\n\n<br>\n<br><div class='%s'>\n<pre class='%s'>\n\n",txt_prettify_css_file_path,TXT_PRETTIFY_BODY_CLASS,TXT_PRETTIFY_DIV_CLASS,TXT_PRETTIFY_PRE_CLASS);
	

	char currLine[PATHSIZE*2]={0};
	while(fgets(currLine,PATHSIZE,fstream)){
		dprintf(curr_client.fd,"%s",currLine);
	}
	dprintf(curr_client.fd,"\n</pre>\n</div>\n\n<br>\n<br>\n"
				"</body>\n</html>\n");
	fclose(fstream);
	lseek(curr_client.fd,0,SEEK_SET);

}
