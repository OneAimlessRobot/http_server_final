#include "../Includes/preprocessor.h"
#include "../Includes/client.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_response_aux.h"
#include "../Includes/server_vars.h"
#include "../Includes/send_resource_func.h"
#include "../Includes/io_ops.h"

#include "../Includes/recv_resource_wrappers_for_file_exts.h"
static char* the_location_str = NULL,
	* file_name_base_here = NULL;
static u_int64_t the_location_index = 0;
static void send_header(void){

	const char* the_mime_type= get_mime_type(curr_client.target,NULL);
	fillUploadHeader(the_mime_type,the_location_str,file_name_base_here);
	int header_size=strlen(curr_client.sent_header_buff);
	if(logging > 5){
		print_response_header(curr_client.sent_header_buff);
	}
	send_wrapper(curr_client.sent_header_buff,header_size);

}
void recvResource(void){
	the_location_str = (char*)get_upload_target(curr_client.req->header->second_target, &the_location_index);
	file_name_base_here = basename(curr_client.target);
	snprintf(upload_file_dest_dir,sizeof(upload_file_dest_dir)-1,"%s%s/%s/%s",currDir,defaultTarget,the_location_str,file_name_base_here);
	if(logging>4){
		fprintf(logstream,"The full path will be: %s\n",upload_file_dest_dir);
	}
	/*
	 * 
	 * lets null out the last fwd slash
	 * In order to get the actual dir
	 * We need to append the target back to it
	 * In another aux string though
	 * 
	 */
	char* dir_ptr= strrchr(upload_file_dest_dir,'/');
	char prev_c = *(dir_ptr+1);
	*(dir_ptr+1) = '\0';
	if(logging>4){
		fprintf(logstream,"We are building this directory in case it does not exist!: \n"
						"%s\n",upload_file_dest_dir);
	}
	
	super_mkdir(upload_file_dest_dir);
	*(dir_ptr+1) = prev_c;
	
	
	
	if(logging>4){
		fprintf(logstream,"We will upload to this location!: \n"
						"%s\n",upload_file_dest_dir);
	}
	
	recv_fd=open(upload_file_dest_dir,O_TRUNC|O_WRONLY|O_CREAT,0777);
	if(recv_fd<0){
		if(logging>4){
			fprintf(logstream,"Upload destination fd could not be created!\n"
							"Error: %s\n",
							strerror(errno));
		}
		return;
	}
	if(logging>4){
		fprintf(logstream,"Upload will happen!!!\n");
	}
	pull_bytes();
	
	send_header();
	close(recv_fd);
	if(logging){
		fprintf(logstream,"Done!\n");
	}
}
