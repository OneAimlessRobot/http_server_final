#ifndef SOCK_OPS_H
#define SOCK_OPS_H

void setSocketRecvBuffSize(int sd,int size);

void setSocketSendBuffSize(int sd,int size);

int getSocketRecvBuffSize(int sd);

int getSocketSendBuffSize(int sd);

void setLinger(int socket,int onoff,int time);

void setNonBlocking(int socket);
#endif
