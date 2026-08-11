#include "Includes/preprocessor.h"
#include "Includes/auxFuncs.h"
#include "Includes/client.h"
#include "Includes/handlecustom.h"
#include "Includes/http_req_parser.h"
#include "Includes/server_innards.h"

int main(int argc, char** argv){
	
	int init_mode=0,
	sock_num=0,
	is_tls=0;
	switch(argc){
		case 1:
			initializeServer();
			break;
		case 2:
			initializeServer();
			break;
		case 3:
			initializeServer();
			break;
		default:
			init_mode=atoi(argv[1]);
			switch(init_mode){
					case 1:
						is_tls=atoi(argv[2]);
						sock_num=atoi(argv[3]);
						clientBootstrap();
						runClientConnection(is_tls,sock_num);
					break;
					default:
						initializeServer();
					break;
						
			}
			break;
	}
	return 0;
}
