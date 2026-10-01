#ifndef THE_REQUEST_STATE_STRUCT_H
#define THE_REQUEST_STATE_STRUCT_H


typedef struct client{
	
	char peerbuff[MAX_RECV_REQ_BUFFSIZE],
		filepath[PATHSIZE*5],
		target[PATHSIZE*3],
		relative_file_path[PATHSIZE*3],
		redirect_file_path[PATHSIZE*4],
		response_mimetype[128],
		sent_header_buff[HEADER_SEND_SIZE];
	http_request* req; 
	int32_t socket, fd;
	int64_t recv_size,send_size;
	u_int8_t is_tls,
			enable_first_time_in_upload,
			compress,
			is_non_prettifyable,
			is_not_found,
			would_cache,
			should_cache,
			should_redirect;
	int64_t mimetype_table_cell,
				post_request_table_cell,
				fetch_request_table_cell;
	struct stat * the_stat_struct;
	SSL* ctx_if_ssl;
	struct sockaddr_in client_addr;
	generating_func_t generating_func;
	
}client;

#endif
