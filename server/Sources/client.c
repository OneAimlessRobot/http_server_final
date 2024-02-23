
#include "../Includes/preprocessor.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/handlecustom.h"
#include "../Includes/server_vars.h"
#include "../Includes/client.h"
#include "../Includes/send_resource_func.h"
#include "../Includes/load_logins.h"
#include "../Includes/load_html.h"
#include "../Includes/server_vars.h"
#include "../Includes/sock_ops.h"
#include "../Includes/session_ops.h"
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
//#define READ_FUNC_TO_USE readall
#define READ_FUNC_TO_USE timedreadall
static socklen_t socklenpointer;
static int clientOn;
static void initEmptyClient(client* result){
		FD_ZERO(&result->readfd);
		result->socket=0;
		memset(result->username,0,FIELDSIZE);
		memset(&result->client_addr,0,sizeof(struct sockaddr_in));
		

}
static client* spawnEmptyClient(void){
	client* result= malloc(sizeof(client));
		initEmptyClient(result);
	return result;

}

static int sendMediaData(client*c,char* buff,char* mimetype,int compress){

	return sendResource(c,buff,mimetype,compress);
}


static void handleCurrentActivity(client*c,http_request req){
	int compress=0;
	
	char targetinout[PATHSIZE]={0};
	http_header header=*(req.header);

	if(!strcmp(header.target,"/")){

		strcpy(header.target,defaultLoginTarget);
	}
	if(logging){
		fprintf(logstream,"%s\n",header.target);
	}
	if(findInStringArr(header.split_encoding,servComp.encodingExt)>=0){
		compress=compression;
	}
	if(isDirectory(header.target)){
		
		sendMediaData(c,generateDirListing(header.target),defaultMimetype,compress);
		deleteDirListingHTML();
		return;
	
	}
	switch(header.type){
	case GET:
			if(isCustomGetReq(header.target)){


				handleCustomGetReq(c,header.target,req.data,targetinout);
				int result=sendMediaData(c,targetinout,defaultMimetype,compress);
				if(result<0){

					send(c->socket,notFoundHeader,strlen(notFoundHeader),0);
				}
			}
			else{
				int result=sendMediaData(c,header.target,header.mimetype,compress);
				if(result<0){
					send(c->socket,notFoundHeader,strlen(notFoundHeader),0);
				
				}
			}
	break;
	case POST:
			if(isCustomPostReq(header.target)){

				handleCustomPostReq(c,header.target,req.data,targetinout);
				int result=sendMediaData(c,targetinout,defaultMimetype,compress);
				if(result<0){
					send(c->socket,notFoundHeader,strlen(notFoundHeader),0);

				}
			}
			else{
				int result=sendMediaData(c,header.target,header.mimetype,compress);
				if(result<0){
					send(c->socket,notFoundHeader,strlen(notFoundHeader),0);

				}
			}
	break;
	default:
		
		sendMediaData(c,header.target,defaultMimetype,compress);
	break;
	}
	
}

void handleConnection(client* c){
 			memset(c->peerbuff,0,PAGE_DATA_SIZE);
			http_request req;
			memset(&req,0,sizeof(http_request));
			http_header header;
			memset(&header,0,sizeof(http_header));
			if(READ_FUNC_TO_USE(c,c->peerbuff,PAGE_DATA_SIZE-1)!=-2){
                  	if(errno == ECONNRESET){
				raise(SIGINT);
			}
			else if(strlen(c->peerbuff)){
				spawnHTTPRequest(c->peerbuff,&header,&req);
				if(logging>0){
				fprintf(logstream,"Recebemos request!!!:\n");
				
				print_http_req(logstream,req);
				}
				if(!strlen(req.data)&&(req.header->content_length>0)){
				free(req.data);
				req.data=malloc(req.header->content_length+1);
				memset(req.data,0,req.header->content_length+1);
				if(READ_FUNC_TO_USE(c,req.data,req.header->content_length)!=-2){

				
					if(logging>0){
					fprintf(logstream,"Recebemos dados!!!: %s\n",req.data);
					}
				}
				}
				handleCurrentActivity(c,req);
				free(req.data);
			}
			}
			else{
			raise(SIGINT);
		
			}
			
}

static void sigpipe_handler(int signal){
	
	if(logging>10){
	fprintf(logstream,"SIGPIPE!!!!!\n");
	}
	clientOn=signal=0;
}
static void sigint_handler(int signal){
	clientOn=signal=0;
}

void runClientConnection(int sock){

	signal(SIGPIPE,sigpipe_handler);
	signal(SIGINT,sigint_handler);
		client*c=spawnEmptyClient();
		socklenpointer=sizeof(c->client_addr);
		c->socket=sock;
		clientOn=1;
c->peerbuff=malloc(PAGE_DATA_SIZE);
memset(c->peerbuff,0,PAGE_DATA_SIZE);
while(clientOn){
	
		handleConnection(c);
	
}
	getpeername(c->socket , (struct sockaddr*)&c->client_addr , (socklen_t*)&socklenpointer);
                    if(logging>0){
			fprintf(logstream,"Host disconnected. Cliente com socket %d ip %s , port %d \n" ,c->socket,inet_ntoa(c->client_addr.sin_addr) , ntohs(c->client_addr.sin_port));
                   
                 }
close(c->socket);
free(c->peerbuff);
free(c);


}
