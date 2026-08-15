#ifndef IO_OPS_H
#define IO_OPS_H

int sendsome_ssl(SSL* ssl, const char* buf, size_t len);

int readsome_ssl(SSL* ssl, char* buf, size_t len);

int readsome(int sd,char buff[],u_int64_t size,int flags);

int readall(client* c,char* buff);

int timedreadall(client* c,char* buff);

int sendallchunkedfd(client* c,int fd, u_int64_t start_point);

int sendallfd(client* c,int fd, u_int64_t start_point);

extern exit_func_t exit_func_for_this_module;

#endif
