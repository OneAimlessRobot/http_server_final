#include "../Includes/buffSizes.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <pthread.h>
#include "../Includes/client.h"
#include "../Includes/cfg_loading.h"
#include "../Includes/server_vars.h"

int logging;

int smalltimeoutsecs,smalltimeoutusecs;

int bigtimeoutsecs,bigtimeoutusecs;

int port;

int quota;

int recv_socket_size;

int send_socket_size;

int session_time_usecs;

int compression;

int compression_level;

int use_fd;

int use_chunked;
char currDir[PATHSIZE];
int serverOn=0;
FILE* logstream=NULL;
compMethod br = {"brotli","br"," br"},gzip = {"gzip","gzip"," gzip"},
deflateM = {"deflate","deflate"," deflate"};
compMethod servComp;


char defaultTarget[FIELDSIZE]={0};
char defaultLoginTarget[FIELDSIZE]={0};
char incorrectLoginTarget[FIELDSIZE]={0};
char doubleSessionTarget[FIELDSIZE]={0};
char timeoutTarget[FIELDSIZE]={0};

char* defaultVersion="HTTP/1.1";
char* defaultMimetype="*/*";

