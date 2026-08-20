#include "../Includes/preprocessor.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include "../Includes/buffSizes.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/http_req_parser.h"

char* reserved_characters[]={
	"|",
	">",
	"<",
	"\\",
	"\""
	":",
	NULL
},
* reserved_characters_for_special[]={
	">",
	"<",
	"\\",
	"\""
	":",
	NULL
};

const char* request_const_name_arr[MAX_REQUEST_TYPE] = {
	
	"GET",
	"HEAD",
	"POST",
	"INVALID"
	
};

const browser_struct_t request_const_browser_name_arr[MAX_BROWSER_MODE]={
	
	{"Chrome",{1,0}},
	{"Firefox",{1,1}},
	{"Safari",{1,1}},
	{"Invalid",{0,0}}
};

static http_req_type getREQtypeFromString(char* str){
	if(!str){
		return INVALID;
	}
	if(!strnlen(str,FIELDSIZE)){
		return INVALID;
	}
	for(http_req_type i=GET;i<sizeof(request_const_name_arr);i++){
		
		if(!strncmp(str,request_const_name_arr[i],FIELDSIZE)){
				
				return i;
		}
		
	}
	return INVALID;
}
static void fillOutBrowserModeInREQ(http_request*req){
	
	for(browser_mode_t i=(browser_mode_t)0;i<BROWSER_INVALID;i++){
		
		if(strstr(req->header->user_agent,request_const_browser_name_arr[i].browser_name)){
				
				req->browser_mode=i;
				return;
		}
		
	}
	req->browser_mode=BROWSER_INVALID;
}
void print_field_arr(FILE* fstream,http_header_field* fields){

	for (int i=0;fields[i].field[0];i++){
		
		fprintf(fstream,"Field %d:\nField type: %s\nField value: %s\n",i,fields[i].field[0],fields[i].field[1]);

	}

}
void print_http_req_header(FILE* fstream){

	fprintf(fstream,"\n\nHTTP REQUEST HEADER:\n"
					"______________________________________\n"
					"______________________________________\n\n\n"
					"Type: %s\n"
					"Origin: %s\n"
					"Specific: %s\n"
					"Content-Length: %s\n"
					"Target: %s\n"
					"Version: %s\n"
					"Host: %s\n"
					"User-Agent: %s\n"
					"mimetype: %s\n"
					"OS: %s\n"
					"Mobile? (raw): %s\n"
					"Mobile? (captured value): %hhu\n"
					"Range (string): |%s|\n"
					"Range: bytes=%s-\n"
					"CF-Connecting-IP: %s\n"
					"Accepted-encoding: %s\n"
					"Accepted-encoding (split):\n\n",
							request_const_name_arr[curr_client.req->header->type],
							curr_client.req->header->origin,
							curr_client.req->header->specific,
							curr_client.req->header->content_length,
							curr_client.req->header->target,
							curr_client.req->header->version,
							curr_client.req->header->host,
							curr_client.req->header->user_agent,
							curr_client.req->header->mimetype,
							curr_client.req->header->os,
							curr_client.req->header->is_mobile,
							curr_client.req->header->is_mobile_value,
							curr_client.req->header->range_string,
							curr_client.req->header->is_partial?curr_client.req->header->range:"NO_RANGE_SPECIFIED",
							curr_client.req->header->cloudfare_ip,
							curr_client.req->header->encodingcopy);

		print_string_arr(fstream,curr_client.req->header->split_encoding);
		fprintf(fstream,"\nContent-Type (raw): %s\n"
						"Content-Type (actual): %s\n"
						"Form-boundary: %s\n"
						"Content-Type (split):\n\n",
							curr_client.req->header->content_type_copy,
							curr_client.req->header->content_type,
							curr_client.req->header->form_boundary);
		print_string_arr(fstream,curr_client.req->header->content_type_split);
		fprintf(fstream,"\n\n"
					"______________________________________\n"
					"______________________________________\n\n"
					"END OF HTTP REQUEST HEADER!\n\n");
}
static char* find_field_value_in_field_arr(char* fieldName,http_header_field* fields){

	for (int i=0;fields[i].field[0];i++){
		
		if(!strcmp(fields[i].field[0],fieldName)){
			return fields[i].field[1];
		}
	}
	return "";

}

http_header_field* make_http_header_field_arr(char *strarr[ARGVMAX]) {
    int size= get_string_arr_size(strarr)+1;
    http_header_field* arr=malloc(size*sizeof(http_header_field));
    memset(arr,0,size*sizeof(http_header_field));
    for(int i=0;strarr[i]&&i<size-1;i++){
	
			splitString(strarr[i],":",arr[i].field);


	}
    arr[size-1]=(http_header_field){{NULL,NULL}};
	return arr;
}

