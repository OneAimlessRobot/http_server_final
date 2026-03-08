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

static char* FREE_ALL_APPS_COUNTDOWN = "/webcrookedstuff/.keep_android_open_please.js";
static char* rss_feed_self_link= "/webcrookedstuff/rss/the_main_rss.rss";
static char* tmpOne="/.tmp.rss",* tmpTwo="/.tmp1.rss";
static char channel_string_buff[PATHSIZE*2]={0};

static char tmpDir[PATHSIZE*2]={0},tmpDir2[PATHSIZE*2]={0},currFetchedRSS[PATHSIZE*2]={0};

typedef enum {ATOM=0,XHTML,MAX_NAMESPACE_URI_ID}namespace_uri_id;

static const char* uris_for_namespace_ids[MAX_NAMESPACE_URI_ID]={
	"http://www.w3.org/2005/Atom",
	"http://www.w3.org/1999/xhtml"
};

typedef enum {TITLE=0,DESC,CAT,DATE,LINK,MAX_TAG_ID}tag_id;

static const char* names_for_tag_ids[MAX_TAG_ID]=
				{"title",
	         		"description",
				"category",
				"pubDate",
				"link"};


static const char* empty_contents_for_tag_ids[MAX_TAG_ID]=
				{"No title here",
	         		"No description here",
				"No category here",
				"Thu 1970-01-01 01:00:00 CET",
				"about:blank"};


typedef enum {CH_TITLE=0,CH_LINK,CH_LANGUAGE,CH_UPDATE_PERIOD,CH_UPDATE_FREQUENCY,CH_DESCRIPTION,CH_DATE,CH_GENERATOR,MAX_CH_TAG_ID}channel_tag_id;
/*
<language>en-us</language>
 <sy:updatePeriod>hourly</sy:updatePeriod>
 <sy:updateFrequency>1</sy:updateFrequency>1
*/
static const char* names_for_channel_tag_ids[MAX_CH_TAG_ID]=
				{"title",
	         		"link",
				"language",
				"atom:updatePeriod",

				"atom:updateFrequency",
				"description",
				"pubDate",
				"generator"};


static const char* empty_contents_for_channel_tag_ids[MAX_CH_TAG_ID]=
				{"No channel title here",
	         		"about:blank",
				"en",
				"hourly",

				"1",
				"No description here",
				"Thu 1970-01-01 01:00:00 CET",
				"le generateur"};



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

int print_out_rss_line_to_fd(int fd, char the_listing[PATHSIZE*2],const char* names_for_tag_ids[],const char* empty_contents_for_tag_ids[],uint64_t max_len){
	char* argv[ARGVMAX]={0};
	int num_tags=make_str_arr(the_listing,"|",argv,max_len);
	if(num_tags<=0){

		return -1;
	}
	argv[max_len-2][strlen(argv[max_len-2])-1]=0;
	for(uint64_t i=0;i<max_len-1;i++){
		dprintf(fd,"	<%s>%s</%s>\n",names_for_tag_ids[i],strlen(argv[i])?argv[i]:empty_contents_for_tag_ids[i],names_for_tag_ids[i]);
	}
	return 0;
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
	int pathsize=strlen(dir);
	if(pathsize&&!strcmp(dir +pathsize-1,"/")){

		dir[pathsize-1]=0;
	}
	skip_comments(fstream);
	uint64_t obtained_channel=(uint64_t)fgets(channel_string_buff,PATHSIZE,fstream);
	if(!obtained_channel){
		goto end;
	}
	dprintf(fd,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			"<rss version=\"2.0\" xmlns:atom=\"%s\">\n"
			"<xhtml:script xmlns:xhtml=\"%s\" type=\"application/javascript\"/>\n"
			"<channel>\n"
			"<atom:link href=\"%s\" type=\"application/atom+xml\" />\n"
			"<script src=\"%s\"></script>\n",
			uris_for_namespace_ids[ATOM],uris_for_namespace_ids[XHTML],rss_feed_self_link,FREE_ALL_APPS_COUNTDOWN);

	int result_from_channel_printing=print_out_rss_line_to_fd(fd,channel_string_buff,names_for_channel_tag_ids,empty_contents_for_channel_tag_ids,MAX_CH_TAG_ID+1);
	if(result_from_channel_printing<0){
		goto channel_end;
	}
	skip_comments(fstream);

	char currListing[PATHSIZE*2]={0};
	while(fgets(currListing,PATHSIZE,fstream)){
		dprintf(fd,"<item>\n");
		print_out_rss_line_to_fd(fd, currListing,names_for_tag_ids,empty_contents_for_tag_ids,MAX_TAG_ID+1);
		dprintf(fd,"</item>\n");
		skip_comments(fstream);
	}
	channel_end:
		dprintf(fd,"</channel>\n</rss>\n");
	end:
		fclose(fstream);
		close(fd);
	return tmpTwo;

}

void deleteRssXML(void){
char buff[PATHSIZE*2]={0};
snprintf(buff,PATHSIZE*2,"%s%s",currDir,tmpTwo);
remove(buff);

}
