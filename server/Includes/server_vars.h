#ifndef SERVER_VARS_H
#define SERVER_VARS_H
typedef struct compMethod{
	char * cmdTool,
		*fileExt,
		*encodingExt;


}compMethod;

extern u_int32_t smalltimeoutsecs,
	smalltimeoutusecs,

	bigtimeoutsecs,
	bigtimeoutusecs,

	tls_shake_timeout_secs,
	tls_shake_timeout_usecs,

	tls_data_timeout_secs,
	tls_data_timeout_usecs,
	
	recv_socket_size,
	send_socket_size;


extern u_int16_t port;


extern u_int32_t logging,
	serverOn,
	serverWasOn,
	use_valgrind,
	ignore_partial,
	use_tls,
	use_exit_func,
	use_chunked,
	use_compression,
	compression_level;


extern char currDir[PATHSIZE],
	defaultTarget[FIELDSIZE],
	notFoundDefaultTarget[FIELDSIZE],
	mainRssFeed[FIELDSIZE],
	*tmp_file_dir,
	*defaultMimetype;


extern FILE* logstream;

extern compMethod br,
		gzip,
		bzip2,
		servComp;


#endif
