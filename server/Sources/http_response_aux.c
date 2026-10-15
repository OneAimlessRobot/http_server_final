#include "../Includes/preprocessor.h"
#include "../Includes/http_response_aux.h"
#include "../Includes/server_vars.h"
#include "../Includes/client.h"
#include "../Includes/load_date.h"


#define FILL_UP_BORING_CRAP  LE_TIMEUREUR_DATE, \
						KEEP_ALIVE_STRING, \
						LE_TIMEUREUR_LAST_MODIFIED_DATE, \
						ETag, \
						curr_client.should_cache?"no-cache":"no-store", \
						s_maxage

#define FILL_UP_BORING_CRAP_NOT_MODIFIED LE_TIMEUREUR_DATE, \
						KEEP_ALIVE_STRING, \
						LE_TIMEUREUR_LAST_MODIFIED_DATE, \
						ETag, \
						curr_client.should_cache?"no-cache":"no-store", \
						s_maxage
/*
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-------------------------------NORMALS---------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
*/


uint64_t fillUpNormalHeader(const char* mimetype,int64_t size){

	return snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						NORMAL_HEADER,
						FILL_UP_BORING_CRAP,
						mimetype,
						size);

}

uint64_t fillUpNormalHeaderComp(const char* mimetype,char* method,int64_t size){

	return snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						NORMAL_HEADER_COMP,
						FILL_UP_BORING_CRAP,
						mimetype,
						size,
						method);

}


uint64_t fillUpChunkedHeader(const char* mimetype){
	return snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						CHUNKED_HEADER,
						FILL_UP_BORING_CRAP,
						mimetype);

}

uint64_t fillUpChunkedHeaderComp(const char* mimetype,char* method){
	return snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						CHUNKED_HEADER_COMP,
						FILL_UP_BORING_CRAP,
						mimetype,
						method);

}


/*
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-------------------------------PARTIALS--------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
*/


uint64_t fillUpNormalPartialHeader(const char* mimetype, int64_t start_point,
										int64_t end_point,
										int64_t size,
										int64_t file_size){
											
	return snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						NORMAL_PARTIAL_HEADER,
						FILL_UP_BORING_CRAP,
						mimetype,
						size,
						start_point,
						end_point,
						file_size);
}
			   
uint64_t fillUpNormalPartialHeaderComp(const char* mimetype,char* method, int64_t start_point,
										int64_t end_point,
										int64_t size,
										int64_t file_size){
	return snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						NORMAL_PARTIAL_HEADER_COMP,
						FILL_UP_BORING_CRAP,
						mimetype,
						size,
						start_point,
						end_point,
						file_size,
						method);
}
		   

uint64_t fillUpChunkedPartialHeader(const char* mimetype,
										int64_t start_point,
										int64_t end_point,
										int64_t file_size){

	return snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						CHUNKED_PARTIAL_HEADER,
						FILL_UP_BORING_CRAP,
						mimetype,
						start_point,
						end_point,
						file_size);
}

uint64_t fillUpChunkedPartialHeaderComp(const char* mimetype,char* method,
										int64_t start_point,
										int64_t end_point,
										int64_t file_size){
	
	
	return snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						CHUNKED_PARTIAL_HEADER_COMP,
						FILL_UP_BORING_CRAP,
						mimetype,
						start_point,
						end_point,
						file_size,
						method);
}


/*
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-------------------------------UPLOADS---------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
*/


uint64_t fillUploadHeader(const char* mimetype, char* target_location_dir, char* target_filename){
	
	char* defaultTarget_ptr= &defaultTarget[0];
	return snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
				UPLOAD_HEADER,
				LE_TIMEUREUR_DATE,
				KEEP_ALIVE_STRING,
				mimetype,
				curr_client.is_tls?"https":"http",
				port,
				defaultTarget_ptr,
				target_location_dir,
				target_filename);
}

uint64_t fillNoContentHeader(const char* mimetype, char* target_location_dir, char* target_filename){
	
	char* defaultTarget_ptr= &defaultTarget[0];
	return snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
				NO_CONTENT_HEADER,
				LE_TIMEUREUR_DATE,
				KEEP_ALIVE_STRING,
				mimetype,
				ETag,
				curr_client.is_tls?"https":"http",
				port,
				defaultTarget_ptr,
				target_location_dir,
				target_filename);
}

/*
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-------------------------------MISC------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
*/



uint64_t fillUpRedirectHeader(char* path, u_int8_t upgrade_obligatory){

	return snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
							REDIRECT_HEADER,
							LE_TIMEUREUR_DATE,
							KEEP_ALIVE_STRING,
							upgrade_obligatory?"https":"http",
							port,
							path);

}

uint64_t fillUpFileNotFoundHeader(int64_t size,const char* mimetype){

	return snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
							HTTP_NOT_FOUND_HEADER,
							LE_TIMEUREUR_DATE,
							KEEP_ALIVE_STRING,
							mimetype,
							size);

}

uint64_t fillUpInternalErrorHeader(void){

	return snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
							HTTP_INTERNAL_ERROR_HEADER,
							LE_TIMEUREUR_DATE,
							KEEP_ALIVE_STRING);

}
uint64_t fillUpNotModifiedHeader(void){
	
	return snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
							HTTP_NOT_MODIFIED_HEADER,
							FILL_UP_BORING_CRAP_NOT_MODIFIED);
	
}

/*
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-------------------------------AUX_STUFF-------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
*/

void print_response_header(char* headerBuff){
	
	if(logging>5){
		fprintf(logstream,"This is the header:\n\n%s",headerBuff);
	}
}
