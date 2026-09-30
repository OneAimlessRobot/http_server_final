#ifndef REQUEST_CONSTS_H
#define REQUEST_CONSTS_H



extern SpecialPostReqEntry the_custom_post_request_handler_table[];


/*

	Dealing with NULL is required for whoever calls this. There is no fallback

*/

request_handling_func_t get_parser_handler(const char *target, int64_t* index_obtained);

/*
	may return NULL
	expects valid indices
	
*/

const char* get_parser_handler_string(int64_t table_index);
u_int8_t get_parser_handler_is_specific(int64_t table_index);

#endif
