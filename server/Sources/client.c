
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

http_header header = {0};
http_request req = {0};
client curr_client = {0};

char tmpFile[PATHSIZE*3]={0},
	tmpFile2[PATHSIZE*3]={0},
	tmpDir[PATHSIZE*5]={0};
	
static socklen_t socklenpointer;


void exit_function_client(void){
	
	if(curr_client.is_tls){
		ShutdownSSL(&curr_client.ctx_if_ssl);
		end_openssl_libs_server_side();
		DestroySSL();
	}
	if(curr_client.fd >=0){
		close(curr_client.fd);
	}
	
	if(curr_client.socket >=0){
		close(curr_client.socket);
	}
	exit(-1);
}

static int generating_func_wrapper(void (*generating_func)(char*), char* arg){
	generating_func(arg);
	return 0;
}

static void head_and_get_wrapper(void (*generating_func)(char*)){
	(generating_func!=NULL)?
			generating_func_wrapper(generating_func, curr_client.target)
			:
			snprintf(curr_client.filepath,sizeof(curr_client.filepath)-1,
						"%s%s",currDir,curr_client.target);
}
static void process_req_switch_block(void (*generating_func)(char*)){
	

	switch(curr_client.req->header->type){
		case GET:
			head_and_get_wrapper(generating_func);
		break;
		case HEAD:
			head_and_get_wrapper(generating_func);
		break;
			
		break;
		case POST:
			if(isCustomPostReq(curr_client.req)){
				handleCustomPostReq(curr_client.req);
				fprintf(logstream,"The relative file path here is: %s\n"
								"The processed target here is: %s\n",
									curr_client.relative_file_path,
									curr_client.req->processed_data);
				
				
			}
			(generating_func!=NULL)?
				generating_func_wrapper(generating_func, curr_client.req->processed_data):
				snprintf(curr_client.filepath,sizeof(curr_client.filepath)-1,
							"%s%s",currDir,curr_client.relative_file_path);
			
			memset(curr_client.target,0,PATHSIZE);
			snprintf(curr_client.target,sizeof(curr_client.target),
							"%s",curr_client.relative_file_path);
		break;
		default:
		break;
	}	
}

static void process_header_target(void){
	
	url_decode(curr_client.req->header->target, curr_client.target);
	snprintf(curr_client.relative_file_path,sizeof(curr_client.relative_file_path),"%s",curr_client.target);
	
	if(logging>3){
		fprintf(logstream,"The target is: %s\n", curr_client.target);
		fprintf(logstream,"The relative file path here is: %s\n", curr_client.relative_file_path);
	}
}
static void handleCurrentActivity(void){
	
	if(curr_client.req->header->type==INVALID){
		send_wrapper((char*)HTTP_METHOD_ERR_HEADER,strlen(HTTP_METHOD_ERR_HEADER));
		return;
	}

	if(!strcmp(curr_client.req->header->target,"/")){

		strcpy(curr_client.req->header->target,defaultTarget);
	}
	
	process_header_target();
	
	if(findInStringArr(curr_client.req->header->split_encoding,servComp.encodingExt)>=0){
		curr_client.compress=use_compression;
	}
	u_int8_t is_head = (curr_client.req->header->type==HEAD);
	
	void (*generating_func)(char*) = NULL;
	
	if((curr_client.is_directory=isDirectory(curr_client.target))){
		generating_func = generateDirListing;
	}
	else if(isRssFeed(curr_client.target)){
		generating_func = generateRss;
	}
	else if(isPHPRequest(curr_client.target)){
		generating_func = generatePHPResult;
	}
	else if(isText(curr_client.target)){
		generating_func = generateText;
	}
	else if(isRconRequest(curr_client.target)){
		
		generating_func = generateRconResult;
	}
	process_req_switch_block(generating_func);
	sendResource(is_head);
	if(generating_func){
		remove(curr_client.filepath);
	}
}

void handleConnection(void){
	if(READ_FUNC_TO_USE(&curr_client,curr_client.peerbuff)!=-2){
		if(errno == ECONNRESET){
			exit_function_client();
		}
		else if(strlen(curr_client.peerbuff)){
			spawnHTTPRequest();
			if(logging>0){
				fprintf(logstream,"Recebemos request!!!:\n");
				
				print_http_req(logstream);
			}
			if(!strlen(curr_client.req->data)){
				memset(curr_client.req->data,0,BUFFSIZE);
				if(READ_FUNC_TO_USE(&curr_client,curr_client.req->data)!=-2){

					if(logging>0){
						fprintf(logstream,"Recebemos dados!!!: %s\n",curr_client.req->data);
					}
				}
			}
			handleCurrentActivity();
		}
	}
}
int is_tls_connection(int client_fd) {
    char packet_buff[6];
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

void runClientConnection(u_int8_t is_tls, int32_t sock){
	
	exit_func_for_this_module = exit_function_client;
	use_tls=is_tls;
	socklenpointer=sizeof(curr_client.client_addr);
	curr_client.socket=sock;
	curr_client.is_tls=is_tls_connection(curr_client.socket);
	if(curr_client.is_tls){
		if(logging>0){
			fprintf(logstream,"Its tls we do eht!\n");
		}
		InitializeSSL();
		init_openssl_libs_server_side();
		convert_server_con_to_ssl(&curr_client.ctx_if_ssl,curr_client.socket);
	}
	req.header=&header;
	curr_client.req= &req;
	handleConnection();

	getpeername(curr_client.socket , (struct sockaddr*)&curr_client.client_addr , (socklen_t*)&socklenpointer);
	if(logging>0){
		fprintf(logstream,"Disconnexao cliente de:\n%s:%d\n" ,
					inet_ntoa(curr_client.client_addr.sin_addr) ,
					ntohs(curr_client.client_addr.sin_port));
			   
	}
			 
	exit_function_client();

}
