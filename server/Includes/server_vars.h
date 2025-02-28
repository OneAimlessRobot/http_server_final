#ifndef SERVER_VARS_H
#define SERVER_VARS_H
typedef struct compMethod{
	char * cmdTool,*fileExt,*encodingExt;
	

}compMethod;
extern int logging,beeping;
extern int smalltimeoutsecs,smalltimeoutusecs;
extern int bigtimeoutsecs,bigtimeoutusecs;
extern int port;
extern int compression;
extern int compression_level;
extern int use_fd;
extern int use_chunked;
extern int quota;
extern int send_socket_size;
extern int recv_socket_size;
extern int session_time_usecs;
extern char currDir[PATHSIZE];
extern int serverOn;
extern FILE* logstream;
extern compMethod br,gzip,deflateM,servComp;

/*
extern char* logfpath,* defaultTarget,
*notFoundTarget,
*testScriptTarget,
*defaultTargetAdmin,
*defaultLoginTarget,
*doubleSessionTarget,
*timeoutTarget,
*incorrectLoginTarget;
*/


extern char
defaultTarget[FIELDSIZE],
defaultLoginTarget[FIELDSIZE],
doubleSessionTarget[FIELDSIZE],
incorrectLoginTarget[FIELDSIZE],
* defaultVersion,
* defaultMimetype;

#endif
