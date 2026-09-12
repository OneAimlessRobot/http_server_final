#include "../Includes/preprocessor.h"
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


static http_req_type getREQtypeFromString(char* str){
	if(!str){
		return INVALID;
	}
	if(!strnlen(str,FIELDSIZE)){
		return INVALID;
	}
	for(http_req_type i=GET;i<MAX_REQUEST_TYPE ;i++){
		
		if(!strncmp(str,request_const_name_arr[i],FIELDSIZE)){
				
				return i;
		}
		
	}
	return INVALID;
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
					
					"Referer: %s\n"
					"Accept: %s\n"
					"Accept-Language: %s\n"
					"Sec-Fetch-Site: %s\n"
					"Sec-Fetch-Mode: %s\n"
					"Sec-Fetch-Dest: %s\n"
					"Sec-Fetch-User: %s\n"
					
					"Sec-CH-Ua: %s\n"
					"Sec-CH-Ua-Platform: %s\n"
					
					"Cookie: %s\n"
					
					"if-none-match: |%s|\n"
					
					"mimetype: %s\n"
					"Mobile? (raw): %s\n"
					"Mobile? (captured value): %hhu\n"
					"Range (string): |%s|\n"
					"Range: bytes=%s\n"
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
							curr_client.req->header->refer,
							curr_client.req->header->accept,
							curr_client.req->header->accept_language,
							curr_client.req->header->sec_fetch_site,
							curr_client.req->header->sec_fetch_mode,
							curr_client.req->header->sec_fetch_dest,
							curr_client.req->header->sec_fetch_user,
							
							curr_client.req->header->sec_ch_ua,
							curr_client.req->header->sec_ch_ua_platform,
							
							curr_client.req->header->cookie,
							
							curr_client.req->header->if_none_match,
							
							curr_client.req->header->mimetype,
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
		
		if(!strcicmp(fields[i].field[0],fieldName)){
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
	b_zero(curr_client.req->header->target);
	b_zero(curr_client.req->header->content_length);
	b_zero(curr_client.req->header->origin);
	b_zero(curr_client.req->header->host);
	b_zero(curr_client.req->header->mimetype);
	b_zero(curr_client.req->header->specific);
	
	b_zero(curr_client.req->header->is_mobile);
	b_zero(curr_client.req->header->version);
	b_zero(curr_client.req->header->user_agent);
	b_zero(curr_client.req->header->range);
	b_zero(curr_client.req->header->range_string);
	b_zero(curr_client.req->header->cloudfare_ip);
	
	b_zero(curr_client.req->header->refer);
	b_zero(curr_client.req->header->accept);
	b_zero(curr_client.req->header->accept_language);
	b_zero(curr_client.req->header->sec_fetch_site);
	b_zero(curr_client.req->header->sec_fetch_mode);
	b_zero(curr_client.req->header->sec_fetch_dest);
	b_zero(curr_client.req->header->sec_fetch_user);
	b_zero(curr_client.req->header->sec_ch_ua);
	b_zero(curr_client.req->header->sec_ch_ua_platform);
	b_zero(curr_client.req->header->cookie);
	b_zero(curr_client.req->header->if_none_match);
	
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
		strcpy(curr_client.req->header->is_mobile,find_field_value_in_field_arr("sec-ch-ua-mobile",fieldarr));
		if(strnlen(curr_client.req->header->is_mobile,FIELDSIZE)>0){
			
			sscanf(curr_client.req->header->is_mobile,"?%hhu",&curr_client.req->header->is_mobile_value);
			
		}

		strcpy(curr_client.req->header->encoding,find_field_value_in_field_arr("Accept-Encoding",fieldarr));
		strcpy(curr_client.req->header->content_length,find_field_value_in_field_arr("Content-Length",fieldarr));
		strcpy(curr_client.req->header->user_agent,find_field_value_in_field_arr("User-Agent",fieldarr));
		strcpy(curr_client.req->header->cloudfare_ip,find_field_value_in_field_arr("CF-Connecting-IP",fieldarr));
		
		strcpy(curr_client.req->header->refer,find_field_value_in_field_arr("Referer",fieldarr));
		strcpy(curr_client.req->header->accept,find_field_value_in_field_arr("Accept",fieldarr));
		strcpy(curr_client.req->header->accept_language,find_field_value_in_field_arr("Accept-Language",fieldarr));
		strcpy(curr_client.req->header->sec_fetch_site,find_field_value_in_field_arr("Sec-Fetch-Site",fieldarr));
		strcpy(curr_client.req->header->sec_fetch_mode,find_field_value_in_field_arr("Sec-Fetch-Mode",fieldarr));
		strcpy(curr_client.req->header->sec_fetch_dest,find_field_value_in_field_arr("Sec-Fetch-Dest",fieldarr));
		strcpy(curr_client.req->header->sec_fetch_user,find_field_value_in_field_arr("Sec-Fetch-User",fieldarr));
		strcpy(curr_client.req->header->sec_ch_ua,find_field_value_in_field_arr("Sec-CH-Ua",fieldarr));
		strcpy(curr_client.req->header->sec_ch_ua_platform,find_field_value_in_field_arr("Sec-CH-Ua-Platform",fieldarr));
		strcpy(curr_client.req->header->cookie,find_field_value_in_field_arr("Cookie",fieldarr));
		strcpy(curr_client.req->header->if_none_match,find_field_value_in_field_arr("if-none-match",fieldarr));
		
		u_int64_t the_incomming_etag_len = 
				strnlen(curr_client.req->header->if_none_match,sizeof(curr_client.req->header->if_none_match));
		
		if(the_incomming_etag_len>=3){
			memmove(curr_client.req->header->if_none_match,&curr_client.req->header->if_none_match[2],
						the_incomming_etag_len-3);
			curr_client.req->header->if_none_match[the_incomming_etag_len-3]=0;
		}
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
	curr_client.req->header->range_start = 0;
	curr_client.req->header->range_end = 0;
	if((curr_client.req->header->is_partial=(!ignore_partial))){
		strcpy(curr_client.req->header->range_string,find_field_value_in_field_arr("Range",fieldarr));
		curr_client.req->header->range_start= 0;
		if((curr_client.req->header->is_partial=(strlen(curr_client.req->header->range_string)>0))){
			sscanf(curr_client.req->header->range_string," bytes=%ld-%ld",
						&curr_client.req->header->range_start,
						&curr_client.req->header->range_end);
			
		}
	}
	snprintf(curr_client.req->header->range,FIELDSIZE-1,"%ld-%ld",curr_client.req->header->is_partial?curr_client.req->header->range_start:-1,curr_client.req->header->is_partial?(curr_client.req->header->range_end>0?curr_client.req->header->range_end:-1):-1);
	free(fieldarr);
	
}

void spawnHTTPRequest(void){
	char* split_one[2];
	
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
			curr_client.enable_first_time_in_upload = (the_size_to_use>0);
						

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
					"______________________________________\n\n\n",
					curr_client.req->is_a_form?0:curr_client.req->data_size,
					curr_client.req->is_a_form?curr_client.req->form_size:0,
					curr_client.req->is_a_form?curr_client.req->form:"There is no form...",
					curr_client.recv_size,
					curr_client.req->is_a_form?0:curr_client.req->begin_data_size,
					curr_client.req->is_a_form?"There is no data...":curr_client.req->begin_data);
	
	if(logging> 7){
		
		fprintf(logstream, "Browser-Mode view:\n"
						"_____________________________________\n"
						"So. Should we enable first time if this is an upload??:%s\n"
						"_____________________________________\n"
						"______________________________________\n\n\n",
						curr_client.enable_first_time_in_upload?"Yes!":"No...");
	}			

}
