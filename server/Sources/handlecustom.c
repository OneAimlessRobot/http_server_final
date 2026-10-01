#include "../Includes/preprocessor.h"
#include "../Includes/client.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/request_consts.h"
#include "../Includes/handlecustom.h"



    
static void handlePostRequest(http_request* req){
	char* argv2[ARGVMAX]={0};
	
	if(!strnlen(req->form,sizeof(req->form))){
			return;
	}
	int size=make_str_arr(req->form,"&",argv2,ARGVMAX,0);
	
	char* the_ptr=req->processed_form;
	for(int i=0;i<size;i++){
		if(!argv2[i]){
			continue;
		}
		string_pair pair={0};
		splitString(argv2[i],"=",pair);
		replaceStringCharacter(pair[1],'+',' ');
		the_ptr+=snprintf(the_ptr,
				sizeof(req->processed_form)-(the_ptr-req->processed_form),
				"%s%s",pair[1]?pair[1]:"","|");
		
	}
}



SpecialPostReqEntry the_custom_post_request_handler_table[] = {
	
		{ "/find.find", handlePostRequest},
		
		{ "/rcon.rcon", handlePostRequest},
		
		{0}
	
	
};


int sanitize_post_form(http_request* req){
	
	request_handling_func_t the_handler = get_post_parser_handler(req->is_a_form?req->header->target:req->header->specific, &curr_client.post_request_table_cell);
	
	int result = (curr_client.post_request_table_cell>=0);
	if(the_handler){
		the_handler(req);
	}
	return result;
}

