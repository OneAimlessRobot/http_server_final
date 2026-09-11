#ifndef COOL_MACROS_H
#define COOL_MACROS_H

#define min(a,b) (a<b?a:b)

#define max(a,b) (a>b?a:b)

#define b_zero(a) (memset(a,0,sizeof(a)))

#define ato_ull(a) (strtoull(a, NULL, 10))

#define BUFFSIZE (min(MAX_RECV_REQ_BUFFSIZE,recv_req_buff_size))


#endif
