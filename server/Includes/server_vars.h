#ifndef SERVER_VARS_H
#define SERVER_VARS_H


extern u_int64_t smalltimeoutsecs,
	smalltimeoutusecs,

	bigtimeoutsecs,
	bigtimeoutusecs,

	tls_shake_timeout_secs,
	tls_shake_timeout_usecs,

	tls_data_timeout_secs,
	tls_data_timeout_usecs,
	
	recv_socket_size,
	send_socket_size,
	recv_req_buff_size,
	
	s_maxage;

extern u_int16_t port;



extern u_int8_t logging,
	serverOn,
	serverWasOn,
	use_valgrind,
	use_fancy_find,
	ignore_partial,
	use_tls,
	want_tls,
	use_exit_func,
	use_chunked,
	use_compression,
	lower_security_for_compat,
	compression_level,
	keep_alive;


extern char currDir[PATHSIZE],
	defaultTarget[FIELDSIZE],
	notFoundDefaultTarget[FIELDSIZE],
	internalServerErrorTarget[FIELDSIZE],
	mainRssFeed[FIELDSIZE],
	robotsTarget[FIELDSIZE],
	excludePrettifyTarget[FIELDSIZE],
	valgrindSuppressionsFile[FIELDSIZE],
	bind_iface_addr[FIELDSIZE],
	*tmp_file_dir,
	*defaultMimetype,
	*robotsMimetype;


extern FILE* logstream;

extern compMethod 
		* servComp,
		methods_for_compression[];



#endif
