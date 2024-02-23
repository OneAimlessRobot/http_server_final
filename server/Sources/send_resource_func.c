#include "../Includes/preprocessor.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/handlecustom.h"
#include "../Includes/server_vars.h"
#include "../Includes/send_resource_func.h"
#include "../Includes/server_innards.h"
#include <sys/socket.h>
#include <zlib.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CHUNK 16384
//static char* compressionCmd = "%s -c %s";
//static char* compressionCmd = "gzip -c %s > %s.gzip";
static char inpathbuff[PATHSIZE*2];
static char outpathbuff[PATHSIZE*2];
//static char cmd[PATHSIZE*6];
typedef struct comp_result{
	int comp_enable,fd;
	FILE* stream;


}comp_result;
/* Compress from file source to file dest until EOF on source.
   def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_STREAM_ERROR if an invalid compression
   level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
   version of the library linked do not match, or Z_ERRNO if there is
   an error reading or writing the files. */
static int def(FILE* source, FILE* dest, int level)
{
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK)
        return ret;
    /* compress until end of file */
    do {
        
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;
	/* run deflate() on input until output buffer not full, finish
           compression if all of source has been read in */
        do {

            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            have = CHUNK - strm.avail_out;
            if (fwrite(out,1,have,dest) != have|| ferror(dest)) {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }

        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */
        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */
    /* clean up and return */
    (void)deflateEnd(&strm);
    return Z_OK;
}
/*static int compressFileCmd(char* target,int enable,int result[2]){
	memset(inpathbuff,0,PATHSIZE*2);
	snprintf(inpathbuff,(PATHSIZE*2)-1,"%s%s",currDir,target);
	if(enable){
	memset(outpathbuff,0,PATHSIZE*2);
	snprintf(outpathbuff,(PATHSIZE*2)-1,"%s.%s",inpathbuff,servComp.fileExt);
	memset(cmd,0,PATHSIZE*6);
	int fd=result[0]=open(outpathbuff,O_RDWR|O_CREAT|O_TRUNC,0777);
	if(result[0]<0){
			if(logging){
			fprintf(logstream,"Invalid filepath: %s\n%s\n",outpathbuff,strerror(errno));
			}
			return -1;
	}
	snprintf(cmd,(PATHSIZE*6)-1,compressionCmd,servComp.cmdTool,inpathbuff);
	printf("%s\n",cmd);
	char* argv[ARGVMAX]={0};
	makeargv(cmd,argv);
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
			break;


	}
	result[1]=1;
	
	}
	else{
	result[0]=open(inpathbuff,O_RDONLY,0777);
	result[1]=0;
	
	if(result[0]<0){
		if(logging){
			fprintf(logstream,"Invalid filepath: %s\n%s\n",inpathbuff,strerror(errno));
		}
		return -1;

	}
	}
	return 0;
}
*/
static int compressFile(char* target,int enable,comp_result* result){
	memset(inpathbuff,0,PATHSIZE*2);
	snprintf(inpathbuff,(PATHSIZE*2)-1,"%s%s",currDir,target);
	if(enable){
	memset(outpathbuff,0,PATHSIZE*2);
	snprintf(outpathbuff,(PATHSIZE*2)-1,"%s.%s",inpathbuff,servComp.fileExt);
	result->fd=open(outpathbuff,O_RDWR,0777);
	result->stream=fopen(outpathbuff,"rb+");
	if((!(result->stream))||(result->fd<0)){
			if(result->stream){
				fclose(result->stream);
			
			}
			else if(result->fd>=0){
				close(result->fd);
			}
			if(logging){
			fprintf(logstream,"Invalid (dest) filepath: %s\n%s\n",inpathbuff,strerror(errno));
			}
			return -1;
	
	}
	FILE* in;
	if(!(in=fopen(inpathbuff,"r"))){
			if(logging){
			fprintf(logstream,"Invalid (src) filepath: %s\n%s\n",inpathbuff,strerror(errno));
			}
			return -1;
	}
	
	def(in,result->stream,compression_level);
	result->comp_enable=1;
	}
	else{
	result->fd=open(inpathbuff,O_RDONLY,0777);
	result->stream=fopen(inpathbuff,"r");
	if((!(result->stream))||(result->fd<0)){
			if(result->stream){
				fclose(result->stream);
			
			}
			else if(result->fd>=0){
				close(result->fd);
			}
			if(logging){
			fprintf(logstream,"Invalid (src) filepath: %s\n%s\n",inpathbuff,strerror(errno));
			}
			return -1;
	
	}
	result->comp_enable=0;
	}
	return 0;
}
int isDirectory(char* resourceTarget){
        char path[PATHSIZE]={0};
	char* ptr= path;
	ptr+=snprintf(ptr,PATHSIZE,"%s%s",currDir,resourceTarget);
	
DIR* directory=opendir(path);
	if(directory){
		closedir(directory);
		if(logging){
			fprintf(logstream,"Diretoria encontrada!!: %s\n",path);
		}
	errno=0;
		return 1;
		
	}
	else{
	errno=0;
		return 0;
	
	}

}
int sendResource(client* c,char* resourceTarget,char* mimetype,int compress){
	char path[PATHSIZE]={0};
	char* ptr= path;
	ptr+=snprintf(ptr,PATHSIZE,"%s%s",currDir,resourceTarget);
	
	comp_result result={0,-1,NULL};
	
	if(logging){
	fprintf(logstream,"Fetching %s...\n",path);
	}
	if(compressFile(resourceTarget,compress,&result)<0){

		if(logging){

			fprintf(logstream,"Erro na compressão %s...\n",strerror(errno));
		}
		return -1;
	}
	
	if(logging){
	fprintf(logstream,"Done!\n");
	}
	char headerBuff[PATHSIZE]={0};
	int needsDelete=0;
	if(result.comp_enable){
	needsDelete=1;
	

		fillUpChunkedHeaderComp(headerBuff,chunkedHeaderComp,mimetype,servComp.fileExt);
	
	}
	else{
	
		fillUpChunkedHeader(headerBuff,chunkedHeaderComp,mimetype);
		
	}
		int header_size=strlen(headerBuff);
			if(send(c->socket,headerBuff,header_size,0)!=(header_size)){
				if(logging){
				fprintf(logstream,"ERRO NO SEND!!!! O GET TEM UM ARGUMENTO!!!!:\n%s\n",strerror(errno));
				}
			}

	if(use_fd){
		sendallchunkedfd(c,result.fd);
		
	}
	else{
		sendallchunkedstream(c,result.stream);
	}
	
	close(result.fd);
	fclose(result.stream);
	
	if(needsDelete){
		remove(outpathbuff);
	}
	return 0;
}

