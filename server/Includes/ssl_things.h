#ifndef SSL_THINGS_H
#define SSL_THINGS_H

extern uint8_t SERVER_SSL_initted_in_process;

extern uint8_t SSL_on_in_process;

extern uint8_t CLIENT_SSL_initted_in_process;

void InitializeSSL(void);

void DestroySSL(void);

void ShutdownSSL(SSL** cSSL);

void convert_server_con_to_ssl(SSL** cSSL, int sd,int_pair times);
void convert_client_con_to_ssl(SSL** cSSL, int sd,int_pair times);
void init_openssl_libs_client_side(void);

void init_openssl_libs_server_side(void);

void end_openssl_libs_client_side(void);

void end_openssl_libs_server_side(void);

extern SSL_CTX *global_server_ctx,
                *global_client_ctx;


#endif
