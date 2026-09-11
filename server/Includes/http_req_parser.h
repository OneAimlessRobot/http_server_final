#ifndef HTTP_REQ_PARSER_H
#define HTTP_REQ_PARSER_H


#define REQUEST_SECTION_DELIMITER_STRING "\r\n\r\n"
#define DOMAIN_NAME "lenasattic.ddns.net"



extern char* reserved_characters[],
	* reserved_characters_for_special[];

extern const char* request_const_name_arr[MAX_REQUEST_TYPE];


http_header_field* make_http_header_field_arr(char *strarr[ARGVMAX]);

void print_field_arr(FILE* fstream,http_header_field* fields);

void spawnHTTPRequest(void);

void print_http_req(FILE* fstream);

#endif
