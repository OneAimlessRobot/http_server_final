#ifndef CUSTOM_STRUCTS_H
#define CUSTOM_STRUCTS_H


typedef struct rcon_arg_line{

	char* default_value;
	char* the_actual_value;

}rcon_arg_line;


typedef struct cfg_field{
        char* mem;
        char* field[2];
        u_int8_t is_accessed;

}cfg_field;



typedef struct compMethod{
	char *cmdTool,
		*fileExt,
		*encodingExt,
		*comp_cmd;


}compMethod;

typedef struct timer_struct_t{
	time_t timeur;
	struct tm* time_struct_ptr;

}timer_struct_t;

typedef struct {
    const char *extension,
				*mime_type;
	u_int8_t allows_compression,
			to_be_cached;

} MimeEntry;

typedef struct http_header_field{
	char* field[2];

}http_header_field;

typedef struct http_header{

	http_req_type type;
	char target[FIELDSIZE],
		version[FIELDSIZE],
		host[FIELDSIZE],
		specific[FIELDSIZE],
		content_length[FIELDSIZE],
		*split_encoding[ARGVMAX],
		encoding[FIELDSIZE],
		encodingcopy[FIELDSIZE],
		origin[FIELDSIZE],
		if_none_match[ETAG_SIZE],
		mimetype[FIELDSIZE],
		is_mobile[FIELDSIZE],
		user_agent[FIELDSIZE],
		range_string[FIELDSIZE],
		range[FIELDSIZE],
		content_type_raw[FIELDSIZE],
		content_type_copy[FIELDSIZE],
		*content_type_split[ARGVMAX],
		content_type[FIELDSIZE],
		*form_boundary_split[2],
		form_boundary[FIELDSIZE],
		cloudfare_ip[FIELDSIZE],
		refer[FIELDSIZE],
		accept[FIELDSIZE],
		accept_language[FIELDSIZE],
		sec_fetch_site[FIELDSIZE],
		sec_fetch_mode[FIELDSIZE],
		sec_fetch_dest[FIELDSIZE],
		sec_fetch_user[FIELDSIZE],
		sec_ch_ua[FIELDSIZE],
		sec_ch_ua_platform[FIELDSIZE],
		cookie[FIELDSIZE];
		
	u_int8_t is_partial,
			is_mobile_value;
	int64_t range_start,range_end;

}http_header;

typedef struct http_request{

	char form[MAX_RECV_REQ_BUFFSIZE],
		processed_form[MAX_RECV_REQ_BUFFSIZE],
		final_form[MAX_RECV_REQ_BUFFSIZE];
	char begin_data[MAX_RECV_REQ_BUFFSIZE];
	int64_t data_size,
			begin_data_size,
			form_size;
	u_int8_t is_a_form;
	http_header* header;


}http_request;


typedef struct client{
	
	char peerbuff[MAX_RECV_REQ_BUFFSIZE],
		og_filepath[PATHSIZE*5],
		filepath[PATHSIZE*5],
		target[PATHSIZE*3],
		relative_file_path[PATHSIZE*3],
		response_mimetype[128],
		sent_header_buff[HEADER_SEND_SIZE];
	http_request* req; 
	int32_t socket, fd;
	int64_t recv_size,send_size;
	u_int8_t is_tls,
			enable_first_time_in_upload,
			compress,
			is_directory,
			is_non_prettifyable,
			is_not_found,
			would_cache,
			should_cache;
	u_int64_t mimetype_table_cell;
	struct stat * the_stat_struct;
	SSL* ctx_if_ssl;
	struct sockaddr_in client_addr;
	generating_func_t generating_func;
	
}client;


#endif
