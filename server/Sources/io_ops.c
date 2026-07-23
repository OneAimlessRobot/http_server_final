#include "../Includes/preprocessor.h"
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
#include "../Includes/http_response_aux.h"
#define SEND_FUNC_TO_USE sendsome


int sendsome_ssl(SSL* ssl, const char* buf, size_t len, int_pair times) {

	int sd= SSL_get_fd(ssl);
	size_t send_total = 0;
	while (send_total < len) {
	struct timeval tv;
	tv.tv_sec=times[0];
	tv.tv_usec=times[1];
	fd_set wrfds;
	FD_ZERO(&wrfds);
	FD_SET(sd, &wrfds);
	int iResult=select(sd + 1, (fd_set*)0, &wrfds, (fd_set*)0, &tv);
	if(iResult>0){
		int ret = SSL_write(ssl, buf + send_total, len - send_total);
	        if (ret > 0) {
	            send_total += ret;
	            continue;
	        }
		else if (ret == 0) {
			return send_total;
		}
		int ssl_err = SSL_get_error(ssl, ret);
		if (ssl_err == SSL_ERROR_WANT_READ) {
			struct timeval mtv;
			mtv.tv_sec=times[0];
			mtv.tv_usec=times[1];
			fd_set mrfds;
			FD_ZERO(&mrfds);
			FD_SET(sd, &mrfds);
			select(sd + 1, &mrfds, (fd_set*)0, (fd_set*)0, &mtv);
			continue;
		}
		else if (ssl_err == SSL_ERROR_WANT_WRITE) {
			struct timeval mtv;
			mtv.tv_sec=times[0];
			mtv.tv_usec=times[1];
			fd_set mwfds;
			FD_ZERO(&mwfds);
			FD_SET(sd, &mwfds);
			select(sd + 1, (fd_set*)0, &mwfds, (fd_set*)0, &mtv);
			continue;
		}
		else if (ssl_err == SSL_ERROR_ZERO_RETURN) {
			return send_total;
		}
		else if(ssl_err == SSL_ERROR_SYSCALL){
		    ERR_print_errors_fp(stderr);
		    if(logging){

				fprintf(logstream, "SSL SYSCALL ERROR AT SSL SEND\n%s\n",strerror(errno));
		    }
		    if(errno==EAGAIN){

			continue;
		    }
		    if(errno == EWOULDBLOCK){
			   continue;
			}
		     else{
			    if(logging){

					fprintf(logstream, "Will emergency func be called? %s\n",use_exit_func?"Yes!":"No..:");
			    }
			    if((errno!=ECONNRESET)&&use_exit_func){
					exit_func_for_this_module();
			    }
			    return -1;
			}
		}
		else{
		    ERR_print_errors_fp(stderr);
		    if(errno==EAGAIN){

			continue;
		    }
		    if(errno == EWOULDBLOCK){
			   continue;
			}
		if(logging){

			fprintf(logstream, "SSL FATAL ERROR AT SSL SEND\n%s\nWill emergency func be called? %s\n",strerror(errno),use_exit_func?"Yes!":"No..:");
		}
	           if(use_exit_func){
				exit_func_for_this_module();
		    }
		    return -1;
		}
	}
	else if(!iResult){
		return -2;
	}
	else{
		if(logging){

			fprintf(logstream, "SSL FATAL ERROR AT SSL SEND\n%s\nWill emergency func be called? %s\n",strerror(errno),use_exit_func?"Yes!":"No..:");
		}
		if(use_exit_func){
			exit_func_for_this_module();
		}
		return -1;
	}

}
return send_total;


}

