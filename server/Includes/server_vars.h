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
,use_chunked
,use_valgrind
,ignore_partial
,send_socket_size,
	recv_socket_size,
	serverOn,
	serverWasOn,
	use_tls,
	use_exit_func;

extern FILE* logstream;
extern compMethod br,gzip,bzip2,servComp;


extern char
	defaultTarget[FIELDSIZE],
	notFoundDefaultTarget[FIELDSIZE],
	mainRssFeed[FIELDSIZE],
	currDir[PATHSIZE],
	tmp_file_dir[PATHSIZE*2],
	tmp_directory_name[PATHSIZE],
	* defaultMimetype;

#endif
