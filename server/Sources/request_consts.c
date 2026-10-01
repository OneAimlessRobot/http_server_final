#include "../Includes/preprocessor.h"
#include "../Includes/request_consts.h"
#include "../Includes/load_find_results.h"
#include "../Includes/load_html.h"
#include "../Includes/load_rss.h"
#include "../Includes/php_proxy.h"
#include "../Includes/rcon_proxy.h"
#include "../Includes/txt_sending.h"

SpecialFetchReqEntry the_custom_fetch_request_handler_table[]={
		{isRssFeed, generateRss, "%s.tmp%lx.rss", "%s.tmp1%lx.rss"},
		{isDirectory, generateDirListing, "%s.tmp%lx.dir", "%s.tmp1%lx.dir"},
		{isPHPRequest, generatePHPResult, "%s.tmp%lx.php", "%s.tmp1%lx.php"},
		{isText, generateText, "%s.tmp%lx.pretty_txt", "%s.tmp1%lx.pretty_txt"},
		{isDateReq, generateDate, "/.tmp%lx.date",NULL},
		{isRconRequest, generateRconResult, "/.tmp%lx.rcon", "/.tmp1%lx.rcon"},
		{isFindRequest, generateFindResult, "/.tmp%lx.find", "/.tmp1%lx.find"},
		{0}
};

request_handling_func_t get_post_parser_handler(const char *target, int64_t* index_obtained){
	
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
    *index_obtained=-1;
    return NULL;
}
const char* get_post_parser_handler_string(int64_t table_index) {
    return (the_custom_post_request_handler_table[table_index].the_req_string);
}

generating_func_t get_fetch_parser_handler(char *target, int64_t* index_obtained){
	
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
		  the_custom_fetch_request_handler_table[(*index_obtained)].the_checking_func != NULL;
		  (*index_obtained)++ ) {

        if (the_custom_fetch_request_handler_table[(*index_obtained)].the_checking_func(target)) {
            return the_custom_fetch_request_handler_table[(*index_obtained)].the_func;
        }
    }
    *index_obtained=-1;
    return NULL;
}
const char* get_fetch_parser_handler_first_format(int64_t table_index){
    return (the_custom_fetch_request_handler_table[table_index].tmp_file_format1);
}

const char* get_fetch_parser_handler_second_format(int64_t table_index){
    return (the_custom_fetch_request_handler_table[table_index].tmp_file_format2);
}

