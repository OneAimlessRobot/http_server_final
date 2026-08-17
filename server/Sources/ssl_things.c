#include "../Includes/preprocessor.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/handlecustom.h"
#include "../Includes/server_vars.h"
#include "../Includes/load_html.h"
#include "../Includes/load_rss.h"
#include "../Includes/php_proxy.h"
#include "../Includes/rcon_proxy.h"
#include "../Includes/send_resource_func.h"
#include "../Includes/server_innards.h"
#include "../Includes/ssl_things.h"
#include <openssl/provider.h>
#include <stdio.h>

static OSSL_PROVIDER *legacy_prov = NULL,
		*default_prov = NULL;

uint8_t SERVER_SSL_initted_in_process=0;

uint8_t SSL_on_in_process=0;

/*
 * 
 * versions usable with set min version:
 * 
 * SSL3_VERSION, 
 * TLS1_VERSION, 
 * TLS1_1_VERSION, 
 * TLS1_2_VERSION, 
 * TLS1_3_VERSION for TLS 
 * and DTLS1_VERSION, 
 * DTLS1_2_VERSION for DTLS.
*/

SSL_CTX *global_server_ctx=NULL;
       
static void cleanup_openssl_providers(void) {
    if (legacy_prov){
		OSSL_PROVIDER_unload(legacy_prov);
    }
    if (default_prov){
		OSSL_PROVIDER_unload(default_prov);
	}
}	
 
int verify_callback(int ok, X509_STORE_CTX *ctx) {
    if (!ok) {
        int err = X509_STORE_CTX_get_error(ctx);
		if(logging>10){
			fprintf(logstream,"Verify error: %s\n",
               X509_verify_cert_error_string(err));
        }
    }
    return ok;
}

void InitializeSSL(void){
     if(use_tls){
	if(SSL_on_in_process){
		if(logging>10){
			fprintf(logstream,"SSL já ativo! Ignorando!\n");
		}
		return;
	}
	else{
		if(logging>10){
			fprintf(logstream,"SSL ativo! SSL_on_in_process: 0 -> 1 !\n");
		}
		SSL_on_in_process=1;
	}
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
   }
}

void DestroySSL(void){
	if(use_tls){
		if(!SSL_on_in_process){
			if(logging>10){
				fprintf(logstream,"SSL já desativado! Ignorando!\n");
			}
			return;
		}
		else{
			if(logging>10){
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
			tv.tv_sec=tls_shake_timeout_secs;
			tv.tv_usec=tls_shake_timeout_usecs;
			fd_set rfds;
			FD_ZERO(&rfds);
			FD_SET(sd, &rfds);
			select(sd + 1, &rfds, (fd_set*)0, (fd_set*)0, &tv);
		}
		else if (ssl_err == SSL_ERROR_WANT_WRITE) {
			struct timeval tv;
			tv.tv_sec=tls_shake_timeout_secs;
			tv.tv_usec=tls_shake_timeout_usecs;
			fd_set wfds;
			FD_ZERO(&wfds);
			FD_SET(sd, &wfds);
			select(sd + 1, (fd_set*)0, &wfds, (fd_set*)0, &tv);
		}
		else {
			if(logging>10){
				fprintf(logstream, "SSL handshake failed:\n");
				ERR_print_errors_fp(logstream);
			}
			ShutdownSSL(cSSL);
			break;
		}
		if(logging>10){
			fprintf(logstream,"Waiting...\n");
		}
	}
}
void lower_security_func(void){

	SSL_CTX_clear_options(global_server_ctx, 
					SSL_OP_NO_SSLv3
					| SSL_OP_NO_TLSv1
					| SSL_OP_NO_TLSv1_1
					| SSL_OP_NO_TLSv1_2 
					| SSL_OP_NO_TLSv1_3
					| SSL_OP_NO_DTLSv1
					| SSL_OP_NO_DTLSv1_2);

	#if OPENSSL_VERSION_NUMBER >= 0x10101000L
		//SSL_CTX_set_min_proto_version(global_server_ctx, TLS1_1_VERSION); 
		if(SSL_CTX_set_min_proto_version(global_server_ctx, SSL3_VERSION)!=1){
			fprintf(stderr,"SSL ERROR ON SETTING PROTOCOL!\n");
			ERR_print_errors_fp(stderr);
		}
	#endif

	SSL_CTX_set_security_level(global_server_ctx, 0);	
	printf("Cypher list: %s\n",OSSL_default_cipher_list());
	if(SSL_CTX_set_cipher_list(global_server_ctx, "SSLv3:ALL:COMPLEMENTOFDEFAULT")!=1){
		fprintf(stderr,"SSL ERROR ON SETTING CYPHERS TO LOWER SECURITY!\n");
		ERR_print_errors_fp(stderr);
	}
	STACK_OF(SSL_CIPHER) *sk = SSL_CTX_get_ciphers(global_server_ctx);
	for (int i = 0; i < sk_SSL_CIPHER_num(sk); i++) {
		SSL_CIPHER *c = (SSL_CIPHER *)sk_SSL_CIPHER_value(sk, i);
		printf("Active Context Cipher [%d]: %s\n", i, SSL_CIPHER_get_name(c));
	}
}
void init_openssl_libs_server_side(void){
		if(logging>10){
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
                        if(logging>10){
                                fprintf(logstream,"SERVER SSL já ativo! Ignorando!\n");
                        }
                        return;
                }
                else{
                        if(logging>10){
                                fprintf(logstream,"SERVER SSL ativo! SERVER_SSL_initted_in_process: 0 -> 1 !\n");
                        }
                        SERVER_SSL_initted_in_process=1;
                }
                legacy_prov = OSSL_PROVIDER_load(NULL, "legacy");
                
                default_prov = OSSL_PROVIDER_load(NULL, "default");

				if (legacy_prov == NULL) {
					fprintf(stderr, "Warning: Failed to load OpenSSL 3.0 Legacy Provider. "
									"SSLv3/TLS1.0 legacy ciphers will be unavailable.\n");
					ERR_print_errors_fp(stderr);
					return;
				}
    
                if(!(global_server_ctx = SSL_CTX_new(TLS_method()))){
					fprintf(stderr, "SSL Context creation failure\n");
					ERR_print_errors_fp(stderr);
					return;	
				}
                //SSL_CTX_set_verify(global_server_ctx, SSL_VERIFY_PEER , verify_callback);
                

				SSL_CTX_set_verify(global_server_ctx, SSL_VERIFY_NONE , NULL);


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

        if(logging>10){

                fprintf(logstream,"Closing server's global ssl context?\n");
        }
        if(use_tls){
                if(!SERVER_SSL_initted_in_process){
                        if(logging>10){
                                fprintf(logstream,"SERVER SSL já desativado! Ignorando!\n");
                        }
                        return;
                }
                else{
                        if(logging>10){
                                fprintf(logstream,"SERVER SSL desativado! SERVER_SSL_initted_in_process: 1 -> 0 !\n");
                        }
                        SERVER_SSL_initted_in_process=0;
                }
                if(logging>10){

                        fprintf(logstream,"Yes!\n");
                }
                SSL_CTX_free(global_server_ctx);
                cleanup_openssl_providers();
                if(logging>10){

                        fprintf(logstream,"Server's global ssl context closed!!\n");
                }
        }
        else if(logging>10){

                fprintf(logstream,"Nooooo... (Server's global ctx is null so there is nothing to close)\n");
        }


}

