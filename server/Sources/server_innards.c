#include "../Includes/preprocessor.h"
#include "../Includes/filePaths.h"
#include "../Includes/client.h"
#include "../Includes/cfg_loading.h"
#include "../Includes/server_vars.h"
#include "../Includes/server_innards.h"
#include "../Includes/sock_ops.h"
#include "../Includes/io_ops.h"

#define ARGV_TO_USE (exec_argv_arr[use_valgrind])

u_int64_t request_id_here =0;
struct sigaction sa;


static socklen_t socklenpointer;
static int server_socket = -1;
static char buff_sock[32]={0},
	buff_tls[32]={0},
	buff_suppressions_file[FIELDSIZE*2]={0},
	buff_request_id[128]={0};

static char* super_valgrind_argv[] = {"valgrind",
					"--track-origins=yes",
					"--leak-check=full",
					"--show-leak-kinds=all",
					"--track-fds=yes",
					/*buff_suppressions_file,*/
					/*"--show-reachable=no",*/
					"./server.exe","1",
					buff_tls,
					buff_sock,
					buff_request_id,
					NULL},
	
			* valgrind_argv[] = {"valgrind",
					"--leak-check=full",
					"--show-leak-kinds=all",
					"--track-fds=yes",
					/*"--show-reachable=no",*/
					/*buff_suppressions_file,*/
					"./server.exe",
					"1",
					buff_tls,
					buff_sock,
					buff_request_id,
					NULL},
					
			* normal_argv[] = {"./server.exe", 
					"1",
					buff_tls,
					buff_sock,
					buff_request_id,
					NULL};
					

argv_type exec_argv_arr[3] = { normal_argv,
					valgrind_argv,
					super_valgrind_argv};
					



static struct sockaddr_in server_address, clientAddress;

static fd_set readfds;


void cleanup(void){
	if(serverWasOn && (server_socket>=0)){
		close(server_socket);
		
	}
	fprintf(stdout,"Adeus! Process out.\n");
	if(logstream!=stdout && logstream!= stderr){
		fclose(logstream);
	}
	
	if(cfgs){
	
		freeCFGs(&cfgs);
		cfgs = NULL;


	}
	if(serverWasOn){
		serverWasOn=0;
	}
}
static void sigact_sigint_handler(int arg){
	serverOn+=(arg*0);

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
		serverOn=0;
		return;
	}
	else if(activity>0){

		
		if ((client_socket = accept(server_socket,(struct sockaddr*)(&clientAddress),&socklenpointer))<0)
		{
		if(logging){
                fprintf(logstream,"accept error!!!!: %s\n",strerror(errno));
        	}
			serverOn=0;
			return;
		}
        request_id_here++;
		setNonBlocking(client_socket);
		getpeername(client_socket , (struct sockaddr*)&clientAddress , (socklen_t*)&socklenpointer);
        if(logging){
			fprintf(logstream,"Client connected , ip %s , port %hu\nRequest number: %lu\n" ,inet_ntoa(clientAddress.sin_addr) , ntohs(clientAddress.sin_port),request_id_here);
        }
        
        memset(buff_sock,0, sizeof(buff_sock));
        memset(buff_tls,0, sizeof(buff_tls));
        memset(buff_request_id,0, sizeof(buff_request_id));
        
		snprintf(buff_sock,sizeof(buff_sock)-1,"%d",client_socket);
		snprintf(buff_tls,sizeof(buff_tls)-1,"%hhu",want_tls);
		snprintf(buff_request_id,sizeof(buff_request_id)-1,"%lx",request_id_here);
		
		int pid=fork();
		switch(pid){
			case -1:
				if(logging){

					fprintf(logstream,"Erro no fork!!!\n %s\n",strerror(errno));
				}
				exit(-1);
			case 0:
				close(server_socket);
				execvp(ARGV_TO_USE[0],ARGV_TO_USE);
				perror("execvp");
				break;
			default:
				close(client_socket);
				break;
			}
		
		}
	

}

static void mainLoop(void){
	while(serverOn){
	
		handleIncommingConnections();
	
	}
}

