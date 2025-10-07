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
static char* compressionCmd = "%s -c -%d %s";
//static char* compressionCmd = "gzip -c %s > %s.gzip";
static char inpathbuff[PATHSIZE*6];
static char outpathbuff[PATHSIZE*6+1];
static char cmd[PATHSIZE*30];
typedef struct comp_result{
	int comp_enable,fd;
	FILE* stream;

}comp_result;

typedef struct {
    const char *extension;
    const char *mime_type;
} MimeEntry;

MimeEntry mime_types[] = {
    {".html", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".png", "image/png"},
    {".PNG", "image/png"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".webp", "image/webp"},
    {".mp4", "video/mp4"},
    {".mp3", "audio/mp3"},
    {".wasm", "application/wasm"},
    {".webmanifest", "application/manifest+json"},
    {".gif", "image/gif"},
    {".jfif", "image/jpeg"},
    {".avif", "image/avif"},
    {".ico", "image/x-icon"},
    {".icon", "image/x-icon"},
    {".json", "application/json"},
    {".svg", "image/svg+xml"},
    {".txt", "text/plain"},
    {".webm", "audio/webm"},
    {".wav", "audio/wav"},
    {".m4a", "audio/mp4"},
    {".sp", "text/x-sourcepawn"},
    {".sma", "text/x-sourcepawn"},
    {".inc", "text/x-sourcepawn"},
    {".cpp", "text/x-c++"},
    {".c", "text/x-c++"},
    {".h", "text/x-c++"},
    {".hpp", "text/x-cpp"},
    {".cfg", "text/x-config"},
    {".vdf", "text/plain"},
    {".gam", "text/plain"},
    {".py", "text/x-python"},
    {".bmp", "image/bmp"},
    {".log", "text/plain"},
    {".tga", "image/x-targa"},
    {".tga", "image/x-targa"},
    {".dds","image/x-dds"},
    {".vtf","image/x-vtf"},
    {".vmt","text/x-vmt"},
    {".ogg","audio/ogg"},
    {".flv","video/x-flv"},
    {".flac","audio/flac"},
    {".avi","video/x-msvideo"},
    {".wad","application/x-doom-wad"},
    {".lst","text/plain"},
    {".db","text/plain"},
    {NULL, "application/octet-stream"} // Default fallback
};

const char* get_mime_type(const char *filename) {
    const char *dot = strrchr(filename, '.'); // Find last dot (file extension)
    if (!dot) return "application/octet-stream"; // No extension? Default type.

    for (int i = 0; mime_types[i].extension != NULL; i++) {
        if (strcmp(dot, mime_types[i].extension) == 0) {
            return mime_types[i].mime_type;
        }
    }
    return "application/octet-stream"; // If no match, return generic type
}

static int file_exists(const char *filename)
{
    struct stat buffer;
    return stat(filename, &buffer) == 0;
}

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
static int compressFile(char* target,int enable,struct comp_result* result){
	memset(inpathbuff,0,sizeof(inpathbuff)-1);
	snprintf(inpathbuff,sizeof(inpathbuff)-1,"%s%s",currDir,target);
	if(enable){
	memset(outpathbuff,0,sizeof(outpathbuff)-1);
	snprintf(outpathbuff,sizeof(outpathbuff)-1,"%s%s",inpathbuff,servComp.fileExt);
	memset(cmd,0,sizeof(inpathbuff)-1);

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
	snprintf(cmd,sizeof(cmd)-1,compressionCmd,servComp.cmdTool,compression,inpathbuff);
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
int sendResource(client* c,char* resourceTarget,int compress,int is_directory){
	char path[PATHSIZE]={0};
	char* ptr= path;
	int use_chunked_here=use_chunked;
	u_int64_t fileSize=0;
	ptr+=snprintf(ptr,PATHSIZE,"%s%s",currDir,resourceTarget);
	int we_in_csgo_mode= (!strcmp(servComp.fileExt,bzip2.fileExt));
	if(we_in_csgo_mode&&!is_directory){
		
		if(logging){
			fprintf(logstream,"We in csgo mode!!!!\n");
		}
		path[strlen(path)-strlen(servComp.fileExt)-1]=0;
		resourceTarget[strlen(resourceTarget)-strlen(servComp.fileExt)-1]=0;
		if(logging){
			fprintf(logstream,"We modified the path to: %s\n",path);
			fprintf(logstream,"And modified the resource target to: %s\n",resourceTarget);
		}
	}
	else{

		if(logging){
			fprintf(logstream,"Brother we are NOT in csgo mode\n");
		}
	}
	comp_result result={0,-1,NULL};
	
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
			use_chunked_here=0;
			fprintf(logstream,"Enviando resposta 404...\n");
			printf("Trying to delete %s...\n",outpathbuff);
			remove(outpathbuff);
		}
		return -1;
	}
	
	if(logging){
	fprintf(logstream,"Done!\n");
	}
	char headerBuff[PATHSIZE]={0};
	if(use_fd){
	lseek(result.fd,0,SEEK_END);
	fileSize=lseek(result.fd,0,SEEK_CUR);
	lseek(result.fd,0,SEEK_SET);
	
	}
	else{
	fseek(result.stream,0,SEEK_END);
	fileSize=ftell(result.stream);
	fseek(result.stream,0,SEEK_SET);

	}
	
	if(!notfound){
	if(result.comp_enable){
			if(use_chunked_here){
			
			fillUpChunkedHeaderComp(headerBuff,chunkedHeaderComp,(const char*)get_mime_type(resourceTarget),servComp.fileExt);
			}
			else{
			fillUpNormalHeaderComp(headerBuff,normalHeaderComp,fileSize,(const char*)get_mime_type(resourceTarget),servComp.fileExt);
			
			}
		}
		else{
			if(use_chunked_here){
			fillUpChunkedHeader(headerBuff,chunkedHeader,(const char*)get_mime_type(resourceTarget));
			}
			else{
			fillUpNormalHeader(headerBuff,normalHeader,fileSize,(const char*)get_mime_type(resourceTarget));
			}
		}
	}
	else{

		if(result.comp_enable){
			fillUpFileNotFoundHeaderComp(headerBuff,fileSize,(const char*)get_mime_type(notFoundDefaultTarget),servComp.fileExt);
		}
		else{
			fillUpFileNotFoundHeader(headerBuff,fileSize,(const char*)get_mime_type(notFoundDefaultTarget));

		}
	}
		int header_size=strlen(headerBuff);
			if(send(c->socket,headerBuff,header_size,0)!=(header_size)){
				if(logging){
				fprintf(logstream,"ERRO NO SEND!!!! O GET TEM UM ARGUMENTO!!!!:\n%s\n",strerror(errno));
				}
			}

	if(use_fd){
		if(use_chunked_here){
			sendallchunkedfd(c,result.fd);
	
		}
		else{
			sendallfd(c,result.fd);
	
		}
	}
	else{
		if(use_chunked_here){
			sendallchunkedstream(c,result.stream);

		}
		else{
			sendallstream(c,result.stream);
	
		}
	}
	
	close(result.fd);
	if(result.stream!=NULL){
		fclose(result.stream);
	}
	
	printf("Trying to delete %s...\n",outpathbuff);
	if(result.comp_enable){
		remove(outpathbuff);
	}
	return 0;
}

