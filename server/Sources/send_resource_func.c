#include "../Includes/preprocessor.h"
#include "../Includes/auxFuncs.h"
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


void send_wrapper(client* c,char* buff, int buff_size) {
	if((c->is_tls?SSL_write(c->ctx_if_ssl,buff,buff_size):send(c->socket,buff,buff_size,0))!=(buff_size)){
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

static int compressFile(char* target,int enable,struct comp_result* result){
	memset(inpathbuff,0,sizeof(inpathbuff)-1);
	snprintf(inpathbuff,sizeof(inpathbuff)-1,"%s%s",currDir,target);

	if(enable){
	memset(outpathbuff,0,sizeof(outpathbuff)-1);
	snprintf(outpathbuff,sizeof(inpathbuff)+1,"%s.%s",inpathbuff,servComp.fileExt);
	int fd=result->fd=open(outpathbuff,O_RDWR|O_CREAT|O_TRUNC,0777);
	if(result->fd<0){
			if(logging){
				fprintf(logstream,"Invalid filepath: %s\n%s\n",outpathbuff,strerror(errno));
			}
			return -1;
	}
	if(!file_exists(inpathbuff)){
			if(logging){
				fprintf(logstream,"Invalid filepath: %s\n%s\n",inpathbuff,strerror(errno));
			}
			close(result->fd);
			result->fd=-1;
			result->comp_enable=0;
			return -1;
	}
	snprintf(cmd,sizeof(cmd)-1,compressionCmd,servComp.cmdTool,compression_level,inpathbuff);
	
	if(logging){
	
		printf("%s\n",cmd);
	
	}
	
	char* argv[ARGVMAX]={0};
	makeargv(cmd,argv, -1);
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
			result->comp_enable=1;
			break;


	}
	}
	else{
	result->fd=open(inpathbuff,O_RDONLY,0777);
	result->comp_enable=0;
	if(result->fd<0){
		if(logging){
			fprintf(logstream,"Invalid filepath: %s\n%s\n",inpathbuff,strerror(errno));
		}
		return -1;

	}
	}
	return 0;
}

static void send_header(client* c, comp_result* result, int notfound,  int start_point, int is_partial, char* resourceTarget){

	u_int64_t fileSize=0;
	char headerBuff[PATHSIZE]={0};
	const char* the_mime_type= get_mime_type(notfound?notFoundDefaultTarget:resourceTarget);
	
	(use_fd)?lseek(result->fd,0,SEEK_END):fseek(result->stream,0,SEEK_END);
	
	fileSize=(use_fd)?lseek(result->fd,0,SEEK_CUR):ftell(result->stream);
	
	(use_fd)?lseek(result->fd,0,SEEK_SET):fseek(result->stream,0,SEEK_SET);
	
	(!notfound)?
		((result->comp_enable)?
			((use_chunked)?
				((!is_partial)?
					fillUpChunkedHeaderComp(headerBuff,the_mime_type,servComp.fileExt)
					:
					fillUpChunkedPartialHeaderComp(headerBuff,the_mime_type,servComp.fileExt,start_point,fileSize)
				)
				:
				((!is_partial)?
					fillUpNormalHeaderComp(headerBuff,the_mime_type,servComp.fileExt, fileSize)
					:
					fillUpNormalPartialHeaderComp(headerBuff,the_mime_type,servComp.fileExt,start_point,fileSize)
				)
			)
			:
			((use_chunked)?
				((!is_partial)?
					fillUpChunkedHeader(headerBuff,the_mime_type)		
					:
					fillUpChunkedPartialHeader(headerBuff,the_mime_type,start_point,fileSize)
				)
				:
				((!is_partial)?
					fillUpNormalHeader(headerBuff,the_mime_type,fileSize)		
					:
					fillUpNormalPartialHeader(headerBuff,the_mime_type,start_point,fileSize)
				)
			)
		)
		:
		fillUpFileNotFoundHeader(headerBuff,fileSize,the_mime_type);
	int header_size=strlen(headerBuff);
	print_response_header(headerBuff);
	send_wrapper(c,headerBuff,header_size);

}
void sendResource(client* c,char* resourceTarget,int compress,int is_directory,int is_partial, int start_point, int is_head){
	if(!resourceTarget){
		return;
	}
	char path[PATHSIZE]={0};
	char* ptr= path;
	ptr+=snprintf(ptr,PATHSIZE,"%s%s",currDir,resourceTarget);

	int we_in_csgo_mode= (!strcmp(servComp.fileExt,bzip2.fileExt));
	if(we_in_csgo_mode&&!is_directory){
		path[strlen(path)-strlen(servComp.fileExt)-1]=0;
		resourceTarget[strlen(resourceTarget)-strlen(servComp.fileExt)-1]=0;
	}
	comp_result result={0,-1,NULL};
	if(logging > 5){
		fprintf(logstream,"Fetching %s...\n",path);
	}
	int notfound=0;
	if(compressFile(resourceTarget,compress,&result)<0){

		if(logging){

			fprintf(logstream,"Erro na compressão %s...\n",strerror(errno));
		}
		if(compressFile(notFoundDefaultTarget,0,&result)<0){

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
	send_header(c, &result, notfound, start_point, is_partial, resourceTarget);
	if(!is_head){
		push_bytes(c, &result, is_partial,start_point);
	}
	final_cleanup(&result,notfound);
}