static void spawnHTTPHeader(void){
	curr_client.req->header->type=INVALID;
	memset(curr_client.req->header->target,0,FIELDSIZE);
	memset(curr_client.req->header->origin,0,FIELDSIZE);
	memset(curr_client.req->header->content_length,0,FIELDSIZE);
	memset(curr_client.req->header->host,0,FIELDSIZE);
	memset(curr_client.req->header->mimetype,0,FIELDSIZE);
	memset(curr_client.req->header->specific,0,FIELDSIZE);
	memset(curr_client.req->header->is_mobile,0,FIELDSIZE);
	memset(curr_client.req->header->version,0,FIELDSIZE);
	memset(curr_client.req->header->os,0,FIELDSIZE);
	memset(curr_client.req->header->user_agent,0,FIELDSIZE);
	memset(curr_client.req->header->range,0,FIELDSIZE);
	memset(curr_client.req->header->range_string,0,FIELDSIZE);
	memset(curr_client.req->header->cloudfare_ip,0, FIELDSIZE);
	char* headerArr[ARGVMAX] = {0};
	char* firstHeaderArr[ARGVMAX] = {0};
	char test[sizeof(curr_client.peerbuff)+1] = {0};
	memcpy(test,curr_client.peerbuff,strlen(curr_client.peerbuff)+1);
	make_str_arr(test,"\r\n",headerArr,ARGVMAX);
	http_header_field* fieldarr= make_http_header_field_arr(headerArr);
	int firstHeaderLength=strlen(fieldarr[0].field[0]);
	char firstHeaderBuff[firstHeaderLength+1];
	memset(firstHeaderBuff,0,firstHeaderLength+1);
	memcpy(firstHeaderBuff,fieldarr[0].field[0],firstHeaderLength);
	make_str_arr(firstHeaderBuff," ",firstHeaderArr,ARGVMAX);
	curr_client.req->header->type=getREQtypeFromString(firstHeaderArr[0]);
	if(get_string_arr_size(firstHeaderArr)==1){
		memset(curr_client.req->header->target,0,FIELDSIZE);
		memset(curr_client.req->header->version,0,FIELDSIZE);
		strcpy(curr_client.req->header->target,defaultTarget);
		strcpy(curr_client.req->header->mimetype,defaultMimetype);
	}
	else{
		strcpy(curr_client.req->header->target,firstHeaderArr[1]);
		strcpy(curr_client.req->header->version,firstHeaderArr[2]);
		strcpy(curr_client.req->header->host,find_field_value_in_field_arr("Host",fieldarr));
		strcpy(curr_client.req->header->origin,find_field_value_in_field_arr("Origin",fieldarr));
		strcpy(curr_client.req->header->mimetype,defaultMimetype);
		strcpy(curr_client.req->header->os,find_field_value_in_field_arr("sec-ch-ua-platform",fieldarr));
		strcpy(curr_client.req->header->is_mobile,find_field_value_in_field_arr("sec-ch-ua-mobile",fieldarr));
		if(strnlen(curr_client.req->header->is_mobile,FIELDSIZE)>0){
			
			sscanf(curr_client.req->header->is_mobile,"?%hhu",&curr_client.req->header->is_mobile_value);
			
		}
		strcpy(curr_client.req->header->encoding,find_field_value_in_field_arr("Accept-Encoding",fieldarr));
		strcpy(curr_client.req->header->content_length,find_field_value_in_field_arr("Content-Length",fieldarr));
		strcpy(curr_client.req->header->user_agent,find_field_value_in_field_arr("User-Agent",fieldarr));
		fillOutBrowserModeInREQ(curr_client.req);
		strcpy(curr_client.req->header->cloudfare_ip,find_field_value_in_field_arr("CF-Connecting-IP",fieldarr));
		strcpy(curr_client.req->header->specific,find_field_value_in_field_arr("Specific",fieldarr));
		strcpy(curr_client.req->header->encodingcopy,curr_client.req->header->encoding);
		make_str_arr(curr_client.req->header->encoding,",",curr_client.req->header->split_encoding,ARGVMAX);
		strcpy(curr_client.req->header->content_type_raw,find_field_value_in_field_arr("Content-Type",fieldarr));
		
		
		if(strlen(curr_client.req->header->content_type_raw)){
			strcpy(curr_client.req->header->content_type_copy,curr_client.req->header->content_type_raw);
			make_str_arr(curr_client.req->header->content_type_raw,"; ",curr_client.req->header->content_type_split,ARGVMAX);
			
			strcpy(curr_client.req->header->content_type,curr_client.req->header->content_type_split[0]);
			if(!curr_client.req->header->content_type_split[1]){
				memset(curr_client.req->header->form_boundary,0,FIELDSIZE);
			}
			else{
				splitString(curr_client.req->header->content_type_split[1],"=", curr_client.req->header->form_boundary_split);
				strcpy(curr_client.req->header->form_boundary,curr_client.req->header->form_boundary_split[1]);
			
			}
		}
	
		else{
			curr_client.req->header->content_type_split[0]=NULL;
			curr_client.req->header->form_boundary_split[0]=NULL;
			memset(curr_client.req->header->content_type_raw,0,FIELDSIZE);
			memset(curr_client.req->header->content_type_copy,0,FIELDSIZE);
			memset(curr_client.req->header->content_type,0,FIELDSIZE);
		}
	}
	curr_client.req->header->range_number= 0;
	if((curr_client.req->header->is_partial=(!ignore_partial))){
		strcpy(curr_client.req->header->range_string,find_field_value_in_field_arr("Range",fieldarr));
		curr_client.req->header->range_number= 0;
		if((curr_client.req->header->is_partial=(strlen(curr_client.req->header->range_string)>0))){
			sscanf(curr_client.req->header->range_string," bytes=%ld-",&curr_client.req->header->range_number);
			
		}
	}
	snprintf(curr_client.req->header->range,FIELDSIZE-1,"%ld",curr_client.req->header->is_partial?curr_client.req->header->range_number:-1);
	free(fieldarr);
	
}

