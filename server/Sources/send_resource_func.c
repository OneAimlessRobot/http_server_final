#include "../Includes/preprocessor.h"
#include "../Includes/client.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_response_aux.h"
#include "../Includes/server_vars.h"
#include "../Includes/io_ops.h"
#include "../Includes/send_resource_func.h"


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

static int compressFile(char* target){
	char* ptr = NULL;
	if(!strlen(target)){
		ptr = curr_client.filepath;
	}
	else{
		
		memset(inpathbuff,0,sizeof(inpathbuff)-1);
		snprintf(inpathbuff,sizeof(inpathbuff)-1,"%s%s",currDir,target);
		ptr= inpathbuff;
	}
	if(curr_client.compress){
		memset(buff_for_dotted_target, 0, sizeof(buff_for_dotted_target));
		snprintf(buff_for_dotted_target, sizeof(buff_for_dotted_target), "%s",curr_client.relative_file_path);
		replaceStringCharacter(&buff_for_dotted_target[1],'/','.');
		memset(outpathbuff,0,sizeof(outpathbuff)-1);
		snprintf(outpathbuff,sizeof(outpathbuff)-1,"%s%s%lu.%s",tmp_file_dir,buff_for_dotted_target,request_id,servComp->fileExt);
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
		snprintf(cmd,sizeof(cmd)-1,servComp->comp_cmd,servComp->cmdTool,compression_level,ptr);
		
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

static void send_header(int notfound){

	int64_t fileSize=0;
	const char* the_mime_type= curr_client.is_robots
								?
								robotsMimetype
								:
								get_mime_type(notfound
											?
											notFoundDefaultTarget
											:
											curr_client.relative_file_path);
	
	lseek(comp_fd,0,SEEK_END);
	
	fileSize=lseek(comp_fd,0,SEEK_CUR);
	
	lseek(comp_fd,0,SEEK_SET);
	
	(!notfound)?
		((comp_enable)?
			((use_chunked)?
				((!curr_client.req->header->is_partial)?
					fillUpChunkedHeaderComp(
											curr_client.sent_header_buff,
											the_mime_type,
											servComp->fileExt)
					:
					fillUpChunkedPartialHeaderComp(curr_client.sent_header_buff,
													the_mime_type,
													servComp->fileExt,
													curr_client.req->header->range_number,
													fileSize)
				)
				:
				((!curr_client.req->header->is_partial)?
					fillUpNormalHeaderComp(curr_client.sent_header_buff,
												the_mime_type,
												servComp->fileExt,
												fileSize)
					:
					fillUpNormalPartialHeaderComp(curr_client.sent_header_buff,
													the_mime_type,
													servComp->fileExt,
													curr_client.req->header->range_number,
													fileSize)
				)
			)
			:
			((use_chunked)?
				((!curr_client.req->header->is_partial)?
					fillUpChunkedHeader(curr_client.sent_header_buff,
										the_mime_type)		
					:
					fillUpChunkedPartialHeader(curr_client.sent_header_buff,
												the_mime_type,
												curr_client.req->header->range_number,
												fileSize)
				)
				:
				((!curr_client.req->header->is_partial)?
					fillUpNormalHeader(curr_client.sent_header_buff,
										the_mime_type,
										fileSize)		
					:
					fillUpNormalPartialHeader(curr_client.sent_header_buff,
												the_mime_type,
												curr_client.req->header->range_number,
												fileSize)
				)
			)
		)
		:
		fillUpFileNotFoundHeader(curr_client.sent_header_buff,fileSize,the_mime_type);
	int header_size=strlen(curr_client.sent_header_buff);
	if(logging>50){
		print_response_header(curr_client.sent_header_buff);
	}
	send_wrapper(curr_client.sent_header_buff,header_size);

}
void sendResource(int is_head){
	if(curr_client.fd<0){
		return;
	}
	close(curr_client.fd);
	if(logging > 5){
		fprintf(logstream,"Fetching %s...\nRecurso pedido por cliente de:\n%s:%hu\nNumero de recurso: %lu",curr_client.filepath,
								inet_ntoa(curr_client.client_addr.sin_addr) ,
								ntohs(curr_client.client_addr.sin_port),
								request_id);
		
	}
	if(logging>0){
			   
	}
	u_int8_t notfound=0,
			compressibility_selected=curr_client.compress;

	if(compressibility_selected){
		curr_client.compress = (curr_client.compress && get_mime_type_compressibility(curr_client.filepath));

		if(logging > 5){
			fprintf(logstream,"Server will use compression!\n"
								"%s is %s\n",
								curr_client.filepath,
								curr_client.compress?
											"Compressible!"
											:
											"Non compressible!");
		}
		
	}
	if(compressFile("")<0){

		if(logging){

			fprintf(logstream,"Erro na compressão %s...\n",strerror(errno));
		}
		curr_client.compress = 0;
		if(compressFile(notFoundDefaultTarget)<0){

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
	send_header(notfound);
	if(!is_head){
		push_bytes();
	}
	final_cleanup(notfound);
	
	if(logging){
		fprintf(logstream,"Done!\n");
	}
}
