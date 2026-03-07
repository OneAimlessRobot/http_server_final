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
#include "../Includes/load_rss.h"
#include <sys/wait.h>


static char* tmpOne="/.tmp.rss",* tmpTwo="/.tmp1.rss";

static char tmpDir[PATHSIZE*2]={0},tmpDir2[PATHSIZE*2]={0},currFetchedRSS[PATHSIZE*2]={0};

void print_current_date(char date_buff[PATHSIZE*2]){

	memset(date_buff,0,PATHSIZE*2);
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        snprintf(date_buff,(PATHSIZE*2)-1,"%d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

static void generateRssPrimitive(char* path){

	int outfd= open(tmpDir,O_TRUNC|O_WRONLY|O_CREAT,0777);
        snprintf(currFetchedRSS,PATHSIZE*2-1,"%s%s",currDir,path);

        char* args_cmd[]={"cat", currFetchedRSS,NULL};
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
char* generateRss(char* dir){

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
        generateRssPrimitive(dir);
        FILE* fstream;
        if(!(fstream=fopen(tmpDir,"r"))){
                if(logging){
                fprintf(logstream,"ERRO NO RSS FEED %s\n",tmpDir);
                }
                close(fd);
                return NULL;
        }
        remove(tmpDir);
        char currListing[PATHSIZE*4-1]={0};
	char currCat[PATHSIZE*4-1]={0};
	char currLink[PATHSIZE*4-1]={0};
	char dateBuff[PATHSIZE*2]={0};
	int pathsize=strlen(dir);
	if(pathsize&&!strcmp(dir +pathsize-1,"/")){

		dir[pathsize-1]=0;
	}
	dprintf(fd,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
			"\n<rss version=\"2.0\">"
			"\n<channel>"
			"\n	<title>Main rss Feed of lena's attic</title>"
			"\n	<description>Current state of main RSS feed of server</description>"
			"\n	<generator>Lena'sAtticHTTP/1.0</generator>\n\n");

	while(fgets(currListing,PATHSIZE,fstream)){
		currListing[strlen(currListing)-1]=0;
		dprintf(fd,"<item>\n"
			   "	<title>%s</title>\n"
			   "	<description>%s</description>\n",currListing,currListing);

		if(!fgets(currCat,PATHSIZE,fstream)){

			goto print_end_item;
		}
		currCat[strlen(currCat)-1]=0;
		if(!strlen(currCat)){

			dprintf(fd,"	<category>No category here</category>\n");
		}
		else{
			dprintf(fd,"	<category>%s</category>\n",currCat);
		}


		if(!fgets(dateBuff,PATHSIZE,fstream)){

			goto print_end_item;
		}
		dateBuff[strlen(dateBuff)-1]=0;
		if(!strlen(dateBuff)){

			dprintf(fd,"	<pubDate>No date here</pubDate>\n");
		}
		else{
			dprintf(fd,"	<pubDate>%s</pubDate>\n",dateBuff);
		}


		if(!fgets(currLink,PATHSIZE,fstream)){

			goto print_end_item;
		}
		currLink[strlen(currLink)-1]=0;
		if(!strlen(currLink)){

			dprintf(fd,"	<link>about:blank</link>\n");
		}
		else{
			dprintf(fd,"	<link>%s</link>\n",currLink);
		}


	print_end_item:
		dprintf(fd,"</item>\n");
		if(!fgets(currLink,PATHSIZE,fstream)){

			break;
		}
	}
	dprintf(fd,"</channel>\n</rss>\n");
	fclose(fstream);
	close(fd);
	return tmpTwo;

}

void deleteRssXML(void){
char buff[PATHSIZE*2]={0};
snprintf(buff,PATHSIZE*2,"%s%s",currDir,tmpTwo);
remove(buff);

}
