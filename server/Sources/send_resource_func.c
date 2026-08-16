#include "../Includes/preprocessor.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/client.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/handlecustom.h"
#include "../Includes/server_vars.h"
#include "../Includes/load_html.h"
#include "../Includes/load_rss.h"
#include "../Includes/php_proxy.h"
#include "../Includes/rcon_proxy.h"
#include "../Includes/send_resource_func.h"
#include "../Includes/server_innards.h"
#include "../Includes/template_funcs.h"
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>


void send_wrapper(char* buff, int buff_size) {
	if((curr_client.is_tls?SSL_write(curr_client.ctx_if_ssl,buff,buff_size):
			send(curr_client.socket,buff,buff_size,0))!=(buff_size)){
		if(logging>5){
			fprintf(logstream,"ERRO NO SEND!!%s\n",strerror(errno));
		}
	}	
}

#include "../Includes/send_resource_wrappers_for_file_exts.h"



static int file_exists(const char *filename)
{
    struct stat buffer;
    return stat(filename, &buffer) == 0;
}

static int compressFile(char* target, int enable){
	char* ptr = NULL;
	if(!strlen(target)){
		ptr = curr_client.filepath;
	}
	else{
		
		memset(inpathbuff,0,sizeof(inpathbuff)-1);
		snprintf(inpathbuff,sizeof(inpathbuff)-1,"%s%s",currDir,target);
		ptr= inpathbuff;
	}
	if(enable){
	memset(outpathbuff,0,sizeof(outpathbuff)-1);
	snprintf(outpathbuff,sizeof(inpathbuff)+1,"%s.%s",ptr,servComp.fileExt);
	int fd=comp_fd=open(outpathbuff,O_RDWR|O_CREAT,0777);
	if(fd<0){
			if(logging){
				fprintf(logstream,"Invalid filepath: %s\n%s\n",outpathbuff,strerror(errno));
			}
			return -1;
	}
	if(!file_exists(ptr)){
			if(logging){
				fprintf(logstream,"Invalid filepath: %s\n%s\n",ptr,strerror(errno));
			}
			close(fd);
			comp_fd=-1;
			comp_enable = 0;
			return -1;
	}
	snprintf(cmd,sizeof(cmd)-1,compressionCmd,servComp.cmdTool,compression_level,ptr);
	
	if(logging){
	
		printf("%s\n",cmd);
	
	}
	
	char* argv[ARGVMAX]={0};
	makeargv(cmd,argv, 3);
	int pid=fork();
	switch(pid){
		case -1:
			close(fd);
			return -1;
		case 0:
			dup2(fd,1);
			execvp(argv[0],argv);
			perror("exec na compressao\n");
			exit(-1);

		default:
			wait(NULL);
			comp_enable=1;
			break;


	}
	}
	else{
	comp_fd=open(ptr,O_RDONLY,0777);
	comp_enable=0;
	if(comp_fd<0){
		if(logging){
			fprintf(logstream,"Invalid filepath: %s\n%s\n",ptr,strerror(errno));
		}
		return -1;

	}
	}
	return 0;
}

static void send_header(int notfound,  int start_point, int is_partial){

	u_int64_t fileSize=0;
	const char* the_mime_type= get_mime_type(notfound?notFoundDefaultTarget:curr_client.relative_file_path);
	
	lseek(comp_fd,0,SEEK_END);
	
	fileSize=lseek(comp_fd,0,SEEK_CUR);
	
	lseek(comp_fd,0,SEEK_SET);
	
	(!notfound)?
		((comp_enable)?
			((use_chunked)?
				((!is_partial)?
					fillUpChunkedHeaderComp(curr_client.sent_header_buff,the_mime_type,servComp.fileExt)
					:
					fillUpChunkedPartialHeaderComp(curr_client.sent_header_buff,the_mime_type,servComp.fileExt,start_point,fileSize)
				)
				:
				((!is_partial)?
					fillUpNormalHeaderComp(curr_client.sent_header_buff,the_mime_type,servComp.fileExt, fileSize)
					:
					fillUpNormalPartialHeaderComp(curr_client.sent_header_buff,the_mime_type,servComp.fileExt,start_point,fileSize)
				)
			)
			:
			((use_chunked)?
				((!is_partial)?
					fillUpChunkedHeader(curr_client.sent_header_buff,the_mime_type)		
					:
					fillUpChunkedPartialHeader(curr_client.sent_header_buff,the_mime_type,start_point,fileSize)
				)
				:
				((!is_partial)?
					fillUpNormalHeader(curr_client.sent_header_buff,the_mime_type,fileSize)		
					:
					fillUpNormalPartialHeader(curr_client.sent_header_buff,the_mime_type,start_point,fileSize)
				)
			)
		)
		:
		fillUpFileNotFoundHeader(curr_client.sent_header_buff,fileSize,the_mime_type);
	int header_size=strlen(curr_client.sent_header_buff);
	print_response_header(curr_client.sent_header_buff);
	send_wrapper(curr_client.sent_header_buff,header_size);

}
void sendResource(int compress,int is_directory,int is_partial, int start_point, int is_head){
	if(curr_client.fd<0){
		return;
	}
	close(curr_client.fd);
	int we_in_csgo_mode= (!strcmp(servComp.fileExt,bzip2.fileExt));
	if(we_in_csgo_mode&&!is_directory){
		curr_client.filepath[strlen(curr_client.filepath)-strlen(servComp.fileExt)-1]=0;
		curr_client.target[strlen(curr_client.target)-strlen(servComp.fileExt)-1]=0;
	}
	if(logging > 5){
		fprintf(logstream,"Fetching %s...\n",curr_client.filepath);
	}
	int notfound=0;
	if(compressFile("",compress)<0){

		if(logging){

			fprintf(logstream,"Erro na compressão %s...\n",strerror(errno));
		}
		if(compressFile(notFoundDefaultTarget,0)<0){

			if(logging){

				fprintf(logstream,"Erro na compressão %s...\n",strerror(errno));
			}
		}
		else{
			notfound=1;
			use_chunked=0;
			
			if(logging){
			
				fprintf(logstream,"Enviando resposta 404...\n");
			
			}
		}
	}
	if(logging){
		fprintf(logstream,"Done!\n");
	}
	send_header(notfound, start_point, is_partial);
	if(!is_head){
		push_bytes(is_partial,start_point);
	}
	final_cleanup(notfound);
}