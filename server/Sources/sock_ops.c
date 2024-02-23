#include <sys/socket.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include "../Includes/buffSizes.h"
#include "../Includes/resource_consts.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/sock_ops.h"
void setSocketRecvBuffSize(int sd,int size){

setsockopt(sd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

}

void setSocketSendBuffSize(int sd,int size){

setsockopt(sd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)); // Send buffer 1K

}
int getSocketRecvBuffSize(int sd){

int sizevaluecontainer=0;
socklen_t sizeofbuff=sizeof(sizevaluecontainer);
getsockopt(sd, SOL_SOCKET, SO_RCVBUF, &sizevaluecontainer, &sizeofbuff);
return sizevaluecontainer;
}

int getSocketSendBuffSize(int sd){

int sizevaluecontainer=0;
socklen_t sizeofbuff=sizeof(sizevaluecontainer);
getsockopt(sd, SOL_SOCKET, SO_SNDBUF, &sizevaluecontainer, &sizeofbuff);
return sizevaluecontainer;
}

void setLinger(int socket,int onoff,int time){

struct linger so_linger;
so_linger.l_onoff = onoff; // Enable linger option
so_linger.l_linger = time; // Linger time, set to 0

if (setsockopt(socket, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger)) < 0) {
    perror("setsockopt");
    // Handle error
}

}
void setNonBlocking(int socket) {
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        fprintf(logstream,"erro a atribuir flags a uma socket de cliente: %s\n",strerror(errno));
        return;
    }

    if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
        fprintf(logstream,"erro a atribuir flags a uma socket de cliente: %s\n",strerror(errno));
    }
}
