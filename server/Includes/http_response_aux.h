#ifndef HTTP_RESPONSE_AUX_H
#define HTTP_RESPONSE_AUX_H

#define HTTP_OK_HEADER "HTTP/1.1 200 OK\r\n"
#define HTTP_PARTIAL_CONTENT_OK_HEADER "HTTP/1.1 206 Partial-Content\r\n"

extern char
	* normalHeader,*normalHeaderComp,
	
	
	* chunkedHeader,*chunkedHeaderComp,
	
	
	* normalPartialHeader,*normalPartialHeaderComp,
	
	
	*chunkedPartialHeader,*chunkedPartialHeaderComp,

* redirectHeader,*notFoundHeader;


void fillUpNormalHeader(char headerBuff[PATHSIZE],const char* mimetype,u_int64_t size);
void fillUpNormalHeaderComp(char headerBuff[PATHSIZE],const char* mimetype,char* method,u_int64_t size);


void fillUpChunkedHeader(char headerBuff[PATHSIZE],const char* mimetype);
void fillUpChunkedHeaderComp(char headerBuff[PATHSIZE],const char* mimetype,char* method);


/*
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-------------------------------PARTIALS--------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
*/

void fillUpNormalPartialHeader(char headerBuff[PATHSIZE],const char* mimetype,u_int64_t start_point,u_int64_t size);
void fillUpNormalPartialHeaderComp(char headerBuff[PATHSIZE],const char* mimetype,char* method,u_int64_t start_point,u_int64_t size);


void fillUpChunkedPartialHeader(char headerBuff[PATHSIZE],const char* mimetype,u_int64_t start_point,u_int64_t size);
void fillUpChunkedPartialHeaderComp(char headerBuff[PATHSIZE],const char* mimetype,char* method,u_int64_t start_point,u_int64_t size);


/*
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-------------------------------MISC------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
*/


void fillUpRedirectHeader(char headerBuff[PATHSIZE],char* path);

void fillUpFileNotFoundHeader(char headerBuff[PATHSIZE],u_int64_t size,const char* mimetype);
void fillUpFileNotFoundHeaderComp(char headerBuff[PATHSIZE],u_int64_t size,const char* mimetype,char* method);


/*
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-------------------------------AUX_STUFF-------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
*/

void print_response_header(char* headerBuff);

#endif
