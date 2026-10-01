#ifndef SEND_EXTENDER_H
#define SEND_EXTENDER_H


u_int8_t comp_enable = 0;
int comp_fd = -1;

static void push_bytes(void);



static char inpathbuff[PATHSIZE*6]={0},
		outpathbuff[PATHSIZE*6+10]={0},
		cmd[PATHSIZE*30]={0},
		buff_for_dotted_target[PATHSIZE*3]={0};


static void push_bytes(void){
	
	((use_chunked)?
		sendallchunkedfd(&curr_client,comp_fd,curr_client.req->header->is_partial?
							curr_client.req->header->range_start
							:
							0,
							curr_client.send_size)
		:
		sendallfd(&curr_client,comp_fd,curr_client.req->header->is_partial?
							curr_client.req->header->range_start
							:
							0,
							curr_client.send_size));
	
}

static void final_cleanup(void){
	
	close(comp_fd);

	if(!curr_client.is_not_found){
		if(comp_enable){
			if(logging>3){
				fprintf(logstream,"Trying to delete %s...\n",outpathbuff);
			}
			remove(outpathbuff);
		}
		if(curr_client.generating_func){
			/*
			 * 
			 * delete generator tmp file
			 * (from dir/find/rss/etc requests)
			 * if there is one
			 * 
			 */
			if(logging>3){
				fprintf(logstream,"Trying to delete %s...\n",curr_client.filepath);
			}
			remove(curr_client.filepath);
		}
	}
}

	
#endif