int readsome_ssl(SSL* ssl, char* buf, size_t len, int_pair times) {
	int sd= SSL_get_fd(ssl);
	size_t read_total = 0;
	while (read_total < len) {
	struct timeval tv;
	tv.tv_sec=times[0];
	tv.tv_usec=times[1];
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(sd, &rfds);
	int iResult=select(sd + 1, &rfds, (fd_set*)0, (fd_set*)0, &tv);
	if(iResult>0){
		int ret = SSL_read(ssl, buf + read_total, len - read_total);
		if (ret > 0) {
			read_total += ret;
			continue;
		}
		else if (ret == 0) {
			return read_total;
		}
		int ssl_err = SSL_get_error(ssl, ret);
		if (ssl_err == SSL_ERROR_WANT_READ) {
			struct timeval mtv;
			mtv.tv_sec=times[0];
			mtv.tv_usec=times[1];
			fd_set mrfds;
			FD_ZERO(&mrfds);
			FD_SET(sd, &mrfds);
			select(sd + 1, &mrfds, (fd_set*)0, (fd_set*)0, &mtv);
			continue;
		}
		else if (ssl_err == SSL_ERROR_WANT_WRITE) {
			struct timeval mtv;
			mtv.tv_sec=times[0];
			mtv.tv_usec=times[1];
			fd_set wfds;
			FD_ZERO(&wfds);
			FD_SET(sd, &wfds);
			select(sd + 1, (fd_set*)0, &wfds, (fd_set*)0, &mtv);
			continue;
		}
		else if (ssl_err == SSL_ERROR_ZERO_RETURN) {
			return read_total;
		}
		else if(ssl_err == SSL_ERROR_SYSCALL){
		    ERR_print_errors_fp(stderr);
		    if(logging){

				fprintf(logstream, "SSL SYSCALL ERROR AT SSL READ\n%s\n",strerror(errno));
		    }
		    if(errno==EAGAIN){

			continue;
		    }
		    if(errno == EWOULDBLOCK){
			   continue;
			}
		    else{
			    if(logging){

					fprintf(logstream, "Will emergency func be called? %s\n",use_exit_func?"Yes!":"No..:");
			    }
			    if((errno!=ECONNRESET)&&use_exit_func){
				    exit_func_for_this_module();
			    }
			    return -1;
			}
		    }
		else{
		    ERR_print_errors_fp(stderr);
		    if(errno==EAGAIN){

			continue;
		    }
		    if(errno == EWOULDBLOCK){
			   continue;
			}
		   if(logging){

				fprintf(logstream, "SSL FATAL ERROR AT SSL READ\n%s\nWill emergency func be called? %s\n",strerror(errno),use_exit_func?"Yes!":"No..:");
		    }
		    if(use_exit_func){
				exit_func_for_this_module();
		    }
		    return -1;
		}
	}
	else if(!iResult){
		return -2;
	}
	else{
		if(logging){

			fprintf(logstream, "SSL FATAL ERROR AT SSL READ\n%s\nWill emergency func be called? %s\n",strerror(errno),use_exit_func?"Yes!":"No..:");
		}
		if(use_exit_func){
			exit_func_for_this_module();
		}
	        return -1;
	}

}
return read_total;

}

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
			char peek_byte;
			recv(c->socket, &peek_byte, 1, MSG_PEEK);

			if ((unsigned char)peek_byte == 0x16) {
				// It's probably HTTPS – drop the connection
				send(c->socket,notFoundHeader,strlen(notFoundHeader),0);
				fprintf(logstream,"Recebemos https!!!\nNOT Dropping!!!\n");
				raise(SIGINT);
				return -1;
			}
                return readall(c,buff,size);

                }
		else if(!iResult){
               	return -2;
		}
		else{
		if(logging){

		fprintf(logstream, "SELECT ERROR!!!!! READ\n");
		}
		raise(SIGINT);
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
		raise(SIGINT);
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
		raise(SIGINT);
		return -2;
	}
	else if(errno==ENOTCONN){
		if(logging){
		fprintf(logstream,"Li %ld ao todo!!!! readall saiu com erro!!!!!:\nAvisando server para desconectar!\n%s\n",total,strerror(errno));
		}
		raise(SIGINT);
		return -1;
	}
	else if(len!=-2){
		if(logging){
		fprintf(logstream,"Li %ld ao todo!!!! readall saiu com erro!!!!!:\n%s\n",total,strerror(errno));
		}
		raise(SIGINT);
		return -1;
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
    int64_t truesize = snprintf(chunkbuff, sizeof(chunkbuff), "%x\r\n", numread);
    memcpy(chunkbuff + truesize, buff, numread);
    memcpy(chunkbuff + truesize + numread, "\r\n", 2);

    int64_t totalsent = 0;
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
		raise(SIGINT);
		return -1;
		//continue;
	}
	}
	totalsent += sent;
    	//fprintf(logstream,"Send de %d bytes feito!!!\n",sent);
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
    int64_t truesize = snprintf(chunkbuff, sizeof(chunkbuff), "%x\r\n", numread);
    memcpy(chunkbuff + truesize, buff, numread);
    memcpy(chunkbuff + truesize + numread, "\r\n", 2);

    int64_t totalsent = 0;
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
		raise(SIGINT);
		break;
	}
	}
	totalsent += sent;
    	//fprintf(logstream,"Send de %d bytes feito!!!\n",sent);
	}
}

// Send final zero-sized chunk
send(c->socket, "0\r\n\r\n", 5, 0);
return 0;
}

int sendallstream(client*c,FILE* stream){

char buff[BUFFSIZE];
int numread;
int sent=0;
while ((numread = fread(buff,1,BUFFSIZE,stream)) > 0) {
    int64_t totalsent = 0;
    while (totalsent < numread) {
        errno=0;
	sent = SEND_FUNC_TO_USE(c->socket, buff + totalsent,  numread - totalsent);
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
		raise(SIGINT);
		break;
	}
	}
	totalsent += sent;
    	//fprintf(logstream,"Send de %d bytes feito!!!\n",sent);
    	}
}

return 0;
}


int sendallfd(client*c,int fd){

char buff[BUFFSIZE];
int numread;
int sent=0;
while ((numread = read(fd,buff,BUFFSIZE)) > 0) {
    int64_t totalsent = 0;
    while (totalsent < numread) {
        errno=0;
	sent = SEND_FUNC_TO_USE(c->socket, buff + totalsent,  numread - totalsent);
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
		raise(SIGINT);
		break;
	}
	}
	totalsent += sent;
    	//fprintf(logstream,"Send de %d bytes feito!!!\n",sent);
	}
}

return 0;
}

