#include "../Includes/preprocessor.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/resource_consts.h"
#include "../Includes/http_req_parser.h"
#include "../Includes/handlecustom.h"
#include "../Includes/client.h"
#include "../Includes/cfg_loading.h"
#include "../Includes/server_vars.h"
#include "../Includes/send_resource_func.h"
#include "../Includes/load_logins.h"
#include "../Includes/server_innards.h"
#include "../Includes/sock_ops.h"
#include "../Includes/session_ops.h"
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#define SEND_SOCK_BUFF_SIZE 10000000

static socklen_t socklenpointer;
static int server_socket,currNumOfClients;

static struct sockaddr_in server_address, clientAddress;

static fd_set readfds;
static char addressContainer[INET_ADDRSTRLEN];

void freeClientArr(client*** arr){
	if(*arr){
	for(int i=0;i<quota;i++){
		if((*arr)[i]){
		client* c=(*arr)[i];
		free(c);
		(*arr)[i]=NULL;
		}
	}
	free(*arr);
	*arr=NULL;
	}
}

int getCurrNumOfClients(void){
	
	return currNumOfClients;


}
int getMaxNumOfClients(void){
	
	return quota;


}
void cleanup(void){
close(server_socket);
	
	if(logging){
	fprintf(logstream,"Adeus clientes...\n");
	}
	if(logging){
	fprintf(logstream,"Adeus! Server out...\n");
	}
	if(currLogins){
	
	freeLogins(&currLogins);


	}
	if(cfgs){
	
	freeCFGs(&cfgs);


	}
	printf("Adeus! Server out...\n");
	fclose(logstream);
	
}
static void sigint_handler(int arg){
	serverOn=arg=0;

}
static void handleIncommingConnections(void){
	
	FD_ZERO(&readfds);
        FD_SET(server_socket,&readfds);
	int client_socket=-1;
		struct timeval tv;
                tv.tv_sec=bigtimeoutsecs;
                tv.tv_usec=bigtimeoutusecs;
 
	int activity = select( server_socket + 1 , &readfds , NULL , NULL , &tv);
        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
            return;
	}
	else if(activity>0){

		
		if ((client_socket = accept(server_socket,(struct sockaddr*)(&clientAddress),&socklenpointer))<0)
		{
		if(logging){
                fprintf(logstream,"accept error!!!!: %s\n",strerror(errno));
        	}
		raise(SIGINT);
		
        	}
		currNumOfClients++;
		setNonBlocking(client_socket);
		getpeername(client_socket , (struct sockaddr*)&clientAddress , (socklen_t*)&socklenpointer);
                if(logging){
		fprintf(logstream,"Client connected , ip %s , port %d \n" ,inet_ntoa(clientAddress.sin_addr) , ntohs(clientAddress.sin_port));
        	}
		int pid=fork();
		switch(pid){
			case -1:
				if(logging){

					fprintf(logstream,"Erro no fork!!!\n %s\n",strerror(errno));
				}
			case 0:
				
				runClientConnection(client_socket);
	 			exit(-1);
			default:
				close(client_socket);
				if(logging){
		    		fprintf(logstream,"Adding to list of sockets as socket no: %d\n" , client_socket);
                    		}
				break;
			}
		
		}
           else{
		if(logging){
			fprintf(logstream,"No new connections\n");
		}
		
		}
	

}

static void mainLoop(void){
	while(serverOn){
	
	handleIncommingConnections();
	
	}
	if(logging){
		fprintf(logstream,"Connection thread out!\n");
	}
}

void initializeConstants(void){

	strncpy(incorrectLoginTarget,find_value_in_cfg_arr("incorrectLoginTarget",cfgs),FIELDSIZE-1);
	strncpy(defaultLoginTarget,find_value_in_cfg_arr("defaultLoginTarget",cfgs),FIELDSIZE-1);
	strncpy(doubleSessionTarget,find_value_in_cfg_arr("defaultSessionTarget",cfgs),FIELDSIZE-1);
	strncpy(defaultTarget,find_value_in_cfg_arr("defaultTarget",cfgs),FIELDSIZE-1);
	smalltimeoutsecs=atoi(find_value_in_cfg_arr("smalltimeoutsecs",cfgs));
	
	smalltimeoutusecs=atoi(find_value_in_cfg_arr("smalltimeoutusecs",cfgs));
	
	bigtimeoutsecs=atoi(find_value_in_cfg_arr("bigtimeoutsecs",cfgs));
	
	bigtimeoutusecs=atoi(find_value_in_cfg_arr("bigtimeoutusecs",cfgs));
	
	port=atoi(find_value_in_cfg_arr("port",cfgs));
	
	quota=atoi(find_value_in_cfg_arr("quota",cfgs));
	
	use_fd=atoi(find_value_in_cfg_arr("use_fd",cfgs));
	
	compression=atoi(find_value_in_cfg_arr("compression",cfgs));
	
	compression_level=atoi(find_value_in_cfg_arr("compression_level",cfgs));
	
	logging=atoi(find_value_in_cfg_arr("logging",cfgs));
	if(quota<=0){
		
		quota=500;

	}
	if(logging<0){
		
		printf("Config error: logging level must be a non negative 32 bit integer!\n");
		raise(SIGINT);
	}
	if(smalltimeoutsecs<0){
		
		printf("Config error: timeout must be a non negative 32 bit integer!\n");
		raise(SIGINT);
	}
	if(smalltimeoutusecs<0){
		
		printf("Config error: timeout must be a non negative 32 bit integer!\n");
		raise(SIGINT);
	}
	if(bigtimeoutsecs<0){
		
		printf("Config error: timeout must be a non negative 32 bit integer!\n");
		raise(SIGINT);
	}
	if(bigtimeoutusecs<0){
		
		printf("Config error: timeout must be a non negative 32 bit integer!\n");
		raise(SIGINT);
	}
	if(compression_level<0 || compression_level > 9){
		
		printf("Config error: compression level must be an integer between 0 and 9!\n");
		raise(SIGINT);
	}

}
void initializeServer(void){

	signal(SIGINT,sigint_handler);
	
	memset(currDir,0,PATHSIZE);
	getcwd(currDir,PATHSIZE);

	loadCfg();
	initializeConstants();

	logstream=stdout;
	serverOn=1;
	currNumOfClients=0;
	server_socket= socket(AF_INET,SOCK_STREAM,0);
        int ptr=1;
        setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,(char*)&ptr,sizeof(ptr));
        if(server_socket==-1){
                raise(SIGINT);

        }

	

        server_address.sin_family=AF_INET;
        server_address.sin_port= htons(port);
        server_address.sin_addr.s_addr = INADDR_ANY;
        socklen_t socklength=sizeof(server_address);
        
	bind(server_socket,(struct sockaddr*)(&server_address),socklength);

        listen(server_socket,MAX_CLIENTS_HARD_LIMIT);
	
        struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&server_address;
        struct in_addr ipAddr = pV4Addr->sin_addr;
        
	inet_ntop( AF_INET, &ipAddr, addressContainer, INET_ADDRSTRLEN );
	if(logging){
	fprintf(logstream,"Server spawnado @ %s\n",inet_ntoa(server_address.sin_addr));
	}
	socklenpointer=sizeof(clientAddress);
	loadLogins();
	servComp=gzip;
	mainLoop();
	cleanup();
}

