#ifndef CUSTOM_FUNC_POINTERS_H
#define CUSTOM_FUNC_POINTERS_H

typedef void (*exit_func_t)(void);
typedef void (*generating_func_t)(char*);

typedef void (*request_handling_func_t)(http_request*);

typedef struct {
	
    const char *the_req_string;
	request_handling_func_t the_func;
	u_int8_t is_specific;

} SpecialPostReqEntry;

#endif
