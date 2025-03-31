#ifndef HTTP_RESPONSE_AUX_H
#define HTTP_RESPONSE_AUX_H

#define HTTP_OK_HEADER "HTTP/1.1 200 OK\r\n\n"
extern char* chunkedHeader,*chunkedHeaderComp,* normalHeader,*normalHeaderComp,* redirectHeader,*notFoundHeader;
void fillUpRedirectHeader(char headerBuff[PATHSIZE],char* path);
void fillUpChunkedHeader(char headerBuff[PATHSIZE],char*headerTemplate,const char* mimetype);
void fillUpChunkedHeaderComp(char headerBuff[PATHSIZE],char*headerTemplate,const char* mimetype,char* method);
void fillUpNormalHeader(char headerBuff[PATHSIZE],char*headerTemplate,u_int64_t size,const char* mimetype);
void fillUpNormalHeaderComp(char headerBuff[PATHSIZE],char* headerTemplate,u_int64_t size,const char* mimetype,char* method);
void fillUpFileNotFoundHeader(char headerBuff[PATHSIZE],u_int64_t size,const char* mimetype);
void fillUpFileNotFoundHeaderComp(char headerBuff[PATHSIZE],u_int64_t size,const char* mimetype,char* method);
#endif
