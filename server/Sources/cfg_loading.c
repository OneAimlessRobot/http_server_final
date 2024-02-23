#include "../Includes/preprocessor.h"
#include "../Includes/buffSizes.h"
#include "../Includes/auxFuncs.h"
#include "../Includes/cfg_loading.h"
#include "../Includes/server_vars.h"
#include <sys/socket.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>



cfg_field** cfgs=NULL;
static void print_cfg_arr(FILE* fstream,cfg_field** cfgs){

	for (int i=0;cfgs[i];i++){
		
		fprintf(fstream,"CFG no. %d:\nCFG name: %s\nCFG value: %s\n",i,cfgs[i]->field[0],cfgs[i]->field[1]);

	}

}
char* find_value_in_cfg_arr(char* setting,cfg_field** cfgs){
	if(cfgs){
	for (int i=0;cfgs[i];i++){
		
		if(stringsAreEqual(cfgs[i]->field[0],setting)){
			return cfgs[i]->field[1];
		}
	}
	return "NO_SUCH_SETTING";
	}
	return "NO_SETTINGS_LOADED";

}

void loadCfg(void)
{
	int numOfCfgs=0;
	FILE* CFGStream;
	char path[PATHSIZE*2]={0};
	snprintf(path,PATHSIZE*2,"%s%s",currDir,CFG_DIR);
	if(!(CFGStream=fopen(path,"r"))){
		
		if(logging){

			printf("CVARS NAO CARREGADOS!!!!!\nPATH INVALIDA!!!!\nPath: %s\nErro: %s\n",path,strerror(errno));
		}
		return;
	}
	char* buff=malloc(FIELDSIZE+1);
	memset(buff,0,FIELDSIZE+1);
	while(fgets(buff,FIELDSIZE,CFGStream)){
	
		numOfCfgs++;
	
	}
	if(!numOfCfgs){
		fclose(CFGStream);
		return;
	}
	memset(buff,0,FIELDSIZE+1);
	rewind(CFGStream);
	cfgs=malloc(sizeof(cfg_field*)*(numOfCfgs+1));
	int i=0;
	while(fgets(buff,FIELDSIZE,CFGStream)){
		
		cfgs[i]=malloc(sizeof(cfg_field));
		cfgs[i]->mem=strdup(buff);
		splitString(cfgs[i]->mem,"=",cfgs[i]->field);
		int valueLength=strlen(cfgs[i]->field[1]);
		cfgs[i]->field[1][valueLength-1]=0;
		memset(buff,0,FIELDSIZE+1);
		i++;
		

	}
	cfgs[i]=NULL;
		printf("\nCFGs carregadas:\n");
		print_cfg_arr(stdout,cfgs);
	fclose(CFGStream);
	free(buff);
	return;


}

void freeCFGs(cfg_field*** cfgs)
{
	if(*cfgs){
		
	for (int i=0;(*cfgs)[i];i++){
		
		free((*cfgs)[i]->mem);
		free((*cfgs)[i]);
	}
	free((*cfgs));
	*cfgs=NULL;
	}



}


