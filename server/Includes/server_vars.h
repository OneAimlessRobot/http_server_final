#ifndef SERVER_VARS_H
#define SERVER_VARS_H
typedef struct compMethod{
	char * cmdTool,*fileExt,*encodingExt;


}compMethod;
extern int logging
,beeping
,smalltimeoutsecs,smalltimeoutusecs
,bigtimeoutsecs,bigtimeoutusecs
,tls_shake_timeout_secs,tls_shake_timeout_usecs
,tls_data_timeout_secs,tls_data_timeout_usecs
,port
,compression
,compression_level
,use_fd
,use_chunked
,ignore_partial
,quota
,send_socket_size,
	recv_socket_size,
	session_time_usecs,
	serverOn,
	use_tls,
	use_exit_func;

extern char currDir[PATHSIZE];
extern FILE* logstream;
extern compMethod br,gzip,bzip2,servComp;

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
