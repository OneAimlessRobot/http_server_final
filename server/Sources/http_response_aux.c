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


void fillUpNormalHeader(const char* mimetype,int64_t size){

	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						NORMAL_HEADER,
						FILL_UP_BORING_CRAP,
						mimetype,
						size);

}

void fillUpNormalHeaderComp(const char* mimetype,char* method,int64_t size){

	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						NORMAL_HEADER_COMP,
						FILL_UP_BORING_CRAP,
						mimetype,
						size,
						method);

}


void fillUpChunkedHeader(const char* mimetype){
	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						CHUNKED_HEADER,
						FILL_UP_BORING_CRAP,
						mimetype);

}

void fillUpChunkedHeaderComp(const char* mimetype,char* method){
	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
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


void fillUpNormalPartialHeader(const char* mimetype, int64_t start_point,
										int64_t end_point,
										int64_t size,
										int64_t file_size){
											
	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						NORMAL_PARTIAL_HEADER,
						FILL_UP_BORING_CRAP,
						mimetype,
						size,
						start_point,
						end_point,
						file_size);
}
			   
void fillUpNormalPartialHeaderComp(const char* mimetype,char* method, int64_t start_point,
										int64_t end_point,
										int64_t size,
										int64_t file_size){
	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						NORMAL_PARTIAL_HEADER_COMP,
						FILL_UP_BORING_CRAP,
						mimetype,
						size,
						start_point,
						end_point,
						file_size,
						method);
}
		   

void fillUpChunkedPartialHeader(const char* mimetype,
										int64_t start_point,
										int64_t end_point,
										int64_t file_size){

	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						CHUNKED_PARTIAL_HEADER,
						FILL_UP_BORING_CRAP,
						mimetype,
						start_point,
						end_point,
						file_size);
}

void fillUpChunkedPartialHeaderComp(const char* mimetype,char* method,
										int64_t start_point,
										int64_t end_point,
										int64_t file_size){
	
	
	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
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


void fillUploadHeader(const char* mimetype, char* target){
	
	
	char* defaultTarget_ptr=buff_2_ptr(defaultTarget);
	char* uploadTarget_ptr=buff_2_ptr(uploadTarget);
	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
				UPLOAD_HEADER,
				LE_TIMEUREUR_DATE,
				KEEP_ALIVE_STRING,
				mimetype,
				curr_client.is_tls?"https":"http",
				port,
				defaultTarget_ptr,
				uploadTarget_ptr,
				target);
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



void fillUpRedirectHeader(char* path){

	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
							REDIRECT_HEADER,
							LE_TIMEUREUR_DATE,
							KEEP_ALIVE_STRING,
							path);

}

void fillUpFileNotFoundHeader(int64_t size,const char* mimetype){

	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
							HTTP_NOT_FOUND_HEADER,
							LE_TIMEUREUR_DATE,
							KEEP_ALIVE_STRING,
							mimetype,
							size);

}

void fillUpNotModifiedHeader(void){
	
	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
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
