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
		snprintf(outpathbuff,sizeof(outpathbuff)-1,"%s%s%lx.%s",tmp_file_dir,buff_for_dotted_target,request_id,servComp->fileExt);
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
		
			fprintf(logstream,"%s\n",cmd);
		
		}
		
		char* argv[ARGVMAX]={0};
		make_str_arr(cmd," ",argv, 4,1);
		if(logging>6){
			print_string_arr(logstream,argv);
		}
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

static void send_header(void){

	int64_t fileSize=0;
	uint64_t header_size=0;
	const char* the_mime_type= curr_client.is_non_prettifyable
								?
								robotsMimetype
								:
								curr_client.is_not_found?
										get_mime_type(notFoundDefaultTarget,NULL)
											:
											curr_client.response_mimetype;
	
	lseek(comp_fd,0,SEEK_END);
	
	fileSize=lseek(comp_fd,0,SEEK_CUR);
	
	lseek(comp_fd,0,SEEK_SET);
	
	curr_client.req->header->range_end = curr_client.req->header->range_end<=0
					?
					fileSize-1
					:
					curr_client.req->header->range_end;
	
	curr_client.send_size = curr_client.req->header->range_end-curr_client.req->header->range_start+1;
	
	
	header_size = ((!curr_client.is_not_found)?
		((comp_enable)?
			((use_chunked)?
				((!curr_client.req->header->is_partial)?
					fillUpChunkedHeaderComp(the_mime_type,
											servComp->fileExt)
					:
					fillUpChunkedPartialHeaderComp(the_mime_type,
													servComp->fileExt,
													curr_client.req->header->range_start,
													curr_client.req->header->range_end,
													fileSize)
				)
				:
				((!curr_client.req->header->is_partial)?
					fillUpNormalHeaderComp(the_mime_type,
												servComp->fileExt,
												fileSize)
					:
					fillUpNormalPartialHeaderComp(the_mime_type,
													servComp->fileExt,
													curr_client.req->header->range_start,
													curr_client.req->header->range_end,
													curr_client.send_size,
													fileSize)
				)
			)
			:
			((use_chunked)?
				((!curr_client.req->header->is_partial)?
					fillUpChunkedHeader(the_mime_type)		
					:
					fillUpChunkedPartialHeader(the_mime_type,
													curr_client.req->header->range_start,
													curr_client.req->header->range_end,
													fileSize)
				)
				:
				((!curr_client.req->header->is_partial)?
					fillUpNormalHeader(the_mime_type,
										fileSize)		
					:
					fillUpNormalPartialHeader(the_mime_type,
													curr_client.req->header->range_start,
													curr_client.req->header->range_end,
													curr_client.send_size,
													fileSize)
				)
			)
		)
		:
		fillUpFileNotFoundHeader(fileSize,the_mime_type));
	if(logging > 5){
		print_response_header(curr_client.sent_header_buff);
	}
	send_wrapper(curr_client.sent_header_buff,header_size);

}
void sendResource(int is_head){
	if(curr_client.should_redirect){
		
		
		if(logging){
			fprintf(logstream,"WILL redirect!\n");
		}
		fillUpRedirectHeader(curr_client.redirect_file_path,
								curr_client.req->header->upgrade_to_tls_value && !curr_client.is_tls);
		int header_size=strlen(curr_client.sent_header_buff);
		if(logging > 5){
			print_response_header(curr_client.sent_header_buff);
		}
		send_wrapper(curr_client.sent_header_buff,header_size);
		if(logging){
			fprintf(logstream,"Done!\n");
		}
		return;
		
		
	}
	if(curr_client.fd>=0){
		close(curr_client.fd);
	}
	
	/*
		because it starts at zero
		it means we tried to initialize it in a special handler
		(The generator funcs)
		and it ended up as -1
		so we are done
		closing it doesnt change the variable's value
	*/
	
	if(curr_client.fd<0){
		goto file_not_found;
	}
	
	/*
		otherwise
		it is suposedly a normal file
		and we should try to stat it
	*/
	int stat_result=check_stat_of_file(curr_client.filepath,curr_client.the_stat_struct,logging,logstream);
	if(stat_result){
		goto file_not_found;
	}
	if(logging > 5){
		fprintf(logstream,"Fetching %s...\nRecurso pedido por cliente de:\n%s:%hu\nNumero de request: %lu\n\n",
								curr_client.filepath,
								inet_ntoa(curr_client.client_addr.sin_addr) ,
								ntohs(curr_client.client_addr.sin_port),
								request_id);
		
	}
	
	
	
	snprintf(curr_client.response_mimetype,
						sizeof(curr_client.response_mimetype)-1,
						"%s",
						get_mime_type(curr_client.filepath,&curr_client.mimetype_table_cell));
	/*
		we will cache if:
		- file type allows it
		- the max cacheable size is superior to:
			- file size if full request
			- byte rage if partial
	*/
	curr_client.should_cache = (get_mime_type_cacheability(curr_client.mimetype_table_cell)
									&&
								(
								get_mime_type_max_cacheable_size(curr_client.mimetype_table_cell)
									>=
								(
								curr_client.req->header->range_end<=0
									?
								(curr_client.the_stat_struct->st_size)
									:
								curr_client.req->header->range_end-curr_client.req->header->range_start)
								));			
	
	if(curr_client.should_cache){
		
		build_last_modified_buff(curr_client.filepath);
		curr_client.would_cache=!strncmp(ETag,curr_client.req->header->if_none_match,sizeof(ETag)-1);
		if(WILL_CACHE){
			if(logging){
				fprintf(logstream,"WILL cache!\nThe mimetype is: %s\nOur ETag: %s\nTheir ETag: %s\n",
					curr_client.response_mimetype,
					ETag,
					curr_client.req->header->if_none_match);
			}
			fillUpNotModifiedHeader();
			int header_size=strlen(curr_client.sent_header_buff);
			if(logging > 5){
				print_response_header(curr_client.sent_header_buff);
			}
			send_wrapper(curr_client.sent_header_buff,header_size);
			if(logging){
				fprintf(logstream,"Done!\n");
			}
			return;
		}
		
	}
	else if(logging){
		fprintf(logstream,"Will NOT cache!\n");
	}		
		
	u_int8_t compressibility_selected=curr_client.compress;
	
	if(compressibility_selected){
		/*
			we will compress if:
			- file type allows it
			- the max cacheable size is superior to:
				- file size if full request
				- request is NOT partial
		*/
		curr_client.compress = (curr_client.compress 
									&& 
							get_mime_type_compressibility(curr_client.mimetype_table_cell)
									&&
							(get_mime_type_max_compress_size(curr_client.mimetype_table_cell)
									>=
							(curr_client.the_stat_struct->st_size))
									&&
							(curr_client.req->header->range_end<=0));

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
	
	/*
		- put non null argument to compress a relative path to server root
		- put null to compress current chambered file path
	*/
	
	if(compressFile("")<0){

		if(logging){

			fprintf(logstream,"Erro na compressão %s...\n",strerror(errno));
		}
		file_not_found:
			curr_client.compress = 0;
			if(compressFile(notFoundDefaultTarget)<0){

				if(logging){

					fprintf(logstream,"Erro na compressão %s...\n",strerror(errno));
				}
				send_internal_error_response();
				goto final_cleanup_label;
			}
			else{
				curr_client.is_not_found=1;
				use_chunked=0;
				
				if(logging){
				
					fprintf(logstream,"Enviando resposta 404...\n");
				
				}
			}
	}
	send_header();
	if(!is_head){
		push_bytes();
	}
	final_cleanup_label:
		final_cleanup();
		
		if(logging){
			fprintf(logstream,"Done!\n");
		}
}