void initializeCommonConstants(void){
	
	port=ato_ull(find_value_in_cfg_arr("port",cfgs));
	
	want_tls=ato_ull(find_value_in_cfg_arr("want_tls",cfgs));
	
	bigtimeoutsecs=ato_ull(find_value_in_cfg_arr("bigtimeoutsecs",cfgs));
	
	bigtimeoutusecs=ato_ull(find_value_in_cfg_arr("bigtimeoutusecs",cfgs));
	
	logging=ato_ull(find_value_in_cfg_arr("logging",cfgs));
	
	tmp_file_dir = getenv("tmp_file_dir");
	if(!tmp_file_dir){
		printf("the value from the tmp_file_dir environment variable could not be obtained!\n"
				"Please execute the server\n"
				"Using \"server_start.sh\"\n"
				"exiting...\n");
		cleanup();
		
	}
}
void initializeClientConstants(void){

	strncpy(defaultTarget,find_value_in_cfg_arr("defaultTarget",cfgs),FIELDSIZE-1);
	strncpy(notFoundDefaultTarget,find_value_in_cfg_arr("notFoundDefaultTarget",cfgs),FIELDSIZE-1);
	strncpy(internalServerErrorTarget,find_value_in_cfg_arr("internalServerErrorTarget",cfgs),FIELDSIZE-1);
	strncpy(mainRssFeed,find_value_in_cfg_arr("mainRssFeed",cfgs),FIELDSIZE-1);
	strncpy(host_cert_file_path,find_value_in_cfg_arr("host_cert_file_path",cfgs),FIELDSIZE-1);
	strncpy(host_pkey_file_path,find_value_in_cfg_arr("host_pkey_file_path",cfgs),FIELDSIZE-1);
	strncpy(robotsTarget,find_value_in_cfg_arr("robotsTarget",cfgs),FIELDSIZE-1);
	strncpy(excludePrettifyTarget,find_value_in_cfg_arr("excludePrettifyTarget",cfgs),FIELDSIZE-1);
	int invalid_string_confs=
				!((stringsAreEqual(defaultTarget,NO_SUCH_SETTING)
				||
				stringsAreEqual(defaultTarget,NO_SETTINGS_LOADED))
				&&
				(stringsAreEqual(notFoundDefaultTarget,NO_SUCH_SETTING)
				||
				stringsAreEqual(notFoundDefaultTarget,NO_SETTINGS_LOADED))
				&&
				(stringsAreEqual(mainRssFeed,NO_SUCH_SETTING)
				||
				stringsAreEqual(mainRssFeed,NO_SETTINGS_LOADED))
				&&
				(stringsAreEqual(host_cert_file_path,NO_SUCH_SETTING)
				||
				stringsAreEqual(host_cert_file_path,NO_SETTINGS_LOADED))
				&&
				(stringsAreEqual(host_pkey_file_path,NO_SUCH_SETTING)
				||
				stringsAreEqual(host_pkey_file_path,NO_SETTINGS_LOADED))
				&&
				(stringsAreEqual(excludePrettifyTarget,NO_SUCH_SETTING)
				||
				stringsAreEqual(excludePrettifyTarget,NO_SETTINGS_LOADED))
				&&
				(stringsAreEqual(internalServerErrorTarget,NO_SUCH_SETTING)
				||
				stringsAreEqual(internalServerErrorTarget,NO_SETTINGS_LOADED)));
	
	if(!invalid_string_confs){

		printf("Config error: invalid string-type field set!!!!\n");
		cleanup();

	}
	recv_req_buff_size=ato_ull(find_value_in_cfg_arr("recv_req_buff_size",cfgs));
	
	keep_alive=ato_ull(find_value_in_cfg_arr("keep_alive",cfgs));
	
	s_maxage=ato_ull(find_value_in_cfg_arr("s_maxage",cfgs));
	
	smalltimeoutsecs=ato_ull(find_value_in_cfg_arr("smalltimeoutsecs",cfgs));
	
	smalltimeoutusecs=ato_ull(find_value_in_cfg_arr("smalltimeoutusecs",cfgs));
	
	tls_shake_timeout_usecs=ato_ull(find_value_in_cfg_arr("tls_shake_timeout_usecs",cfgs));
	
	tls_shake_timeout_secs=ato_ull(find_value_in_cfg_arr("tls_shake_timeout_secs",cfgs));
	
	tls_data_timeout_usecs=ato_ull(find_value_in_cfg_arr("tls_data_timeout_usecs",cfgs));
	
	tls_data_timeout_secs=ato_ull(find_value_in_cfg_arr("tls_data_timeout_secs",cfgs));
	
	
	lower_security_for_compat=ato_ull(find_value_in_cfg_arr("lower_security_for_compat",cfgs));
	
	use_compression=ato_ull(find_value_in_cfg_arr("use_compression",cfgs));
	
	compression_level=ato_ull(find_value_in_cfg_arr("compression_level",cfgs));
	
	use_chunked=ato_ull(find_value_in_cfg_arr("use_chunked",cfgs));
	
	ignore_partial=ato_ull(find_value_in_cfg_arr("ignore_partial",cfgs));
	
	
	use_fancy_find=ato_ull(find_value_in_cfg_arr("use_fancy_find",cfgs));
	
	if(compression_level > 9){
		printf("Config error: compression level must be an integer between 0 and 9!\n");
		cleanup();
	}
	initializeCommonConstants();
}
void initializeServerConstants(void){
	
	strncpy(valgrindSuppressionsFile,find_value_in_cfg_arr("valgrindSuppressionsFile",cfgs),FIELDSIZE-1);
	strncpy(bind_iface_addr,find_value_in_cfg_arr("bind_iface_addr",cfgs),FIELDSIZE-1);
	
	int invalid_string_confs=
				!((stringsAreEqual(valgrindSuppressionsFile,NO_SUCH_SETTING)
				||
				stringsAreEqual(valgrindSuppressionsFile,NO_SETTINGS_LOADED))
				&&
				(stringsAreEqual(bind_iface_addr,NO_SUCH_SETTING)
				||
				stringsAreEqual(bind_iface_addr,NO_SETTINGS_LOADED)));
				
	if(!invalid_string_confs){

		printf("Config error: invalid string-type field set!!!!\n");
		cleanup();

	}
	
	snprintf(buff_suppressions_file, sizeof(buff_suppressions_file), 
			"--suppressions=./%s",
			valgrindSuppressionsFile);
	
	
	use_valgrind=ato_ull(find_value_in_cfg_arr("use_valgrind",cfgs));
	
	if(use_valgrind > 2){
		printf("Config error: use_valgrind must be an integer between 0 and 2!\n");
		cleanup();
	}
	
	initializeCommonConstants();
}
void the_exit_func(void){
	serverOn=0;
	cleanup();
}
void clientBootstrap(void){
	
	memset(currDir,0,PATHSIZE);
	getcwd(currDir,PATHSIZE);
	logstream=stdout;
	loadCfg();
	initializeClientConstants();
	socklenpointer=sizeof(clientAddress);
	if(logging){
		fprintf(logstream,"Request id: %lu\n",request_id);
	}
	cleanup();
	
}
void initializeServer(void){

	sa.sa_handler=sigact_sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags=SA_RESTART|SA_NOCLDWAIT;
	
	exit_func_for_this_module = the_exit_func;
	sigaction(SIGCHLD, &sa, NULL);
	signal(SIGINT,sigint_handler);
	memset(currDir,0,PATHSIZE);
	getcwd(currDir,PATHSIZE);
	logstream=stdout;
	loadCfg();
	initializeServerConstants();
	if(cfgs){
		print_loaded_cfgs(logstream);
	}
	else{
		exit(-1);
	}
	serverWasOn=serverOn=1;
	server_socket= socket(AF_INET,SOCK_STREAM,0);
	
	if(server_socket<0){
		cleanup();
		generic_error_print("Server listen socket error!");
		return;
	}
	int ptr=1;
	setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,(char*)&ptr,sizeof(ptr));
	



	server_address.sin_family=AF_INET;
	server_address.sin_port= htons(port);
	int strlen_bind_addr= strnlen(bind_iface_addr,sizeof(bind_iface_addr));
	server_address.sin_addr.s_addr = strlen_bind_addr >0?inet_addr(bind_iface_addr):htonl(INADDR_ANY);
	socklen_t socklength=sizeof(server_address);
        
	int bind_result = bind(server_socket,(struct sockaddr*)(&server_address),socklength);
	if(bind_result<0){
		cleanup();
		generic_error_print("Bind error!");
		return;
	}
	listen(server_socket,MAX_CLIENTS_HARD_LIMIT);

	if(logging){
		fprintf(logstream,"Server spawnado @ %s\n",inet_ntoa(server_address.sin_addr));
	}
	socklenpointer=sizeof(server_address);

	if(!does_dir_exist_aux(tmp_file_dir)){
		mkdir(tmp_file_dir, 0777);
		
	}
	mainLoop();
	cleanup();
}

