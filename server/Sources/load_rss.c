#include "../Includes/preprocessor.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/resource_consts.h"
#include "../Includes/request_consts.h"
#include "../Includes/load_rss.h"

static char channel_string_buff[PATHSIZE*2]={0},
		
		currFetchedRSS[PATHSIZE*2]={0};

static const char* uris_for_namespace_ids[MAX_NAMESPACE_URI_ID]={
	"http://www.w3.org/2005/Atom",
	"http://www.w3.org/1999/xhtml"
};

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


int isRssFeed(char* resourceTarget){
    char path[PATHSIZE]={0},
		* ptr=path;
	ptr+=snprintf(ptr,PATHSIZE,"%s%s",currDir,resourceTarget);
	char* ext=get_file_extension(path);
	if(ext){
		return stringsAreEqual(ext,"rss");
	}
	else{
		return 0;

	}

}

static void generateRssPrimitive(char* path){

	int outfd= open(tmpDir,O_WRONLY|O_CREAT,0777);
		
	snprintf(currFetchedRSS,sizeof(currFetchedRSS)-1,"%s%s",currDir,path);
	
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
	(void)chdir("..");
	close(outfd);

}

static int print_out_rss_line_to_fd(int fd, char the_listing[PATHSIZE*2],const char* names_for_tag_ids[],const char* empty_contents_for_tag_ids[],uint64_t max_len){
	char* argv[ARGVMAX]={0};
	int num_tags=make_str_arr(the_listing,"|",argv,max_len,0);
	if(num_tags<=0){

		return -1;
	}
	argv[num_tags-1][strlen(argv[num_tags-1])-1]=0;
	for(int64_t i=0;i<num_tags;i++){
		dprintf(fd,"	<%s>%s</%s>\n",names_for_tag_ids[i],strlen(argv[i])?argv[i]:empty_contents_for_tag_ids[i],names_for_tag_ids[i]);
	}
	return 0;
}



void generateRss(char* dir){
	const char* first_format = get_fetch_parser_handler_first_format(curr_client.fetch_request_table_cell),
		* second_format = get_fetch_parser_handler_second_format(curr_client.fetch_request_table_cell);
	
	snprintf(tmpFile,sizeof(tmpFile)-1,first_format,dir, request_id);
	replaceStringCharacter(&tmpFile[1],'/','.');
	snprintf(curr_client.relative_file_path,sizeof(curr_client.relative_file_path)-1,second_format,dir, request_id);
	replaceStringCharacter(&curr_client.relative_file_path[1],'/','.');
	snprintf(tmpDir,sizeof(tmpDir)-1,"%s%s",tmp_file_dir,tmpFile);
	snprintf(curr_client.filepath,sizeof(curr_client.filepath)-1,"%s%s",tmp_file_dir,curr_client.relative_file_path);
	
	curr_client.fd= open(curr_client.filepath,O_WRONLY|O_CREAT,0777);
	if(curr_client.fd<0){
			if(logging){
			fprintf(logstream,"ERRO NAS DIRETORIAS %s\nErro: %s\n",
											curr_client.filepath,
											strerror(errno));
			}
			curr_client.fd = -1;
			return;
	}
	else if(logging){
		fprintf(logstream,"Ficheiro criado!\n%s\n",curr_client.filepath);
	}
	generateRssPrimitive(dir);
	
	FILE* fstream;
	if(!(fstream=fopen(tmpDir,"r"))){
			if(logging){
			fprintf(logstream,"ERRO NO RSS FEED %s\n",tmpDir);
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
	skip_comments(fstream);
	uint64_t obtained_channel=(uint64_t)fgets(channel_string_buff,PATHSIZE,fstream);
	if(!obtained_channel){
		goto end;
	}
	dprintf(curr_client.fd,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			"<rss version=\"2.0\" xmlns:atom=\"%s\">\n"
			"<xhtml:script xmlns:xhtml=\"%s\" type=\"application/javascript\"/>\n"
			"<channel>\n"
			"<atom:link href=\"%s\" type=\"application/atom+xml\" />\n",
			uris_for_namespace_ids[ATOM],uris_for_namespace_ids[XHTML],mainRssFeed);

	int result_from_channel_printing=print_out_rss_line_to_fd(curr_client.fd,channel_string_buff,names_for_channel_tag_ids,empty_contents_for_channel_tag_ids,MAX_CH_TAG_ID+1);
	if(result_from_channel_printing<0){
		goto channel_end;
	}
	skip_comments(fstream);

	char currListing[PATHSIZE*2]={0};
	while(fgets(currListing,PATHSIZE,fstream)){
		dprintf(curr_client.fd,"<item>\n");
		print_out_rss_line_to_fd(curr_client.fd, currListing,names_for_tag_ids,empty_contents_for_tag_ids,MAX_TAG_ID+1);
		dprintf(curr_client.fd,"</item>\n");
		skip_comments(fstream);
	}
	channel_end:
		dprintf(curr_client.fd,"</channel>\n</rss>\n");
	end:
		fclose(fstream);
		lseek(curr_client.fd,0,SEEK_SET);

}
