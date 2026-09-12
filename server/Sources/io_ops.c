#include "../Includes/preprocessor.h"
#include "../Includes/filePaths.h"
#include "../Includes/client.h"
#include "../Includes/server_vars.h"
#include "../Includes/io_ops.h"

exit_func_t  exit_func_for_this_module=NULL;

char pull_buff[MAX_RECV_REQ_BUFFSIZE]={0},
	send_buff[MAX_RECV_REQ_BUFFSIZE]={0},
	send_chunkbuff[2 * MAX_RECV_REQ_BUFFSIZE + 10]={0};

int64_t sendsome(int sd,char* buff,u_int64_t size, int flags){
		if(sd < 0){
			return -1;
		}
		int iResult;
		struct timeval tv;
		int64_t send_total = 0;
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(sd,&rfds);
		tv.tv_sec=smalltimeoutsecs;
		tv.tv_usec=smalltimeoutusecs;
		iResult=select(sd+1,(fd_set*)0,&rfds,(fd_set*)0,&tv);
		if(iResult>0){
			send_total=send(sd,buff,size,flags);
			if(send_total < 0){
				return -1;
			}
			return send_total;
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
		return send_total;
}

int64_t sendsome_ssl(SSL* ssl,const char* buf, int64_t len) {

	int sd= SSL_get_fd(ssl);
	if(sd < 0){
		return -1;
	}
	int64_t send_total = 0;
	struct timeval tv;
	tv.tv_sec=tls_data_timeout_secs;
	tv.tv_usec=tls_data_timeout_usecs;
	fd_set wrfds;
	FD_ZERO(&wrfds);
	FD_SET(sd, &wrfds);
	int iResult=select(sd + 1, (fd_set*)0, &wrfds, (fd_set*)0, &tv);
	if(iResult>0){
		send_total = SSL_write(ssl, buf, len);
		if (send_total >= 0) {
			return send_total;
		}
		int ssl_err = SSL_get_error(ssl, send_total);
		if (ssl_err == SSL_ERROR_WANT_READ) {
			struct timeval mtv;
			mtv.tv_sec=tls_data_timeout_secs;
			mtv.tv_usec=tls_data_timeout_usecs;
			fd_set mrfds;
			FD_ZERO(&mrfds);
			FD_SET(sd, &mrfds);
			select(sd + 1, &mrfds, (fd_set*)0, (fd_set*)0, &mtv);
			return -2;
		}
		else if (ssl_err == SSL_ERROR_WANT_WRITE) {
			struct timeval mtv;
			mtv.tv_sec=tls_data_timeout_secs;
			mtv.tv_usec=tls_data_timeout_usecs;
			fd_set mwfds;
			FD_ZERO(&mwfds);
			FD_SET(sd, &mwfds);
			select(sd + 1, (fd_set*)0, &mwfds, (fd_set*)0, &mtv);
			return -2;
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
				return -2;
			}
			if(errno == EWOULDBLOCK){
				return -2;
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
				return -2;
			}
			if(errno == EWOULDBLOCK){
				return -2;
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
	return send_total;


}

int64_t readsome(int sd,char buff[],int64_t size,int flags){
		if(sd < 0){
			return -1;
		}
		int iResult;
		struct timeval tv;
		int64_t read_total=0;
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(sd,&rfds);
		tv.tv_sec=smalltimeoutsecs;
		tv.tv_usec=smalltimeoutusecs;
		iResult=select(sd+1,&rfds,(fd_set*)0,(fd_set*)0,&tv);
		if(iResult>0){
			read_total=recv(sd,buff,size,flags);
			if(read_total < 0){
				return -1;
			}
			return read_total;
		}
		else if(!iResult){
				return -2;
		}
		else{
			if(logging){

				fprintf(logstream, "SELECT ERROR!!!!! READ\n%s\n",strerror(errno));
			}
			if(use_exit_func){
				exit_func_for_this_module();
			}
			return -1;
		}
}

int64_t readsome_ssl(SSL* ssl,char* buf, int64_t len) {
	
	int sd= SSL_get_fd(ssl);
	if(sd < 0){
		return -1;
	}
	int has_pending_result=SSL_has_pending(ssl);
	int pending_result=SSL_pending(ssl);
	if(has_pending_result){
		if(pending_result > 0){
			goto read_label;
		}
	}
	int64_t read_total = 0;
	struct timeval tv;
	tv.tv_sec=tls_data_timeout_secs;
	tv.tv_usec=tls_data_timeout_usecs;
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(sd, &rfds);
	int iResult=select(sd + 1, &rfds, (fd_set*)0, (fd_set*)0, &tv);
	if(iResult>0){
		read_label:
		read_total = SSL_read(ssl, buf, len);
		if (read_total >= 0) {
			return read_total;
		}
		int ssl_err = SSL_get_error(ssl, read_total);
		if (ssl_err == SSL_ERROR_WANT_READ) {
			struct timeval mtv;
			mtv.tv_sec=tls_data_timeout_secs;
			mtv.tv_usec=tls_data_timeout_usecs;
			fd_set mrfds;
			FD_ZERO(&mrfds);
			FD_SET(sd, &mrfds);
			select(sd + 1, &mrfds, (fd_set*)0, (fd_set*)0, &mtv);
			return -2;
		}
		else if (ssl_err == SSL_ERROR_WANT_WRITE) {
			struct timeval mtv;
			mtv.tv_sec=tls_data_timeout_secs;
			mtv.tv_usec=tls_data_timeout_usecs;
			fd_set wfds;
			FD_ZERO(&wfds);
			FD_SET(sd, &wfds);
			select(sd + 1, (fd_set*)0, &wfds, (fd_set*)0, &mtv);
			return -2;
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

				return -2;
			}
			if(errno == EWOULDBLOCK){
				return -2;
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

				return -2;
			}
			if(errno == EWOULDBLOCK){
			   return -2;
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
	return read_total;

}

int64_t readall(client*c,char* buff){
	int64_t total=0,
			len=0;
	
	while(1){
		
		len=c->is_tls?readsome_ssl(c->ctx_if_ssl,buff+total,BUFFSIZE-total):readsome(c->socket,buff+total,BUFFSIZE-total ,0);
		
		if(len<0){
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				if(logging > 49){
					fprintf(logstream,"Would block/Try again later!!!!\n");
				}
				continue;
			}
			else if(errno==EPIPE){

				if(logging){
					fprintf(logstream,"Pipe partido!!!\n");
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
		else if(len >=0){
			total+=len;
			break;
		}
	}
	return total;

}
int64_t pullallfd(int fd){
    int64_t len=(curr_client.enable_first_time_in_upload?curr_client.req->begin_data_size:1);
	int64_t total=0;
	u_int8_t first_time = curr_client.enable_first_time_in_upload;
	
	for(;(len==-2||len>0)&&(total<curr_client.req->data_size);){
		if(first_time){
			memcpy(pull_buff,curr_client.req->begin_data, curr_client.req->begin_data_size);
			
			if(logging> 50){
				fprintf(logstream,"pullallfd appended %ld to file in first time read!\n",len);
			}
			first_time =0;
			
		}
		else{
			len= readall(&curr_client,pull_buff);
			if(logging > 50){
				fprintf(logstream,"pullallfd subread devolve resultado de %ld!\n%ld out of %ld bytes accumulated!\n",len,
								total+(len>=0?len:0),
								curr_client.req->data_size);
			}
		}
		
		if(len > 0){
			size_t written_total = 0;
			while(written_total < (size_t)len){
				ssize_t w = write(fd, pull_buff + written_total, len - written_total);
				if(w > 0){
					written_total += w;
					total += w;
					
				} else if(errno == EAGAIN || errno == EWOULDBLOCK){
					continue; // try again
				} else {
					if(logging){
						fprintf(logstream,"write error em pullallfd: %s\n", strerror(errno));
					}
					return -1;
				}
			}
		}
		else if(len==-2){
				if(logging>3){
					fprintf(logstream,"We continued due to timeout!\nLen is: %ld\n", len);
				}
				continue;
		}
		else{
		
			break;
		}
	}
	if(!(total-curr_client.req->data_size) && logging){
		
		fprintf(logstream,"pullallfd bem sucedido?\n"
						"O numero de bytes escrito\n"
						"É igual ao numero de bytes prescritos\n"
						"No request...\n");

	}
	else if(logging){
		
		fprintf(logstream,"pullallfd saiu sem erros\n"
						"mas com numero de bytes escrito diferente\n"
						"Do que o que foi prescrito\n"
						"no request...\n");
		
	}
	if(len<0){
		
		if(errno==EPIPE){

			if(logging){
				fprintf(logstream,"Pipe partido em pullallfd!!!\n");
			}
			return -2;
		}
		else if(errno==ENOTCONN){
			if(logging){
				fprintf(logstream,"pullallfd saiu com erro!!!!!:\nAvisando server para desconectar!\n%s\n",strerror(errno));
			}
			return -2;
		}
		else if(len!=-2){
			if(logging){
				fprintf(logstream,"pullallfd saiu com erro!!!!!:\n%s\n",strerror(errno));
			}
		}
	}
	if(logging){
		fprintf(logstream,"\n%ld de %ld bytes foram lidos!\n",total,curr_client.req->data_size);
	}
    return 0;
}
int64_t sendallchunkedfd(client*c,int fd, int64_t start_point, int64_t size_to_send){

int64_t numread;
int64_t sent=0;
int64_t num_bytes_sent=0;
lseek(fd,start_point,SEEK_SET);
dprintf(1,"Beginning to send from %ld to %ld (%ld bytes)\n",start_point,start_point+size_to_send-1,size_to_send);
while ((numread = read(fd,send_buff, PARTIAL_SIZE_TO_SEND )) > 0) {
    int64_t truesize = snprintf(send_chunkbuff, sizeof(send_chunkbuff), "%lx\r\n", numread);
    memcpy(send_chunkbuff + truesize, send_buff, numread);
    memcpy(send_chunkbuff + truesize + numread, "\r\n", 2);

    int64_t totalsent = 0;
    while (totalsent < truesize + numread + 2) {
        errno=0;
	sent = c->is_tls?sendsome_ssl(c->ctx_if_ssl, send_chunkbuff + totalsent, truesize + numread + 2 - totalsent)
					:
					sendsome(c->socket, send_chunkbuff + totalsent, truesize + numread + 2 - totalsent, 0);
	if(sent==-2){
		continue;
	}
	if(sent<0){
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                if(logging){
		fprintf(logstream,"Block no sending!!!!: %s\n",strerror(errno));
                }
		break;
        }
	else if(errno==EPIPE){

		if(logging){
		fprintf(logstream,"Pipe partido!!\n");
		}
		return -1;
	}
        else if(errno == ECONNRESET){
		if(logging){
                fprintf(logstream,"Conexão largada!! SIGPIPE!!!!!: %s\n",strerror(errno));
                }
		return -1;
	}
	else {
		if(logging){
                fprintf(logstream,"Outro erro qualquer!!!!!: %d %s\n",errno,strerror(errno));
                }
		return -1;
		//continue;
	}
	}
	totalsent += sent;
    	}
	num_bytes_sent += totalsent;
}

dprintf(1,"A total of %ld bytes was sent!\n",num_bytes_sent);
c->is_tls?SSL_write(c->ctx_if_ssl, "0\r\n\r\n", 5):send(c->socket, "0\r\n\r\n", 5, 0);
return 0;
}



int64_t sendallfd(client*c,int fd, int64_t start_point, int64_t size_to_send){

int64_t numread;
int64_t sent=0;
int64_t num_bytes_sent=0;

lseek(fd,start_point,SEEK_SET);
dprintf(1,"Beginning to send from %ld to %ld (%ld bytes)\n",start_point,start_point+size_to_send-1,size_to_send);
while ((numread = read(fd,send_buff,PARTIAL_SIZE_TO_SEND)) > 0) {
    int64_t totalsent = 0;
    while (totalsent < numread) {
        errno=0;
	sent = c->is_tls?sendsome_ssl(c->ctx_if_ssl, send_buff + totalsent,  numread - totalsent)
					:
					sendsome(c->socket, send_buff + totalsent,  numread - totalsent, 0);
	if(sent==-2){

		continue;
	}
	if(sent<0){
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                if(logging){
					fprintf(logstream,"Block no sending!!!! %s\n",strerror(errno));
                }
		break;

        }
	else if(errno==EPIPE){

		if(logging){
		fprintf(logstream,"Pipe partido!!!\n");
		}
		return -1;
	}
        else if(errno == ECONNRESET){
		if(logging){
                fprintf(logstream,"Conexão largada!!\nSIGPIPE!!!!!: %s\n",strerror(errno));
                }
		return -1;
	}
	else {
		if(logging){
                fprintf(logstream,"Outro erro qualquer!!!!!: %d %s\n",errno,strerror(errno));
                }
		break;
	}
	}
	totalsent += sent;
	}
	num_bytes_sent += totalsent;
}

dprintf(1,"A total of %ld bytes was sent!\n",num_bytes_sent);
return 0;
}

