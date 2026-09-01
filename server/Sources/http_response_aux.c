#include "../Includes/preprocessor.h"
#include "../Includes/http_response_aux.h"
#include "../Includes/server_vars.h"
#include "../Includes/client.h"

#define EMBEDDER_POLICY "require-corp"

#define STD_HEADER_START_OK HTTP_OK_HEADER \
						"Accept-Ranges: bytes\r\n" \
						"Content-Type: %s\r\n"

#define STD_HEADER_START_PARTIAL HTTP_PARTIAL_CONTENT_OK_HEADER \
								"Accept-Ranges: bytes\r\n" \
								"Content-Type: %s\r\n"


#define UPLOAD_HEADER HTTP_CREATED_OK_HEADER \
					"Content-Type: %s\r\n" \
					"Location: " \
					"%s" \
					"://" \
					DOMAIN_NAME \
					":%hu%s" \
					"%s\r\n" \
					"\r\n"
					
/*
"Cross-Origin-Embedder-Policy: "
			  //EMBEDDER_POLICY
			   "\r\n"
*/


/*
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-------------------------------NORMALS---------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
*/

char* normalHeader= STD_HEADER_START_OK
			"Content-Length: %ld\r\n"
			  //"Origin: localhost\r\n"
			  /*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          /*"Cross-Origin-Embedder-Policy: "*/
			  //EMBEDDER_POLICY  "\r\n"
 			  "\r\n";

void fillUpNormalHeader(char headerBuff[PATHSIZE],const char* mimetype,int64_t size){

	snprintf(headerBuff,PATHSIZE,normalHeader,mimetype,size);

}

char* normalHeaderComp= STD_HEADER_START_OK
				"Content-Length: %ld\r\n"
			  //"Origin: localhost\r\n"
			  "Content-encoding: %s\r\n"
			  /*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          /*"Cross-Origin-Embedder-Policy: "*/
			  //EMBEDDER_POLICY "\r\n"
			   "\r\n";
void fillUpNormalHeaderComp(char headerBuff[PATHSIZE],const char* mimetype,char* method,int64_t size){

	snprintf(headerBuff,PATHSIZE,normalHeaderComp,mimetype,size,method);

}

char* chunkedHeader= STD_HEADER_START_OK
			  //"Origin: localhost\r\n"
			  /*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          /*"Cross-Origin-Embedder-Policy: require-corp\r\n"*/
			  /*"Cross-Origin-Embedder-Policy: "*/
			  //EMBEDDER_POLICY  "\r\n"
			  "Transfer-Encoding: chunked\r\n"
                          "\r\n";

void fillUpChunkedHeader(char headerBuff[PATHSIZE],const char* mimetype){
	snprintf(headerBuff,PATHSIZE,chunkedHeader,mimetype);

}

char* chunkedHeaderComp= STD_HEADER_START_OK
			  //"Origin: localhost\r\n"
			  /*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          /*"Cross-Origin-Embedder-Policy: "*/
			  //EMBEDDER_POLICY "\r\n"
                          "Transfer-Encoding: %s, chunked\r\n"
                          "\r\n";

void fillUpChunkedHeaderComp(char headerBuff[PATHSIZE],const char* mimetype,char* method){
	snprintf(headerBuff,PATHSIZE,chunkedHeaderComp,mimetype,method);

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




char* normalPartialHeader= STD_HEADER_START_PARTIAL
						"Content-Length: %ld\r\n"
						"Content-Range: bytes %ld-%ld/%ld\r\n"
			  //"Origin: localhost\r\n"
			  /*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          /*"Cross-Origin-Embedder-Policy: "*/
			  //EMBEDDER_POLICY  "\r\n"
 			  "\r\n";
void fillUpNormalPartialHeader(char headerBuff[PATHSIZE],const char* mimetype, int64_t start_point,int64_t size){
	snprintf(headerBuff,PATHSIZE,normalPartialHeader,mimetype,size-start_point,start_point,size-1,size);
}


char* normalPartialHeaderComp= STD_HEADER_START_PARTIAL
						"Content-Length: %ld\r\n"
						"Content-Range: bytes %ld-%ld/%ld\r\n"
			  //"Origin: localhost\r\n"
			  "Content-encoding: %s\r\n"
			  /*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          /*"Cross-Origin-Embedder-Policy: "*/
			  //EMBEDDER_POLICY "\r\n"
			   "\r\n";
			   
void fillUpNormalPartialHeaderComp(char headerBuff[PATHSIZE],const char* mimetype,char* method, int64_t start_point,int64_t size){
	snprintf(headerBuff,PATHSIZE,normalPartialHeaderComp,mimetype,size-start_point,start_point,start_point+size-1,size,method);
}
		   
char* chunkedPartialHeader= STD_HEADER_START_PARTIAL
						"Content-Range: bytes %ld-%ld/%ld\r\n"
			  //"Origin: localhost\r\n"
			  /*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          /*"Cross-Origin-Embedder-Policy: require-corp\r\n"*/
			  /*"Cross-Origin-Embedder-Policy: "*/
			  //EMBEDDER_POLICY  "\r\n"
			  "Transfer-Encoding: chunked\r\n"
                          "\r\n";
void fillUpChunkedPartialHeader(char headerBuff[PATHSIZE],const char* mimetype,int64_t start_point,int64_t size){
	snprintf(headerBuff,PATHSIZE,chunkedPartialHeader,mimetype,start_point,start_point+size-1,size);
}

char* chunkedPartialHeaderComp= STD_HEADER_START_PARTIAL
						"Content-Range: bytes %ld-%ld/%ld\r\n"
			  //"Origin: localhost\r\n"
			  /*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          /*"Cross-Origin-Embedder-Policy: "*/
			  //EMBEDDER_POLICY "\r\n"
                          "Transfer-Encoding: %s, chunked\r\n"
                          "\r\n";

void fillUpChunkedPartialHeaderComp(char headerBuff[PATHSIZE],const char* mimetype,char* method,int64_t start_point,int64_t size){
	snprintf(headerBuff,PATHSIZE,chunkedPartialHeaderComp,mimetype,start_point,start_point+size-1,size,method);
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


void fillUploadHeader(char headerBuff[PATHSIZE],const char* mimetype, char* target){

	snprintf(headerBuff,
				PATHSIZE*2,
				UPLOAD_HEADER,
				mimetype,
				curr_client.is_tls?"https":"http",
				port,
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




char* redirectHeader= "HTTP/1.1 301 See Other\r\n"
				"Accept-Ranges: bytes\r\n"
				"Location: %s\r\n"
			  	//"Origin: localhost\r\n"
			  	"Content-Length: 0\r\n"
			  	/*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          	/*"Cross-Origin-Embedder-Policy: "*/
			  	//EMBEDDER_POLICY "\r\n"
			   	"\r\n";

void fillUpRedirectHeader(char headerBuff[PATHSIZE],char* path){

	snprintf(headerBuff,PATHSIZE,redirectHeader,path);

}

char* notFoundHeader= "HTTP/1.1 404 Not Found\r\n"
						"Accept-Ranges: bytes\r\n"
                                "Content-Type: %s\r\n"
			  	//"Origin: localhost\r\n"
			  	"Content-Length: %ld\r\n"
			  	/*"Cross-Origin-Embedder-Policy: "*/
			  	//EMBEDDER_POLICY "\r\n"
			   	/*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          	"\r\n";

void fillUpFileNotFoundHeader(char headerBuff[PATHSIZE],int64_t size,const char* mimetype){

	snprintf(headerBuff,PATHSIZE,notFoundHeader,mimetype,size);

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
