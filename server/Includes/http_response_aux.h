#ifndef HTTP_RESPONSE_AUX_H
#define HTTP_RESPONSE_AUX_H


#define EMBEDDER_POLICY "require-corp"


#define HTTP_SERVER_INTRO_HEADERS "Date: %s\r\n" \
									"Server: " \
									SERVER_NAME \
									"\r\n" \
									"Connection: %s\r\n"


#define HTTP_SERVER_FILE_AGE_HEADERS HTTP_SERVER_INTRO_HEADERS \
									"Last-Modified: %s\r\n" \
									"Last-Accessed: %s\r\n" \
									"ETag: \"%s\"\r\n" \
									"Cache-Control: %s, s-maxage=%lu\r\n"



#define HTTP_SERVER_FILE_AGE_HEADERS_NOT_MODIFIED HTTP_SERVER_INTRO_HEADERS \
									"Last-Modified: %s\r\n" \
									"ETag: \"%s\"\r\n" \
									"Cache-Control: %s, s-maxage=%lu\r\n"
/*
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-------------------------------NORMALS---------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
*/

#define HTTP_OK_HEADER  HTTP_VERSION_STRING OK_RESPONSE \
											HTTP_SERVER_FILE_AGE_HEADERS
											
#define STD_HEADER_START_OK HTTP_OK_HEADER \
						"Accept-Ranges: bytes\r\n" \
						"Content-Type: %s\r\n"
						
						
#define NORMAL_HEADER STD_HEADER_START_OK \
					"Content-Length: %ld\r\n" \
					  "\r\n"
 			  
#define NORMAL_HEADER_COMP STD_HEADER_START_OK \
						"Content-Length: %ld\r\n" \
					  "Content-encoding: %s\r\n" \
					   "\r\n"
			   
			   
#define CHUNKED_HEADER STD_HEADER_START_OK \
				  "Transfer-Encoding: chunked\r\n" \
					"\r\n"

#define CHUNKED_HEADER_COMP STD_HEADER_START_OK \
                          "Transfer-Encoding: %s, chunked\r\n" \
                          "\r\n"
                          
void fillUpNormalHeader(const char* mimetype,int64_t size);
void fillUpNormalHeaderComp(const char* mimetype,char* method,int64_t size);


void fillUpChunkedHeader(const char* mimetype);
void fillUpChunkedHeaderComp(const char* mimetype,char* method);


/*
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-------------------------------PARTIALS--------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
*/


#define HTTP_PARTIAL_CONTENT_OK_HEADER HTTP_VERSION_STRING PARTIAL_RESPONSE \
															HTTP_SERVER_FILE_AGE_HEADERS
																
												
#define STD_HEADER_START_PARTIAL HTTP_PARTIAL_CONTENT_OK_HEADER \
								"Accept-Ranges: bytes\r\n" \
								"Content-Type: %s\r\n"
				
																
#define NORMAL_PARTIAL_HEADER STD_HEADER_START_PARTIAL \
						"Content-Length: %ld\r\n" \
						"Content-Range: bytes %ld-%ld/%ld\r\n" \
						"\r\n"

#define NORMAL_PARTIAL_HEADER_COMP  STD_HEADER_START_PARTIAL \
							"Content-Length: %ld\r\n" \
							"Content-Range: bytes %ld-%ld/%ld\r\n" \
							  "Content-encoding: %s\r\n" \
							   "\r\n"

#define CHUNKED_PARTIAL_HEADER STD_HEADER_START_PARTIAL \
						"Content-Range: bytes %ld-%ld/%ld\r\n" \
						  "Transfer-Encoding: chunked\r\n" \
									  "\r\n"
									  
#define CHUNKED_PARTIAL_HEADER_COMP STD_HEADER_START_PARTIAL \
							"Content-Range: bytes %ld-%ld/%ld\r\n" \
							  "Transfer-Encoding: %s, chunked\r\n" \
							  "\r\n"
							  
void fillUpNormalPartialHeader(const char* mimetype,
										int64_t start_point,
										int64_t end_point,
										int64_t size,
										int64_t file_size);
										
void fillUpNormalPartialHeaderComp(const char* mimetype,char* method,
										int64_t start_point,
										int64_t end_point,
										int64_t size,
										int64_t file_size);


void fillUpChunkedPartialHeader(const char* mimetype,
										int64_t start_point,
										int64_t end_point,
										int64_t file_size);
										
void fillUpChunkedPartialHeaderComp(const char* mimetype,char* method,
										int64_t start_point,
										int64_t end_point,
										int64_t file_size);


/*
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-------------------------------UPLOADS---------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
*/

									
#define HTTP_CREATED_OK_HEADER HTTP_VERSION_STRING CREATED_RESPONSE \
													HTTP_SERVER_INTRO_HEADERS

#define UPLOAD_HEADER HTTP_CREATED_OK_HEADER \
					"Content-Type: %s\r\n" \
					"Location: " \
					"%s" \
					"://" \
					DOMAIN_NAME \
					":%hu%s%s" \
					"%s\r\n" \
					"\r\n"



void fillUploadHeader(const char* mimetype, char* target);


/*
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-------------------------------MISC------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
-----------------------------------------------------------------------------------
*/




#define REDIRECT_HEADER HTTP_VERSION_STRING SEE_OTHER_RESPONSE \
						HTTP_SERVER_INTRO_HEADERS \
						"Accept-Ranges: bytes\r\n" \
						"Location: %s\r\n" \
						"Content-Length: 0\r\n" \
						"\r\n"



#define HTTP_NOT_FOUND_HEADER HTTP_VERSION_STRING NOT_FOUND_RESPONSE \
						HTTP_SERVER_INTRO_HEADERS \
						"Accept-Ranges: bytes\r\n" \
						"Content-Type: %s\r\n" \
						"Content-Length: %ld\r\n" \
                          	"\r\n"


#define HTTP_METHOD_ERR_HEADER  HTTP_VERSION_STRING METHOD_NOT_ALLOWED_RESPONSE \
								HTTP_SERVER_INTRO_HEADERS \
								"Content-Length: 0\r\n" \
								"Allow: %s\r\n" \
								"\r\n"
								
#define HTTP_NOT_MODIFIED_HEADER  HTTP_VERSION_STRING NOT_MODIFIED_RESPONSE \
								HTTP_SERVER_FILE_AGE_HEADERS_NOT_MODIFIED \
								"\r\n"

void fillUpRedirectHeader(char* path);

void fillUpFileNotFoundHeader(int64_t size,const char* mimetype);

void fillUpRequestErrorHeader(void);

void fillUpNotModifiedHeader(void);

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