void spawnHTTPRequest(void){
	char* split_one[2];
	
	printf("_________________________________\n"
			"Raw peer buff:\n"
			"_________________________________\n"
			"%s\n"
			"_________________________________\n"
			"_________________________________\n",curr_client.peerbuff);
			
		
	splitString(curr_client.peerbuff,REQUEST_SECTION_DELIMITER_STRING,split_one);



	spawnHTTPHeader();
	if(!split_one[1]){
		curr_client.req->data_size=1;
	}
	else{
			u_int8_t is_a_form = curr_client.req->is_a_form = (strlen(curr_client.req->header->specific)==0);
			
			int64_t the_size_to_use = curr_client.recv_size-(strlen(split_one[0])+strlen(REQUEST_SECTION_DELIMITER_STRING));
			
			curr_client.req->form_size = (is_a_form?the_size_to_use:0);
			curr_client.req->begin_data_size = (is_a_form?0:the_size_to_use);
			curr_client.enable_first_time_in_upload=
					(
					
						(the_size_to_use>0)
						
							&&
						
						request_const_browser_name_arr[curr_client.req->browser_mode].first_time_if_http_or_https[(curr_client.is_tls>0)]
					
					);

			memcpy(is_a_form?curr_client.req->form:curr_client.req->begin_data,
						split_one[1],
						is_a_form?(curr_client.req->form_size):(curr_client.req->begin_data_size));
			
			curr_client.req->data_size=is_a_form?0:strtoll(curr_client.req->header->content_length,NULL,10);
	}

}


void print_http_req(FILE* fstream){
	fprintf(fstream,"HTTP_REQUEST:\n"
					"\n\n"
					"______________________________________\n"
					"______________________________________\n\n\n");
					
	print_http_req_header(fstream);
	fprintf(fstream,"HTTP_REQUEST_CONTENTS=\n"
					"Data size:\n%ld\n"
					"______________________________________\n"
					"Form size:\n%ld\n"
					"Form:\n\n%s\n"
					"______________________________________\n"
					"Client received size:\n%ld\n"
					"Begin-Data-Size:\n%ld\n"
					"Begin-Data:\n%s\n"
					"______________________________________\n"
					"______________________________________\n"
					"Browser-Mode: %s\n"
					"______________________________________\n"
					"______________________________________\n\n\n",
					curr_client.req->is_a_form?0:curr_client.req->data_size,
					curr_client.req->is_a_form?curr_client.req->form_size:0,
					curr_client.req->is_a_form?curr_client.req->form:"There is no form...",
					curr_client.recv_size,
					curr_client.req->is_a_form?0:curr_client.req->begin_data_size,
					curr_client.req->is_a_form?"There is no data...":curr_client.req->begin_data,
					request_const_browser_name_arr[curr_client.req->browser_mode].browser_name);
	
	if(logging> 7){
		
		fprintf(logstream, "Browser-Mode view:\n"
						"_____________________________________\n"
						"Append pending data if http?:%hhu\n"
						"Append pending data if http(s)?:%hhu\n"
						"Are we tls?%hhu\n"
						"So. Should we enable first time if this is an upload??:%s\n"
						"_____________________________________\n"
						"______________________________________\n\n\n",
				request_const_browser_name_arr[curr_client.req->browser_mode].first_time_if_http_or_https[0],
				request_const_browser_name_arr[curr_client.req->browser_mode].first_time_if_http_or_https[1],
				(curr_client.is_tls>0),
				curr_client.enable_first_time_in_upload?"Yes!":"No...");
	}			

}
