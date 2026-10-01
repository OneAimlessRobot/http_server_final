#ifndef REQUEST_CONSTS_H
#define REQUEST_CONSTS_H



extern SpecialPostReqEntry the_custom_post_request_handler_table[];


/*

	Dealing with NULL is required for whoever calls this. There is no fallback

*/

request_handling_func_t get_post_parser_handler(const char *target, int64_t* index_obtained);

/*
	may return NULL
	expects valid indices
	
*/

const char* get_post_parser_handler_string(int64_t table_index);






extern SpecialFetchReqEntry the_custom_fetch_request_handler_table[];




/*

	Dealing with NULL is required for whoever calls this. There is no fallback

*/

generating_func_t get_fetch_parser_handler(char *target, int64_t* index_obtained);

/*
	may return NULL
	expects valid indices
	
*/

const char* get_fetch_parser_handler_first_format(int64_t table_index);

const char* get_fetch_parser_handler_second_format(int64_t table_index);


#endif
