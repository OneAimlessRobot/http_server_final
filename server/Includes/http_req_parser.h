#ifndef HTTP_REQ_PARSER_H
#define HTTP_REQ_PARSER_H

#define ALLOWED_HTTP_REQUESTS_STRING "GET, HEAD, POST"
typedef enum {GET,HEAD,POST,INVALID,MAX_REQUEST_TYPE} http_req_type;


extern const char* request_const_name_arr[MAX_REQUEST_TYPE];


typedef struct http_header_field{
	char* field[2];

}http_header_field;

typedef struct http_header{

	http_req_type type;
	char target[FIELDSIZE],
		version[FIELDSIZE],
		host[FIELDSIZE],
		*split_encoding[ARGVMAX],
		encoding[FIELDSIZE],
		encodingcopy[FIELDSIZE],
		origin[FIELDSIZE],
		mimetype[FIELDSIZE],
		range_string[FIELDSIZE],
		range[FIELDSIZE],
		content_type_raw[FIELDSIZE],
		content_type_copy[FIELDSIZE],
		*content_type_split[ARGVMAX],
		content_type[FIELDSIZE],
		*form_boundary_split[2],
		form_boundary[FIELDSIZE],
		cloudfare_ip[FIELDSIZE],
		os[FIELDSIZE];
	u_int8_t is_partial;
	int64_t range_number;

}http_header;

typedef struct http_request{

	char data[BUFFSIZE],
	processed_data[BUFFSIZE];
	u_int64_t data_size;
	http_header* header;


}http_request;

http_header_field* make_http_header_field_arr(char *strarr[ARGVMAX]);

void print_field_arr(FILE* fstream,http_header_field* fields);

void spawnHTTPRequest(void);

void print_http_req(FILE* fstream);

#endif
