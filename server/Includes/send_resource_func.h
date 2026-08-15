#ifndef SEND_RESOURCE_FUNC_H
#define SEND_RESOURCE_FUNC_H

void send_wrapper(char* buff, int buff_size);

void sendResource(int compress,int is_directory,int is_partial, int start_point, int is_head);

#endif
