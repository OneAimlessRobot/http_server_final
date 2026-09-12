#ifndef IO_OPS_H
#define IO_OPS_H

int64_t sendsome_ssl(SSL* ssl, const char* buf, int64_t len);

int64_t readsome_ssl(SSL* ssl, char* buf, int64_t len);

int64_t readsome(int sd,char* buff,int64_t size,int flags);

int64_t readall(client* c,char* buff);

int64_t sendallchunkedfd(client*c,int fd, int64_t start_point, int64_t size_to_send);

int64_t sendallfd(client* c,int fd, int64_t start_point, int64_t size_to_send);

int64_t pullallfd(int fd);

extern exit_func_t exit_func_for_this_module;

#endif
