#ifndef HTTP_REQ_PARSER_H
#define HTTP_REQ_PARSER_H
typedef enum {GET,POST,INFO,INVALID} http_req_type;

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
		mimetype[FIELDSIZE],
		content_type_raw[FIELDSIZE],
		content_type_copy[FIELDSIZE],
		*content_type_split[ARGVMAX],
		content_type[FIELDSIZE],
		*form_boundary_split[2],
		form_boundary[FIELDSIZE],
		os[FIELDSIZE];
	int content_length;

}http_header;

typedef struct http_request{

	http_header* header;
	u_int64_t data_size;
	char* data;
	


}http_request;

http_header_field* make_http_header_field_arr(char *strarr[ARGVMAX]);

void print_field_arr(FILE* fstream,http_header_field* fields);

void spawnHTTPRequest(char *buff,http_header* header,http_request* reqresult);

void print_http_req(FILE* fstream,http_request req);

#endif
