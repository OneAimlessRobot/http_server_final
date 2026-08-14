#ifndef SEND_RESOURCE_FUNC_H
#define SEND_RESOURCE_FUNC_H

void send_wrapper(client* c,char* buff, int buff_size);

void sendResource(client*c,char* resourceTarget,int compress,int is_directory,int is_partial, int start_point, int is_head);

#endif
