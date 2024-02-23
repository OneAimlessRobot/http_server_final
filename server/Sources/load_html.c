
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

const char* clientHTMLEntryStyle="<style>\n"
				"p:{\n"
				"\n"
				"font-size: 10px;\n"
				"}\n"
				"p.ADMIN:{\n"
				"color: red;\n"
				"\n"
				"}\n"
				"</style>\n";



static char* tmpOne="/.tmp.html",* tmpTwo="/.tmp1.html";

static char* tmpDir=NULL,*tmpDir2=NULL,*currSearchedDir=NULL;

static void generateDirListingPrimitive(char* path){
	tmpDir=malloc(PATHSIZE);
	memset(tmpDir,0,PATHSIZE);
	tmpDir2=malloc(PATHSIZE);
	memset(tmpDir2,0,PATHSIZE);
	currSearchedDir=malloc(PATHSIZE);
	memset(currSearchedDir,0,PATHSIZE);
	snprintf(tmpDir,PATHSIZE,"%s%s",currDir,tmpOne);
	snprintf(tmpDir2,PATHSIZE,"%s%s",currDir,tmpTwo);
	int outfd= open(tmpDir,O_TRUNC|O_WRONLY|O_CREAT,0777);
	char* cmd= malloc(PATHSIZE);
	memset(cmd,0,PATHSIZE);
	snprintf(currSearchedDir,PATHSIZE,"%s%s",currDir,path);
	snprintf(cmd,PATHSIZE,"ls -1 %s > %s",currSearchedDir,tmpDir);
	system(cmd);
	free(cmd);
	close(outfd);
}



char* generateDirListing(char* dir){
	
	generateDirListingPrimitive(dir);
	int fd=	open(tmpDir2,O_TRUNC|O_WRONLY|O_CREAT,0777);
	
	if(!tmpDir||!tmpDir2){

		return NULL;
	}
	if(fd<0){
		if(logging){
		fprintf(logstream,"ERRO NAS DIRETORIAS %s\n",tmpDir2);
		}
		return NULL;
	}
	FILE* fstream;
	if(!(fstream=fopen(tmpDir,"r"))){
		if(logging){
		fprintf(logstream,"ERRO NAS DIRETORIAS %s\n",tmpDir);
		}
		return NULL;
	}
	remove(tmpDir);
	int pathsize=strlen(dir);
	if(pathsize&&!strcmp(dir +pathsize-1,"/")){

		dir[pathsize-1]=0;
	}
	int noRoot=strcmp(dir,"/resources");
	char* currListing=malloc(BUFFSIZE);
	memset(currListing,0,BUFFSIZE);
	dprintf(fd,"<!DOCTYPE html>\n<html>\n<head>\n<base href=''>\n</head>\n<body>");
	if(!noRoot){

		dprintf(fd,"\n<h1>DIRETORIA ROOT DO SERVER!!!!!</h1>");
	}
	else{
		dprintf(fd,"\n<br>\n<h2>INDEX OF: %s</h2><br>\n<br>\n",dir);
		dprintf(fd,"\n<br>\n<a href='%s/..'>Prev</a><br><br><br>\n",dir);
	
	}
	while(fgets(currListing,BUFFSIZE,fstream)){
		currListing[strlen(currListing)-1]=0;
		dprintf(fd,"<a href='%s/%s'>%s</a><br>\n",dir,currListing,currListing);
	
	}
	
	dprintf(fd,"</body>\n</html>\n");
	free(currListing);
	fclose(fstream);
	close(fd);
	free(tmpDir);
	free(tmpDir2);
	free(currSearchedDir);
	return tmpTwo;

}

void deleteDirListingHTML(void){
char buff[PATHSIZE*2]={0};
snprintf(buff,PATHSIZE*2,"%s%s",currDir,tmpTwo);
remove(buff);

}
