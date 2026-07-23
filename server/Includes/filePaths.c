

#define PATHSIZE 1024

char auth_cert_file_path[PATHSIZE]={0};
char host_cert_file_path[PATHSIZE]={0};
char host_pkey_file_path[PATHSIZE]={0};

SSL_CTX *global_server_ctx=NULL,
        *global_client_ctx=NULL;
