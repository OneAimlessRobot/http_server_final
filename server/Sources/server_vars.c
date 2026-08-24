#include "../Includes/preprocessor.h"
#include "../Includes/server_vars.h"


u_int32_t smalltimeoutsecs,
	smalltimeoutusecs,

	bigtimeoutsecs,
	bigtimeoutusecs,

	tls_shake_timeout_secs,
	tls_shake_timeout_usecs,

	tls_data_timeout_secs,
	tls_data_timeout_usecs,
	
	recv_socket_size,
	send_socket_size;


u_int16_t port = 0;


u_int8_t logging=0,
	serverOn=0,
	serverWasOn=0,
	use_valgrind=0,
	use_fancy_find=0,
	ignore_partial=0,
	use_tls=0,
	use_exit_func=0,
	use_chunked=0,
	use_compression=0,
	compression_level=0,
	lower_security_for_compat=0;


char currDir[PATHSIZE] = {0},
	defaultTarget[FIELDSIZE] = {0},
	notFoundDefaultTarget[FIELDSIZE] = {0},
	mainRssFeed[FIELDSIZE] = {0},
	uploadTarget[FIELDSIZE] = {0},
	robotsTarget[FIELDSIZE] = {0},
	tmp_directory_name[PATHSIZE] = {0},
	*tmp_file_dir= NULL,
	*defaultMimetype = "*/*",
	*robotsMimetype = "text/plain";

FILE* logstream=NULL;

compMethod methods_for_compression[] = {
					{"brotli","br"," br", "%s -c -%hhu %s"},
					{"gzip","gzip"," gzip", "%s -c -%hhu %s"},
					{"bzip2","bz2"," bzip2", "\"%s -c -%hhu %s\""},
					{"deflate","deflate", " deflate", "./deflate/build/%s -c -%hhu %s"},
					{0}
					},
					
			* servComp = NULL;
	
		


