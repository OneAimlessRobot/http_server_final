#include "../Includes/preprocessor.h"
#include "../Includes/request_consts.h"


request_handling_func_t get_parser_handler(const char *target, int64_t* index_obtained){
	
	int64_t safeguard_ptr = 0;
    
    index_obtained = ((index_obtained!=NULL)?index_obtained:&safeguard_ptr);
	*index_obtained=-1;
    
    
    if (!target){
		return NULL;
	}
	if(!strnlen(target, PATHSIZE)){
		
		return NULL;
	}
    for ( (*index_obtained) = 0;
		  the_custom_post_request_handler_table[(*index_obtained)].the_req_string != NULL;
		  (*index_obtained)++ ) {

        if (strcmp(target, the_custom_post_request_handler_table[(*index_obtained)].the_req_string) == 0) {
            return the_custom_post_request_handler_table[(*index_obtained)].the_func;
        }
    }
    return NULL;
}
const char* get_parser_handler_string(int64_t table_index) {
    return (the_custom_post_request_handler_table[table_index].the_req_string);
}
u_int8_t get_parser_handler_is_specific(int64_t table_index) {
    return (the_custom_post_request_handler_table[table_index].is_specific);
}
