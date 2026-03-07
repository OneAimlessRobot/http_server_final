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
#include "../Includes/load_html.h"
#include <sys/wait.h>

static char* FREEE_ALL_APPS_COUNTDOWN = "<script src='https://keepandroidopen.org/banner.js'></script>";

static char* tmpOne="/.tmp.html",* tmpTwo="/.tmp1.html";

static const char* dir_listing_css_file_path= "/webcrookedstuff/.dir_listing_style.css";

static char tmpDir[PATHSIZE*2]={0},tmpDir2[PATHSIZE*2]={0},currSearchedDir[PATHSIZE*2]={0};

static void generateDirListingPrimitive(char* path){

	int outfd= open(tmpDir,O_TRUNC|O_WRONLY|O_CREAT,0777);
        snprintf(currSearchedDir,PATHSIZE*2-1,"%s%s",currDir,path);

        char* args_cmd[]={"ls","-1", currSearchedDir,NULL};
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
char* generateDirListing(char* dir){

	logging=1;
        logstream=stderr;
        snprintf(tmpDir,PATHSIZE*2-1,"%s%s",currDir,tmpOne);
        snprintf(tmpDir2,PATHSIZE*2-1,"%s%s",currDir,tmpTwo);
        int fd= open(tmpDir2,O_TRUNC|O_WRONLY|O_CREAT,0777);
        if(fd<0){
                if(logging){
                fprintf(logstream,"ERRO NAS DIRETORIAS %s\nErro: %s\n",tmpDir2,strerror(errno));
                }
                return NULL;
        }
        generateDirListingPrimitive(dir);
        FILE* fstream;
        if(!(fstream=fopen(tmpDir,"r"))){
                if(logging){
                fprintf(logstream,"ERRO NAS DIRETORIAS %s\n",tmpDir);
                }
                close(fd);
                return NULL;
        }
        remove(tmpDir);
        char currListing[PATHSIZE*4-1]={0};
	int pathsize=strlen(dir);
	if(pathsize&&!strcmp(dir +pathsize-1,"/")){

		dir[pathsize-1]=0;
	}
	int noRoot=strcmp(dir,defaultTarget);
	dprintf(fd,"<!DOCTYPE html>"
			"\n<html>"
			"\n<head>"
			"\n<base href=''>"
			"\n<link rel='stylesheet' type='text/css' href='%s'>"
			"\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
			"\n</head>"
			"\n<body class='dir_listing'>\n<br>%s\n<br>\n",dir_listing_css_file_path,FREEE_ALL_APPS_COUNTDOWN);
	if(!noRoot){

		dprintf(fd,"\n<h1>DIRETORIA ROOT DO SERVER!!!!!</h1>");
	}
	else{
		dprintf(fd,"\n<br>"
				"\n<h2>INDEX OF: %s</h2>"
				"<br>\n"
				"<br>\n",dir);

		dprintf(fd,"\n<br>\n<a class='ref_for_dir' href='%s/..'>Prev</a><br><br><br>\n",dir);

	}
	while(fgets(currListing,PATHSIZE,fstream)){
		currListing[strlen(currListing)-1]=0;
		dprintf(fd,"<a class='%s' href='%s/%s'>%s</a><br>\n",get_file_extension(currListing)?"ref_for_file":"ref_for_dir",dir,currListing,currListing);
	
	}
	
	dprintf(fd,"</body>\n</html>\n");
	fclose(fstream);
	close(fd);
	return tmpTwo;

}

void deleteDirListingHTML(void){
char buff[PATHSIZE*2]={0};
snprintf(buff,PATHSIZE*2,"%s%s",currDir,tmpTwo);
remove(buff);

}
