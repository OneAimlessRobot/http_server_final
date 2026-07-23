#ifndef IO_OPS_H
#define IO_OPS_H

int sendsome_ssl(SSL* ssl, const char* buf, size_t len, int_pair times);

int readsome_ssl(SSL* ssl, char* buf, size_t len, int_pair times);

int readall(client* c,char* buff,int64_t size);

int timedreadall(client* c,char* buff,u_int64_t size);

int sendallchunkedfd(client* c,int fd);

int sendallchunkedstream(client* c,FILE* stream);

int sendallstream(client* c,FILE* stream);

int sendallfd(client* c,int fd);

extern exit_func_t exit_func_for_this_module;

#endif
