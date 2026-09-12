#ifndef COOL_MACROS_H
#define COOL_MACROS_H

#define min(a,b) ((int64_t)a<(int64_t)b?(int64_t)a:(int64_t)b)

#define max(a,b) ((int64_t)a>(int64_t)b?(int64_t)a:(int64_t)b)

#define b_zero(a) (memset(a,0,sizeof(a)))

#define ato_ull(a) (strtoull(a, NULL, 10))

#define BUFFSIZE (min(MAX_RECV_REQ_BUFFSIZE,recv_req_buff_size))

#define DIFF_TIME(a,b) (lround(difftime( a, b )))

#define KEEP_ALIVE_STRING (keep_alive?"keep-alive":"close")

#define WILL_CACHE (curr_client.would_cache && curr_client.should_cache)

#define PARTIAL_SIZE_TO_SEND max(0,min(BUFFSIZE,size_to_send-num_bytes_sent))
#endif
