#ifndef SEND_EXTENDER_H
#define SEND_EXTENDER_H


u_int8_t comp_enable = 0;
int comp_fd = -1;

static void push_bytes(void);


static char* compressionCmd = "%s -c -%d %s";

static char inpathbuff[PATHSIZE*6]={0},
		outpathbuff[PATHSIZE*6+10]={0},
		cmd[PATHSIZE*30]={0},
		buff_for_dotted_target[PATHSIZE*3]={0};


static void push_bytes(void){
	
	((use_chunked)?
		sendallchunkedfd(&curr_client,comp_fd,curr_client.req->header->is_partial?
							curr_client.req->header->range_number
							:
							0)
		:
		sendallfd(&curr_client,comp_fd,curr_client.req->header->is_partial?
							curr_client.req->header->range_number
							:
							0));
	
}

static void final_cleanup(int notfound){
	
	close(comp_fd);

	if(comp_enable&&!notfound){
		if(logging>3){
			fprintf(logstream,"Trying to delete %s...\n",outpathbuff);
		}
		remove(outpathbuff);
	}

}

	
#endif
