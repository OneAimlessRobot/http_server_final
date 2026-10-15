#ifndef COOL_MACROS_H
#define COOL_MACROS_H

#define min(a,b) ((int64_t)a<(int64_t)b?(int64_t)a:(int64_t)b)

#define max(a,b) ((int64_t)a>(int64_t)b?(int64_t)a:(int64_t)b)

#define is_between_both_closed(n,s,B) ((n>=s)&&(n<=B))

#define is_between_both_open(n,s,B) ((n>s)&&(n<B))

#define b_zero(a) (memset(a,0,sizeof(a)))

#define ato_ull(a) (strtoull(a, NULL, 10))

#define BUFFSIZE (min(MAX_RECV_REQ_BUFFSIZE,recv_req_buff_size))

#define DIFF_TIME(a,b) (lround(difftime( a, b )))

#define KEEP_ALIVE_STRING (keep_alive?"keep-alive":"close")

#define WILL_CACHE (curr_client.would_cache && curr_client.should_cache)

#define PARTIAL_SIZE_TO_SEND max(0,min(BUFFSIZE,size_to_send-num_bytes_sent))


typedef enum {BYTE=0,KILO,MEGA,GIGA,TERA, MAX_SUFFIX}magnitude_order;


#define byte_to_high_order(a,b) (a>>(10*(magnitude_order)b))

#define high_order_to_byte(a,b) (a<<(10*(magnitude_order)b))

/*
	it means 10 mega
	the default size at which things will stop
	being cached after.
	If this size were absurd
	Videos would take ages to load
	With no real benefit
	
*/

#define DEFALT_MAX_CACHE_SIZE (high_order_to_byte((int64_t)10,MEGA))
/*
	same thing for compression
*/
#define DEFAULT_MAX_COMPRESS_SIZE (high_order_to_byte((int64_t)10,MEGA))

#endif
