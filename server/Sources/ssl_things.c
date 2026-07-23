#include "../Includes/preprocessor.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/handlecustom.h"
#include "../Includes/server_vars.h"
#include "../Includes/load_logins.h"
#include "../Includes/load_html.h"
#include "../Includes/load_rss.h"
#include "../Includes/php_proxy.h"
#include "../Includes/rcon_proxy.h"
#include "../Includes/send_resource_func.h"
#include "../Includes/server_innards.h"
#include "../Includes/ssl_things.h"

uint8_t SERVER_SSL_initted_in_process=0;

uint8_t SSL_on_in_process=0;

uint8_t CLIENT_SSL_initted_in_process=0;


SSL_CTX *global_server_ctx=NULL,
        *global_client_ctx=NULL;
        
int verify_callback(int ok, X509_STORE_CTX *ctx) {
    if (!ok) {
        int err = X509_STORE_CTX_get_error(ctx);
        printf("Verify error: %s\n",
               X509_verify_cert_error_string(err));
    }
    return ok;
}

void InitializeSSL(void){
     if(will_use_tls){
	if(SSL_on_in_process){
		if(logging){
			fprintf(logstream,"SSL já ativo! Ignorando!\n");
		}
		return;
	}
	else{
		if(logging){
			fprintf(logstream,"SSL ativo! SSL_on_in_process: 0 -> 1 !\n");
		}
		SSL_on_in_process=1;
	}
    printf("OpenSSL version: %s\n", OPENSSL_VERSION_TEXT);
    printf("OpenSSL version text func: %s\n", OpenSSL_version(OPENSSL_VERSION));
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
   }
}

void DestroySSL(void){
	if(will_use_tls){
		if(!SSL_on_in_process){
			if(logging){
				fprintf(logstream,"SSL já desativado! Ignorando!\n");
			}
			return;
		}
		else{
			if(logging){
				fprintf(logstream,"SSL desativado! SSL_on_in_process: 1 -> 0 !\n");
			}
			SSL_on_in_process=0;
		}

	    ERR_free_strings();
	    EVP_cleanup();
	}
}

void ShutdownSSL(SSL** cSSL){
	if((*cSSL)!=NULL){
		SSL_shutdown(*cSSL);
		SSL_free(*cSSL);
		*cSSL=NULL;
	}
}
void convert_server_con_to_ssl(SSL** cSSL, int sd,int_pair times){

	(*cSSL)=SSL_new(global_server_ctx);
	SSL_set_fd(*cSSL, sd);
	//Here is the SSL Accept portion.  Now all reads and writes must use SSL
	int ssl_ret=0,ssl_err=0;
	while(1) {
		ssl_ret  = SSL_accept(*cSSL);

		if (ssl_ret == 1) {
			break;
		}

		ssl_err = SSL_get_error(*cSSL, ssl_ret);

		if (ssl_err == SSL_ERROR_WANT_READ) {
			struct timeval tv;
			tv.tv_sec=times[0];
			tv.tv_usec=times[1];
			fd_set rfds;
			FD_ZERO(&rfds);
			FD_SET(sd, &rfds);
			select(sd + 1, &rfds, (fd_set*)0, (fd_set*)0, &tv);
		}
		else if (ssl_err == SSL_ERROR_WANT_WRITE) {
			struct timeval tv;
			tv.tv_sec=times[0];
			tv.tv_usec=times[1];
			fd_set wfds;
			FD_ZERO(&wfds);
			FD_SET(sd, &wfds);
			select(sd + 1, (fd_set*)0, &wfds, (fd_set*)0, &tv);
		}
		else {
			if(logging){
				fprintf(logstream, "SSL handshake failed:\n");
				ERR_print_errors_fp(logstream);
			}
			ShutdownSSL(cSSL);
			break;
		}
		if(logging){
			fprintf(logstream,"Waiting...\n");
		}
	}
}
void convert_client_con_to_ssl(SSL** cSSL, int sd,int_pair times){

	(*cSSL)=SSL_new(global_client_ctx);
	SSL_set_fd(*cSSL, sd);
	//Here is the SSL Accept portion.  Now all reads and writes must use SSL
	int ssl_ret=0,ssl_err=0;
	while(1) {
		ssl_ret  = SSL_connect(*cSSL);

		if (ssl_ret == 1) {
			// Handshake complete
			break;
		}

		ssl_err = SSL_get_error(*cSSL, ssl_ret);

		if (ssl_err == SSL_ERROR_WANT_READ) {
			struct timeval tv;
			tv.tv_sec=times[0];
			tv.tv_usec=times[1];
			fd_set rfds;
			FD_ZERO(&rfds);
			FD_SET(sd, &rfds);
			select(sd + 1, &rfds, (fd_set*)0, (fd_set*)0, &tv);
		}
		else if (ssl_err == SSL_ERROR_WANT_WRITE) {
			struct timeval tv;
			tv.tv_sec=times[0];
			tv.tv_usec=times[1];
			fd_set wfds;
			FD_ZERO(&wfds);
			FD_SET(sd, &wfds);
			select(sd + 1, (fd_set*)0, &wfds, (fd_set*)0, &tv);
		}
		else {
		// Real failure
			if(logging){
				fprintf(logstream, "SSL handshake failed:\n");
				ERR_print_errors_fp(logstream);
			}
			ShutdownSSL(cSSL);
			break;
		}
		if(logging){
			fprintf(logstream,"Waiting...\n");
		}
	}
}
