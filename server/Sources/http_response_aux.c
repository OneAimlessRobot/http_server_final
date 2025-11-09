#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "../Includes/buffSizes.h"
#include "../Includes/http_response_aux.h"

#define EMBEDDER_POLICY "credentialless"
/*
"Cross-Origin-Embedder-Policy: "
			  EMBEDDER_POLICY
			   "\r\n"
*/
char* chunkedHeaderComp= "HTTP/1.1 200 OK\r\n"
                          "Content-Type: %s\r\n"
			  "Origin: https://localho.st:11001\r\n"
			  /*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          "Cross-Origin-Embedder-Policy: "
			  EMBEDDER_POLICY
			   "\r\n"
                          "content-encoding: %s\r\n"
                          "Transfer-Encoding: chunked\r\n"
                          "\r\n";
char* chunkedHeader= "HTTP/1.1 200 OK\r\n"
                          "Content-Type: %s\r\n"
			  "Origin: https://localho.st:11001\r\n"
			  /*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          "Cross-Origin-Embedder-Policy: require-corp\r\n"
			  "Cross-Origin-Embedder-Policy: "
			  EMBEDDER_POLICY
			  "\r\n"
			  "Transfer-Encoding: chunked\r\n"
                          "\r\n";
char* normalHeader= "HTTP/1.1 200 OK\r\n"
                          "Content-Type: %s\r\n"
			  "Origin: https://localho.st:11001\r\n"
			  /*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          "Cross-Origin-Embedder-Policy: "
			  EMBEDDER_POLICY
			   "\r\n"
			   "Content-Length: %lu\r\n"
 			  "\r\n";
char* normalHeaderComp= "HTTP/1.1 200 OK\r\n"
                          "Content-Type: %s\r\n"
			  "Origin: https://localho.st:11001\r\n"
			  "Content-encoding: %s\r\n"
			  "Content-Length: %lu\r\n"
			  /*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          "Cross-Origin-Embedder-Policy: "
			  EMBEDDER_POLICY
			   "\r\n"
			   "\r\n";

char* redirectHeader= "HTTP/1.1 301 See Other\r\n"
				"Location: %s\r\n"
			  	"Origin: https://localho.st:11001\r\n"
			  	"Content-Length: 0\r\n"
			  	/*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          	"Cross-Origin-Embedder-Policy: "
			  	EMBEDDER_POLICY
			   	"\r\n"
			   	"\r\n";

char* notFoundHeader= "HTTP/1.1 404 Not Found\r\n"
                                "Content-Type: %s\r\n"
			  	"Origin: https://localho.st:11001\r\n"
			  	"Content-Length: %lu\r\n"
			  	"Cross-Origin-Embedder-Policy: "
			  	EMBEDDER_POLICY
			   	"\r\n"
			   	/*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          	"\r\n";
char* notFoundHeaderComp= "HTTP/1.1 404 Not Found\r\n"
                                "Content-Type: %s\r\n"
			  	"Origin: https://localho.st:11001\r\n"
			  	"Content-Length: %lu\r\n"
			        "content-encoding: %s\r\n"
                          	"Cross-Origin-Embedder-Policy: "
			  	EMBEDDER_POLICY
			   	"\r\n"
				/*"Cross-Origin-Opener-Policy: same-origin\r\n"*/
                          	"\r\n";

void fillUpRedirectHeader(char headerBuff[PATHSIZE],char* path){

	snprintf(headerBuff,PATHSIZE,redirectHeader,path);

}

void fillUpChunkedHeader(char headerBuff[PATHSIZE],char* headerTemplate,const char* mimetype){
	snprintf(headerBuff,PATHSIZE,headerTemplate,mimetype);

}
void fillUpChunkedHeaderComp(char headerBuff[PATHSIZE],char* headerTemplate,const char* mimetype,char* method){
	snprintf(headerBuff,PATHSIZE,headerTemplate,mimetype,method);

}
void fillUpNormalHeader(char headerBuff[PATHSIZE],char* headerTemplate,u_int64_t size,const char* mimetype){

	snprintf(headerBuff,PATHSIZE,headerTemplate,mimetype,size);

}
void fillUpNormalHeaderComp(char headerBuff[PATHSIZE],char* headerTemplate,u_int64_t size,const char* mimetype,char* method){

	snprintf(headerBuff,PATHSIZE,headerTemplate,mimetype,method,size);

}
void fillUpFileNotFoundHeader(char headerBuff[PATHSIZE],u_int64_t size,const char* mimetype){

	snprintf(headerBuff,PATHSIZE,notFoundHeader,mimetype,size);

}
void fillUpFileNotFoundHeaderComp(char headerBuff[PATHSIZE],u_int64_t size,const char* mimetype,char* method){

	snprintf(headerBuff,PATHSIZE,notFoundHeaderComp,mimetype,size,method);


}
