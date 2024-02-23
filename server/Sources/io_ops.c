#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include "../Includes/buffSizes.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/io_ops.h"
#define SEND_FUNC_TO_USE sendsome


int readsome(int sd,char buff[],u_int64_t size){
		int iResult;
                struct timeval tv;
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sd,&rfds);
                tv.tv_sec=smalltimeoutsecs;
                tv.tv_usec=smalltimeoutusecs;
                iResult=select(sd+1,&rfds,(fd_set*)0,(fd_set*)0,&tv);
                if(iResult>0){

                return recv(sd,buff,size,0);

                }
		else if(!iResult){
               	return -2;
		}
		else{
		if(logging){

		fprintf(logstream, "SELECT ERROR!!!!! READ\n");
		}
		return -1;
		}
}
int timedreadall(client* c,char* buff,u_int64_t size){
		int iResult;
                struct timeval tv;
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(c->socket,&rfds);
                tv.tv_sec=bigtimeoutsecs;
                tv.tv_usec=bigtimeoutusecs;
                iResult=select(c->socket+1,&rfds,(fd_set*)0,(fd_set*)0,&tv);
                if(iResult>0){

                return readall(c,buff,size);

                }
		else if(!iResult){
               	return -2;
		}
		else{
		if(logging){

		fprintf(logstream, "SELECT ERROR!!!!! READ\n");
		}
		return -1;
		}
}

int sendsome(int sd,char buff[],u_int64_t size){
                int iResult;
                struct timeval tv;
                fd_set rfds;
                FD_ZERO(&rfds);
                FD_SET(sd,&rfds);
                tv.tv_sec=smalltimeoutsecs;
                tv.tv_usec=smalltimeoutusecs;
                iResult=select(sd+1,(fd_set*)0,&rfds,(fd_set*)0,&tv);
                if(iResult>0){

                return send(sd,buff,size,0);
                }
		else if(!iResult){
               	return -2;
		}
		else{
		if(logging){

		fprintf(logstream, "SELECT ERROR!!!!! SEND\n");
		}
		return -1;
		}
}

int readall(client*c,char* buff,int64_t size){
        int64_t len=0,
		 total=0;
while(total<size){
        len=readsome(c->socket,buff+total,size-total);
	if(len<=0){
	
                break;
	
	}
	else{
	total+=len;
	}
	if(total==size){
	break;
	}
}
	
	if(len<0){
	if (errno == EAGAIN || errno == EWOULDBLOCK) {
        	if(logging){
		fprintf(logstream,"Li %ld ao todo!!!! readall bem sucedido!! A socket e %d\n",total,c->socket);
		}
	}
	else if(errno==EPIPE){

		if(logging){
		fprintf(logstream,"Pipe partido!!! A socket e %d\n",c->socket);
		}
		return -2;
	}
	else if(errno==ENOTCONN){
		if(logging){
		fprintf(logstream,"Li %ld ao todo!!!! readall saiu com erro!!!!!:\nAvisando server para desconectar!\n%s\n",total,strerror(errno));
		}
		
		return -2;
	}
	else if(len!=-2){
		if(logging){
		fprintf(logstream,"Li %ld ao todo!!!! readall saiu com erro!!!!!:\n%s\n",total,strerror(errno));
		}
	}
	
	}
        
        return total;

}


int sendallchunkedfd(client*c,int fd){

char buff[BUFFSIZE];
char chunkbuff[2 * BUFFSIZE + 10];  // Additional space for size header and CRLF
int numread;
int sent=0;
while ((numread = read(fd,buff, BUFFSIZE)) > 0) {
    int truesize = snprintf(chunkbuff, sizeof(chunkbuff), "%x\r\n", numread);
    memcpy(chunkbuff + truesize, buff, numread);
    memcpy(chunkbuff + truesize + numread, "\r\n", 2);

    int totalsent = 0;
    while (totalsent < truesize + numread + 2) {
        errno=0;
	sent = SEND_FUNC_TO_USE(c->socket, chunkbuff + totalsent, truesize + numread + 2 - totalsent);
	if(sent==-2){

		if(logging){
		fprintf(logstream,"Timeout no sending!!!!: %s\nsocket %d\n",strerror(errno),c->socket);
                }
		continue;
	}
	if(sent<0){
	
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                if(logging){
		fprintf(logstream,"Block no sending!!!!: %s\nsocket %d\n",strerror(errno),c->socket);
                }
		break;
	
        }
	else if(errno==EPIPE){

		if(logging){
		fprintf(logstream,"Pipe partido!!! A socket e %d\n",c->socket);
		}
		raise(SIGINT);
		return -1;
	}
        else if(errno == ECONNRESET){
		if(logging){
                fprintf(logstream,"Conexão largada!! SIGPIPE!!!!!: %s\n",strerror(errno));
                }
		raise(SIGINT);
		return -1;
	}
	else {
		if(logging){
                fprintf(logstream,"Outro erro qualquer!!!!!: %d %s\n",errno,strerror(errno));
                }
	
		break;
		//continue;
	
	}
        }
	else{
	if(logging){
	fprintf(logstream,"send de %d bytes feito!!!!!\n",sent);
	}
	totalsent += sent;
    	}
	}
}

// Send final zero-sized chunk
send(c->socket, "0\r\n\r\n", 5, 0);
return 0;
}

int sendallchunkedstream(client*c,FILE* stream){

char buff[BUFFSIZE];
char chunkbuff[2 * BUFFSIZE + 10];  // Additional space for size header and CRLF
int numread;
int sent=0;
while ((numread = fread(buff,1,BUFFSIZE,stream)) > 0) {
    int truesize = snprintf(chunkbuff, sizeof(chunkbuff), "%x\r\n", numread);
    memcpy(chunkbuff + truesize, buff, numread);
    memcpy(chunkbuff + truesize + numread, "\r\n", 2);

    int totalsent = 0;
    while (totalsent < truesize + numread + 2) {
        errno=0;
	sent = SEND_FUNC_TO_USE(c->socket, chunkbuff + totalsent, truesize + numread + 2 - totalsent);
	if(sent==-2){

		if(logging){
		fprintf(logstream,"Timeout no sending!!!!: %s\nsocket %d\n",strerror(errno),c->socket);
                }
		continue;
	}
	if(sent<0){
	
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                if(logging){
		fprintf(logstream,"Block no sending!!!!: %s\nsocket %d\n",strerror(errno),c->socket);
                }
		break;

        }
	else if(errno==EPIPE){

		if(logging){
		fprintf(logstream,"Pipe partido!!! A socket e %d\n",c->socket);
		}
		raise(SIGINT);
		return -1;
	}
        else if(errno == ECONNRESET){
		if(logging){
                fprintf(logstream,"Conexão largada!!\nSIGPIPE!!!!!: %s\n",strerror(errno));
                }
		raise(SIGINT);
		return -1;
	}
	else {
		if(logging){
                fprintf(logstream,"Outro erro qualquer!!!!!: %d %s\n",errno,strerror(errno));
                }
	
		break;
	}
        }
	else{
	if(logging){
	fprintf(logstream,"send de %d bytes feito!!!!!\n",sent);
	}
	totalsent += sent;
    	}
	}
}

// Send final zero-sized chunk
send(c->socket, "0\r\n\r\n", 5, 0);
return 0;
}

