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
	printf("%s\n",cmd);
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
	if(use_fd){
		lseek(result->fd,0,SEEK_END);
		fileSize=lseek(result->fd,0,SEEK_CUR);
		lseek(result->fd,0,SEEK_SET);
	}
	else{
		fseek(result->stream,0,SEEK_END);
		fileSize=ftell(result->stream);
		fseek(result->stream,0,SEEK_SET);

	}
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
		(
		(result->comp_enable)?
		fillUpFileNotFoundHeaderComp(headerBuff,fileSize,the_mime_type,servComp.fileExt)
		:
		fillUpFileNotFoundHeader(headerBuff,fileSize,the_mime_type)
		);
	int header_size=strlen(headerBuff);
	print_response_header(headerBuff);
	send_wrapper(c,headerBuff,header_size);

}
void sendResource(client* c,char* resourceTarget,int compress,int is_directory,int is_partial, int start_point){
	char path[PATHSIZE]={0},
		path2[PATHSIZE]={0};
	char* ptr= path;
	ptr+=snprintf(ptr,PATHSIZE,"%s%s",currDir,resourceTarget);

	int we_in_csgo_mode= (!strcmp(servComp.fileExt,bzip2.fileExt));
	if(we_in_csgo_mode&&!is_directory){
		if(logging){
			fprintf(logstream,"We in csgo mode!!!!\n");
		}
		path[strlen(path)-strlen(servComp.fileExt)-1]=0;
		resourceTarget[strlen(resourceTarget)-strlen(servComp.fileExt)-1]=0;
		if(logging){
			fprintf(logstream,"And modified the resource target to: %s\n",resourceTarget);
		}
	}
	else{

		if(logging){
			fprintf(logstream,"Brother we are NOT in csgo mode\n");
		}
	}
	comp_result result={0,-1,NULL};
	memcpy(path2,path,PATHSIZE);
	if(logging){
	fprintf(logstream,"Fetching %s...\n",path);
	}
	int notfound=0;
	if(compressFile(resourceTarget,compress,&result)<0){

		if(logging){

			fprintf(logstream,"Erro na compressão %s...\n",strerror(errno));
		}
		char oldoutpathbuff[PATHSIZE]={0};
		char errorbuff[PATHSIZE*10]={0};
		snprintf(errorbuff, sizeof(errorbuff)-1,"%s%s",currDir,notFoundDefaultTarget);
		memcpy(oldoutpathbuff,outpathbuff,PATHSIZE);
		if(compressFile(notFoundDefaultTarget,compress,&result)<0){

			if(logging){

				fprintf(logstream,"Erro na compressão %s...\n",strerror(errno));
			}
			printf("Trying to delete %s...\n",outpathbuff);
			remove(outpathbuff);
		}
		else{
			notfound=1;
			use_chunked=0;
			fprintf(logstream,"Enviando resposta 404...\n");
			printf("Trying to delete %s...\n",outpathbuff);
			remove(outpathbuff);
		}
	}
	if(logging){
		fprintf(logstream,"Done!\n");
	}
	send_header(c, &result, notfound, start_point, is_partial, resourceTarget);
	push_bytes(c, &result, is_partial,start_point);
	final_cleanup(&result);
}