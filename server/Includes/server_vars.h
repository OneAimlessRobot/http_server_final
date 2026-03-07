#ifndef SERVER_VARS_H
#define SERVER_VARS_H
typedef struct compMethod{
	char * cmdTool,*fileExt,*encodingExt;


}compMethod;
extern int logging
,beeping
,smalltimeoutsecs,smalltimeoutusecs
,bigtimeoutsecs,bigtimeoutusecs
,port
,compression
,compression_level
,use_fd
,use_chunked
,quota
,send_socket_size,
	recv_socket_size,
	session_time_usecs,
	serverOn;

extern char currDir[PATHSIZE];
extern FILE* logstream;
extern compMethod br,gzip,deflateM,bzip2,servComp;

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
notFoundDefaultTarget[FIELDSIZE],
mainRssFeed[FIELDSIZE],
* defaultVersion,
* defaultMimetype;

#endif
