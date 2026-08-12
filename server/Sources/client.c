
#include "../Includes/preprocessor.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/handlecustom.h"
#include "../Includes/server_vars.h"
#include "../Includes/client.h"
#include "../Includes/load_html.h"
#include "../Includes/ssl_things.h"
#include "../Includes/load_rss.h"
#include "../Includes/php_proxy.h"
#include "../Includes/rcon_proxy.h"
#include "../Includes/send_resource_func.h"
#include "../Includes/server_vars.h"
#include "../Includes/sock_ops.h"
#include "../Includes/txt_sending.h"
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#define READ_FUNC_TO_USE timedreadall
client* curr_client= NULL;
	
static socklen_t socklenpointer;
static void initEmptyClient(client* result){
		FD_ZERO(&result->readfd);
		result->socket=0;
		result->is_tls=use_tls;
		result->ctx_if_ssl=NULL;
		memset(result->username,0,FIELDSIZE);
		memset(&result->client_addr,0,sizeof(struct sockaddr_in));
		

}
static client* spawnEmptyClient(void){
	client* result= malloc(sizeof(client));
		initEmptyClient(result);
	return result;

}

static void sendMediaData(client*c,char* buff,int compress,int is_directory,int is_partial, int range_number){

	sendResource(c,buff,compress,is_directory,is_partial,range_number);
}

static void process_header_target(http_header* the_header){
	
	if(logging>10){
		fprintf(logstream,"Non processed header target:\n%s\n",the_header->target);
	}
	url_decode(the_header->target, the_header->target_processed);
	
	if(logging>10){
		fprintf(logstream,"Processed header target_processed!:\n%s\n",the_header->target_processed);
	}
}
static void handleCurrentActivity(client*c,http_request req){
	int compress=0;
	char targetinout[PATHSIZE]={0};
	http_header header=*(req.header);
	if(!strcmp(header.target,"/")){

		strcpy(header.target,defaultTarget);
	}
	
	process_header_target(&header);
	
	if(findInStringArr(header.split_encoding,servComp.encodingExt)>=0){
		compress=compression;
	}
	int is_directory=isDirectory(header.target_processed);
	if(is_directory){
		sendMediaData(c,generateDirListing(header.target_processed),compress,is_directory,header.is_partial,header.range_number);
		deleteDirListingHTML(header.target_processed);
		return;
	}
	int is_rss_feed=isRssFeed(header.target_processed);
	if(is_rss_feed){
		sendMediaData(c,generateRss(header.target_processed),compress,is_directory,header.is_partial,header.range_number);
		deleteRssXML(header.target_processed);
		return;
	}
	int is_php_result=isPHPRequest(header.target_processed);
	if(is_php_result){
		sendMediaData(c,generatePHPResult(header.target_processed),compress,is_directory,header.is_partial,header.range_number);
		deletePHPResult(header.target_processed);
		return;
	}
	int is_txt_file=isText(header.target_processed);
	if(is_txt_file){
		sendMediaData(c,generateText(header.target_processed),compress,is_directory,header.is_partial,header.range_number);
		deleteTextHtml(header.target_processed);
		return;
	}
	switch(header.type){
	case GET:
		sendMediaData(c,header.target_processed,compress,is_directory,header.is_partial,header.range_number);
	break;
	case POST:
			if(isCustomPostReq(header.target_processed)){
				int is_rcon_result=isRconRequest(header.target_processed);
				if(is_rcon_result){
					handleCustomPostReq(header.target_processed,req.data,targetinout);
					sendMediaData(c,generateRconResult(targetinout),compress,is_directory,header.is_partial,header.range_number);
					deleteRconResult();
					return;
				}
				sendMediaData(c,targetinout,compress,is_directory,header.is_partial,header.range_number);
			}
			else{
				sendMediaData(c,header.target_processed,compress,is_directory,header.is_partial,header.range_number);
			}
	break;
	default:
		
		sendMediaData(c,header.target_processed,compress,is_directory,header.is_partial,header.range_number);
	break;
	}
	
}

void handleConnection(client* c){
 			memset(c->peerbuff,0,BUFFSIZE);
			http_request req;
			memset(&req,0,sizeof(http_request));
			http_header header;
			memset(&header,0,sizeof(http_header));
			if(READ_FUNC_TO_USE(c,c->peerbuff)!=-2){
				if(errno == ECONNRESET){
					raise(SIGINT);
				}
				else if(strlen(c->peerbuff)){
					spawnHTTPRequest(c->peerbuff,&header,&req);
					if(logging>0){
						fprintf(logstream,"Recebemos request!!!:\n");
						
						print_http_req(logstream,req);
					}
					if(!strlen(req.data)){
					free(req.data);
					req.data=malloc(BUFFSIZE);
					memset(req.data,0,BUFFSIZE);
					if(READ_FUNC_TO_USE(c,req.data)!=-2){

						if(logging>0){
							fprintf(logstream,"Recebemos dados!!!: %s\n",req.data);
						}
					}
					}
					handleCurrentActivity(c,req);
					free(req.data);
				}
			}
			
}
void exit_function_client(void){
	
	if(curr_client->is_tls){
		ShutdownSSL(&curr_client->ctx_if_ssl);
		end_openssl_libs_server_side();
		DestroySSL();
	}
	close(curr_client->socket);
	free(curr_client->peerbuff);
	free(curr_client);
	
	exit(-1);
}
int is_tls_connection(int client_fd) {
    unsigned char packet_buff[6];
    ssize_t n = readsome(client_fd, (char*)packet_buff, sizeof(packet_buff), MSG_PEEK);
    
    if( n< 6){
			return 0;
	}
    //https://stackoverflow.com/questions/16194050/tls-protocol-detection-by-using-client-hello-message
	//Check 22 and version info 0300 0301 or 0302
	if (packet_buff[0] == 0x16 && packet_buff[1] == 0x03
	  && (packet_buff[2] == 0x00 || packet_buff[2] == 0x01 || packet_buff[2] == 0x02)){
		if (packet_buff[5] == 1){
			return 1;
		}
	}
    
    return 0;
}

void runClientConnection(int is_tls, int sock){
	
	exit_func_for_this_module = exit_function_client;
	use_tls=is_tls;
	client*c=spawnEmptyClient();
	curr_client= c;
	socklenpointer=sizeof(c->client_addr);
	c->socket=sock;
	c->is_tls=is_tls_connection(c->socket);
	if(c->is_tls){
			if(logging>0){
				fprintf(logstream,"Its tls we do eht!\n");
			}
			InitializeSSL();
			init_openssl_libs_server_side();
			convert_server_con_to_ssl(&c->ctx_if_ssl,c->socket);
	}
	c->peerbuff=malloc(BUFFSIZE);
	memset(c->peerbuff,0,BUFFSIZE);

	handleConnection(c);

	getpeername(c->socket , (struct sockaddr*)&c->client_addr , (socklen_t*)&socklenpointer);
	if(logging>0){
		fprintf(logstream,"Disconnexao cliente de:\n%s:%d\n" ,inet_ntoa(c->client_addr.sin_addr) , ntohs(c->client_addr.sin_port));
			   
	}
			 
	exit_function_client();

}
