#include "../Includes/preprocessor.h"
#include "../Includes/http_response_aux.h"
#include "../Includes/server_vars.h"
#include "../Includes/client.h"
#include "../Includes/load_date.h"



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
						LE_TIMEUREUR_DATE,
						KEEP_ALIVE_STRING,
						LE_TIMEUREUR_LAST_MODIFIED_DATE,
						LE_TIMEUREUR_LAST_ACCESS_DATE,
						ETag,
						DIFF_TIME(struct_for_date.timeur, struct_for_access_date.timeur),
						curr_client.should_cache?"must-revalidate":"no-cache",
						mimetype,
						size);

}

void fillUpNormalHeaderComp(const char* mimetype,char* method,int64_t size){

	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						NORMAL_HEADER_COMP,
						LE_TIMEUREUR_DATE,
						KEEP_ALIVE_STRING,
						LE_TIMEUREUR_LAST_MODIFIED_DATE,
						LE_TIMEUREUR_LAST_ACCESS_DATE,
						ETag,
						DIFF_TIME(struct_for_date.timeur, struct_for_access_date.timeur),
						curr_client.should_cache?"must-revalidate":"no-cache",
						mimetype,
						size,
						method);

}


void fillUpChunkedHeader(const char* mimetype){
	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						CHUNKED_HEADER,
						LE_TIMEUREUR_DATE,
						KEEP_ALIVE_STRING,
						LE_TIMEUREUR_LAST_MODIFIED_DATE,
						LE_TIMEUREUR_LAST_ACCESS_DATE,
						ETag,
						DIFF_TIME(struct_for_date.timeur, struct_for_access_date.timeur),
						curr_client.should_cache?"must-revalidate":"no-cache",
						mimetype);

}

void fillUpChunkedHeaderComp(const char* mimetype,char* method){
	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						CHUNKED_HEADER_COMP,
						LE_TIMEUREUR_DATE,
						KEEP_ALIVE_STRING,
						LE_TIMEUREUR_LAST_MODIFIED_DATE,
						LE_TIMEUREUR_LAST_ACCESS_DATE,
						ETag,
						DIFF_TIME(struct_for_date.timeur, struct_for_access_date.timeur),
						curr_client.should_cache?"must-revalidate":"no-cache",
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


void fillUpNormalPartialHeader(const char* mimetype, int64_t start_point,int64_t size){
	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						NORMAL_PARTIAL_HEADER,
						LE_TIMEUREUR_DATE,
						KEEP_ALIVE_STRING,
						LE_TIMEUREUR_LAST_MODIFIED_DATE,
						LE_TIMEUREUR_LAST_ACCESS_DATE,
						ETag,
						DIFF_TIME(struct_for_date.timeur, struct_for_access_date.timeur),
						curr_client.should_cache?"must-revalidate":"no-cache",
						mimetype,
						size-start_point,
						start_point,
						size-1,
						size);
}
			   
void fillUpNormalPartialHeaderComp(const char* mimetype,char* method, int64_t start_point,int64_t size){
	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						NORMAL_PARTIAL_HEADER_COMP,
						LE_TIMEUREUR_DATE,
						KEEP_ALIVE_STRING,
						LE_TIMEUREUR_LAST_MODIFIED_DATE,
						LE_TIMEUREUR_LAST_ACCESS_DATE,
						ETag,
						DIFF_TIME(struct_for_date.timeur, struct_for_access_date.timeur),
						curr_client.should_cache?"must-revalidate":"no-cache",
						mimetype,
						size-start_point,
						start_point,
						start_point+size-1,
						size,method);
}
		   

void fillUpChunkedPartialHeader(const char* mimetype,int64_t start_point,int64_t size){
	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						CHUNKED_PARTIAL_HEADER,
						LE_TIMEUREUR_DATE,
						KEEP_ALIVE_STRING,
						LE_TIMEUREUR_LAST_MODIFIED_DATE,
						LE_TIMEUREUR_LAST_ACCESS_DATE,
						ETag,
						DIFF_TIME(struct_for_date.timeur, struct_for_access_date.timeur),
						curr_client.should_cache?"must-revalidate":"no-cache",
						mimetype,
						start_point,
						start_point+size-1,
						size);
}

void fillUpChunkedPartialHeaderComp(const char* mimetype,char* method,int64_t start_point,int64_t size){
	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
						CHUNKED_PARTIAL_HEADER_COMP,
						LE_TIMEUREUR_DATE,
						KEEP_ALIVE_STRING,
						LE_TIMEUREUR_LAST_MODIFIED_DATE,
						LE_TIMEUREUR_LAST_ACCESS_DATE,
						ETag,
						DIFF_TIME(struct_for_date.timeur, struct_for_access_date.timeur),
						curr_client.should_cache?"must-revalidate":"no-cache",
						mimetype,
						start_point,
						start_point+size-1,
						size,
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

	snprintf(curr_client.sent_header_buff,sizeof(curr_client.sent_header_buff)-1,
				UPLOAD_HEADER,
				LE_TIMEUREUR_DATE,
				KEEP_ALIVE_STRING,
				mimetype,
				curr_client.is_tls?"https":"http",
				port,
				defaultTarget,
				uploadTarget,
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
							NOT_FOUND_HEADER,
							LE_TIMEUREUR_DATE,
							KEEP_ALIVE_STRING,
							mimetype,
							size);

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
