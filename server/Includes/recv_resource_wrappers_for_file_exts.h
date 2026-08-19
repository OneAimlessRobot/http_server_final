#ifndef RECV_EXTENDER_H
#define RECV_EXTENDER_H

int recv_fd = -1;
char upload_file_dest_dir[PATHSIZE*5]={0};
static void pull_bytes(void);


static void pull_bytes(void){
	
	pullallfd(recv_fd);
	
}


#endif