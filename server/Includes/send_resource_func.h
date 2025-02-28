#ifndef SEND_RESOURCE_FUNC_H
#define SEND_RESOURCE_FUNC_H
#define USEFD 1
int isDirectory(char* resourceTarget);
int sendResource(client*c,char* resourceTarget,char* mimetype,int compress);
#endif
