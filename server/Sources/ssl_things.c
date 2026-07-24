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



SSL_CTX *global_server_ctx=NULL;
        
int verify_callback(int ok, X509_STORE_CTX *ctx) {
    if (!ok) {
        int err = X509_STORE_CTX_get_error(ctx);
        printf("Verify error: %s\n",
               X509_verify_cert_error_string(err));
    }
    return ok;
}

void InitializeSSL(void){
     if(use_tls){
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
	if(use_tls){
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
void convert_server_con_to_ssl(SSL** cSSL, int sd){

	(*cSSL)=SSL_new(global_server_ctx);
	SSL_set_fd(*cSSL, sd);
	int ssl_ret=0,ssl_err=0;
	while(1) {
		ssl_ret  = SSL_accept(*cSSL);

		if (ssl_ret == 1) {
			break;
		}

		ssl_err = SSL_get_error(*cSSL, ssl_ret);

		if (ssl_err == SSL_ERROR_WANT_READ) {
			struct timeval tv;
			tv.tv_sec=bigtimeoutsecs;
			tv.tv_usec=bigtimeoutusecs;
			fd_set rfds;
			FD_ZERO(&rfds);
			FD_SET(sd, &rfds);
			select(sd + 1, &rfds, (fd_set*)0, (fd_set*)0, &tv);
		}
		else if (ssl_err == SSL_ERROR_WANT_WRITE) {
			struct timeval tv;
			tv.tv_sec=bigtimeoutsecs;
			tv.tv_usec=bigtimeoutusecs;
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
void init_openssl_libs_server_side(void){
        if(logging){
                if(use_tls){

                        fprintf(logstream,"Server's SSL Ativado? inicializando Server's SSL!...\n");
                }
                else{

                        fprintf(logstream,"Server's SSL não ativado. Returning...\n");
                        return;
                }
        }

        if(use_tls){
                if(SERVER_SSL_initted_in_process){
                        if(logging){
                                fprintf(logstream,"SERVER SSL já ativo! Ignorando!\n");
                        }
                        return;
                }
                else{
                        if(logging){
                                fprintf(logstream,"SERVER SSL ativo! SERVER_SSL_initted_in_process: 0 -> 1 !\n");
                        }
                        SERVER_SSL_initted_in_process=1;
                }
                global_server_ctx = SSL_CTX_new(TLS_server_method());
                SSL_CTX_set_verify(global_server_ctx, SSL_VERIFY_PEER , verify_callback);


                /* Load client certificate */
                if (!SSL_CTX_use_certificate_chain_file(global_server_ctx, host_cert_file_path)) {
                    ERR_print_errors_fp(stderr);
                }

                /* Load client private key */
                if (!SSL_CTX_use_PrivateKey_file(global_server_ctx, host_pkey_file_path, SSL_FILETYPE_PEM)) {
                    ERR_print_errors_fp(stderr);
                }

                /* Make sure key matches cert */
                if (!SSL_CTX_check_private_key(global_server_ctx)) {
                    fprintf(stderr, "Client key does not match certificate\n");
                }
        }

}


void end_openssl_libs_server_side(void){

        if(logging){

                fprintf(logstream,"Closing server's global ssl context?\n");
        }
        if(use_tls){
                if(!SERVER_SSL_initted_in_process){
                        if(logging){
                                fprintf(logstream,"SERVER SSL já desativado! Ignorando!\n");
                        }
                        return;
                }
                else{
                        if(logging){
                                fprintf(logstream,"SERVER SSL desativado! SERVER_SSL_initted_in_process: 1 -> 0 !\n");
                        }
                        SERVER_SSL_initted_in_process=0;
                }
                if(logging){

                        fprintf(logstream,"Yes!\n");
                }
                SSL_CTX_free(global_server_ctx);
                if(logging){

                        fprintf(logstream,"Server's global ssl context closed!!\n");
                }
        }
        else if(logging){

                fprintf(logstream,"Nooooo... (Server's global ctx is null so there is nothing to close)\n");
        }


}

