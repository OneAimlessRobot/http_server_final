#include "../Includes/preprocessor.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/http_response_aux.h"
#include "../Includes/handlecustom.h"
#include "../Includes/client.h"
#include "../Includes/load_find_results.h"
#include "../Includes/load_html.h"
#include "../Includes/ssl_things.h"
#include "../Includes/load_rss.h"
#include "../Includes/load_date.h"
#include "../Includes/php_proxy.h"
#include "../Includes/rcon_proxy.h"
#include "../Includes/send_resource_func.h"
#include "../Includes/recv_resource_func.h"
#include "../Includes/server_vars.h"
#include "../Includes/io_ops.h"
#include "../Includes/txt_sending.h"


static http_header header = {0};
static http_request req = {0};

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

static int generating_func_wrapper( char* arg){
	curr_client.generating_func(arg);
	return 0;
}

static void head_and_get_wrapper(void){
	(curr_client.generating_func!=NULL)?
			generating_func_wrapper(curr_client.target)
			:
			snprintf(curr_client.filepath,sizeof(curr_client.filepath)-1,
						"%s%s",currDir,curr_client.target);
}
static void process_req_data(void){
	
	url_decode(curr_client.req->processed_form, curr_client.req->final_form,reserved_characters_for_special);
	
	if(logging>3){
		fprintf(logstream,"The data is: %s\n", curr_client.req->final_form);
	}
}
static void process_header_target(void){
	
	url_decode(curr_client.req->header->target, curr_client.target,reserved_characters);
	snprintf(curr_client.relative_file_path,sizeof(curr_client.relative_file_path),"%s",curr_client.target);
	
	if(logging>3){
		fprintf(logstream,"The target is: %s\n", curr_client.target);
		fprintf(logstream,"The relative file path here is: %s\n", curr_client.relative_file_path);
	}
}
static void process_req_switch_block(void){
	

	switch(curr_client.req->header->type){
		case GET:
			head_and_get_wrapper();
		break;
		case HEAD:
			head_and_get_wrapper();
		break;
		case POST:
			if(isCustomPostReq(curr_client.req)){
				handleCustomPostReq(curr_client.req);
				fprintf(logstream,"The relative file path here is: %s\n"
								"The processed target here is: <%s>\n",
									curr_client.relative_file_path,
									curr_client.req->processed_form);
				
				if(curr_client.req->is_a_form){
					process_req_data();
				}
			}
			
			(curr_client.generating_func!=NULL)?
				generating_func_wrapper(curr_client.req->final_form):
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
static void figure_out_compression(void){
	
	for(int i=0;methods_for_compression[i].encodingExt;i++){
			
		if(findInStringArr(curr_client.req->header->split_encoding,methods_for_compression[i].encodingExt)>=0){
			servComp = &methods_for_compression[i];
			curr_client.compress=use_compression;
			break;
		}
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
	figure_out_compression();
	
	process_header_target();
	
	u_int8_t is_head = (curr_client.req->header->type==HEAD);
	
	
	
	if(curr_client.req->is_a_form){
		if((curr_client.is_directory=isDirectory(curr_client.target))){
			curr_client.generating_func = generateDirListing;
		}
		else if(isRssFeed(curr_client.target)){
			curr_client.generating_func = generateRss;
		}
		else if(isDateReq(curr_client.target)){
			curr_client.generating_func = the_time_func;
		}
		else if(isPHPRequest(curr_client.target)){
			curr_client.generating_func = generatePHPResult;
		}
		else if(isText(curr_client.target)){
			curr_client.generating_func = generateText;
		}
		else if(isRconRequest(curr_client.target)){
			
			curr_client.generating_func = generateRconResult;
		}
		else if(isFindRequest(curr_client.target)){
			
			curr_client.generating_func = generateFindResult;
		}
	}
	process_req_switch_block();
	
	if(curr_client.req->is_a_form){
		sendResource(is_head);
		if(curr_client.generating_func){
			remove(curr_client.filepath);
		}
	}
	else{
		recvResource();
	}
}
void handleConnection(void){
	int64_t obtained_read_size=readall(&curr_client,curr_client.peerbuff);
	
	curr_client.recv_size = obtained_read_size;
	if(curr_client.recv_size>0 
		||
		curr_client.recv_size==-2){
		if(errno == ECONNRESET){
			exit_function_client();
		}
		spawnHTTPRequest();
		if(logging>4){
			print_http_req(logstream);
		}
		handleCurrentActivity();
	}
}
int is_tls_connection(int client_fd) {
    char packet_buff[6];
    ssize_t n = readsome(client_fd, (char*)packet_buff, sizeof(packet_buff), MSG_PEEK);
    
    if( n< 6){
			return 0;
	}
	if(((packet_buff[0] & 0x80) == 0x80)
		&&
		(((packet_buff[0] & 0x7f) << 8 | packet_buff[1]) > 9)
		&&
		(packet_buff[2] == 0x01)){

		return 1;
	
	}
	
    //https://stackoverflow.com/questions/16194050/tls-protocol-detection-by-using-client-hello-message
	//Check 22 and version info 0300 0301 or 0302
	if (packet_buff[0] == 0x16 && packet_buff[1] == 0x03
	  && (packet_buff[2] == 0x00 || packet_buff[2] == 0x01 || packet_buff[2] == 0x02)){
		if (packet_buff[5] == 1){
			return 2;
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
