#include "../Includes/preprocessor.h"


int url_decode(char *src, char *dst, char** reserved_characters_arr) {
    int result=0;
    int counter_src=0;
    int counter_dst=0;
    while (strnlen(&src[counter_src],PATHSIZE-1)) {
	if (src[counter_src] == '%' && isxdigit(src[counter_src+1]) && isxdigit(src[counter_src+2])) {
        
	    sscanf(&src[counter_src],"%%%2hhx",(unsigned char*)&dst[counter_dst]);
	    if(dst[counter_dst]>126){
			
		dst[counter_dst] = '\0';
	    	return 0;
	    }
	    counter_dst+=1;
	    counter_src += 3;
        }
	else if(findInStringArrN(reserved_characters_arr,&src[counter_src],1)>=0){
		
		dst[counter_dst] = '\0';
    		return 0;
	}
	 else {
		 
	    dst[counter_dst] = src[counter_src];
	    counter_dst++;
	    counter_src++;
	}
    }
    dst[counter_dst] = '\0';
    result=1;
    return result;
}

int does_dir_exist_aux(char* path){
	
	DIR* directory=opendir(path);
	if(directory){
		closedir(directory);
	}
	errno=0;
	return (directory!=NULL);
}
int strcicmp(char const *a, char const *b)
{
    for (;; a++, b++) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a){
            return d;
        }
    }
}
void super_mkdir(const char *dir) {
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);
    if (tmp[len - 1] == '/'){
        tmp[len - 1] = 0;
    }
    for (p = tmp + 1; *p; p++){
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    }
    mkdir(tmp, S_IRWXU);
}


double genRanddouble(double min, double max){

    if (max < min) {
        fprintf(stderr, "Error: max must be greater than or equal to min\n");
        return 0.0f;
    }

    double random = ((double)rand() / RAND_MAX);

    return (random * (max - min)) + min;

}

int genRandInt(int min, int max) {
    if (max < min) {
        fprintf(stderr, "Error: max must be greater than or equal to min\n");
        return 0;
        }

    return (rand() % (max - min + 1)) + min;
}


char* randStr(int size){

        char* result= malloc(size+1);
        memset(result,0,size+1);
        for(int i=0;i<size;i++){

                result[i]=(char)genRandInt((int)97,(int)112);

        }
	result[size]=0;
        return result;



}
char** randStrArr(int sizeOfStrs,int size){

	char** result= malloc(sizeof(char*)*size);

	for(int i=0;i<size;i++){

		result[i]=randStr(sizeOfStrs);

	}

	return result;


}

void freeStrArr(char** arr,int size){
	if(!arr){

		return;

	}
	for(int i=0;i<size;i++){
		if(arr[i]){
		free(arr[i]);
		}
	}

	free(arr);


}

int* getRandIntArr(int min,int max,int size){

	int* result=malloc(sizeof(int)*size);
	for(int i=0;i<size;i++){

		result[i]=genRandInt(min,max);
	}
	return result;

}

void swap(void** var1, void** var2){

void* tmp=*var1;
*var1=*var2;
*var2=tmp;




}


char *get_file_extension(const char *path) {
    char *extension = strrchr(path, '.');

    if (extension) {

        return extension + 1;
    }

    return NULL;
}

void generic_error_print(char* prefix_string){
	
	fprintf(stderr,"Error!\nError description: %s\nError string: %s\n",
															prefix_string?prefix_string:"No string given",
															strerror(errno));

}

int makeargv(char *s, char *argv[ARGVMAX], int n_max) {
	int use_nmax= (n_max>0);
	int ntokens = 0;

    if (s == NULL || argv == NULL || ARGVMAX == 0){
        return -1;
    }
    argv[ntokens] = strtok(s, " \t\n");
    while ((argv[ntokens] != NULL) && (ntokens < ARGVMAX)) {
        
		if(use_nmax && (ntokens >=  (n_max-1))){
			break;
		}
		ntokens++;
        argv[ntokens] = strtok(NULL, " \t\n");
    }
    if(use_nmax && (ntokens < n_max)){
		ntokens++;
		char* pair[2];
        argv[ntokens] = strtok(NULL, "\n");
		splitString(argv[ntokens], " -> ", pair);
		argv[ntokens]= pair[0];
	}
	ntokens++;
    argv[ntokens] = NULL;
    return ntokens;
}

int findInStringArr(char* arr[],char* nullTermStr){


	for (int i=0;arr[i];i++){
		if(stringsAreEqual(nullTermStr,arr[i])){
			return i;
		}
	}
	return -1;

}

int findInStringArrN(char* arr[],char* nullTermStr,int n){


	for (int i=0;arr[i];i++){
		if(stringsAreEqualN(nullTermStr,arr[i],n)){

			return i;
		}
	}
	return -1;

}

int get_string_arr_size(char*args[]){

	int i;
	for (i=0;args[i];i++);
	return i;

}
void print_string_arr(FILE* fstream,char * args[]){

	for (int i=0;args[i];i++){
		fprintf(fstream,"%d: <%s>\n",i,args[i]);

	}

}


void splitString(char *input, char *delimiter, char *pair[2]) {
    char *delimiterPos = strstr(input, delimiter);

    if (delimiterPos) {
        size_t firstPartLength = delimiterPos - input;

        pair[0] = input;

		pair[0][firstPartLength] = '\0';

        pair[1] = delimiterPos+strlen(delimiter);
	} else {
        pair[0] = input;
        pair[1] = NULL;
    }
}

int make_str_arr(char *s,char* delim, char *argv[ARGVMAX],int maxlen) {
    int nheaders = 0;
    int lentoadvance=0;
    if (s == NULL || argv == NULL || maxlen<2 || maxlen>ARGVMAX ||ARGVMAX == 0){
        return -1;
    }
    char* nextHeader = strstr(s, delim);
    if(!nextHeader){
	argv[0]=s;
	argv[1]=NULL;
	return 1;
    }
    nextHeader[0]=0;
    argv[nheaders]=s;
    lentoadvance=strlen(argv[nheaders])+strlen(delim);
    s+=lentoadvance;
    nheaders++;
    while ((nextHeader != NULL) && (nheaders < maxlen)) {
	nextHeader = strstr(s, delim);
    	if(nextHeader){
	nextHeader[0]=0;
    	argv[nheaders]=s;
    	lentoadvance=strlen(argv[nheaders])+strlen(delim);
    	s+=lentoadvance;
    	nheaders++;
	}
	else {
	argv[nheaders]=s;
	nheaders++;
	break;
    	}
    }
    nheaders++;
    argv[nheaders] = NULL;
    return nheaders;
}

int makeargvdelim(char *s,char* delim, char *argv[ARGVMAX],int maxlen) {
    int ntokens = 0;

    if (s == NULL || argv == NULL || ARGVMAX == 0|| maxlen<0 || maxlen >ARGVMAX)
        return -1;
    argv[ntokens] = strtok(s, delim);
    while ((argv[ntokens] != NULL) && (ntokens < (maxlen))) {
        ntokens++;
        argv[ntokens] = strtok(NULL, delim);
    }
    argv[ntokens] = NULL;
    return ntokens;
}

void replaceStringCharacter(char *s,char dirty,char wantthisoneinstead){

	int size=strlen(s);
	for(int i=0;i<size;i++){
		if(s[i]==dirty){
			s[i]=wantthisoneinstead;

		}
	}



}
int stringsAreEqual(char* s1,char*s2){

	int lengthIsTheSame= (strlen(s1)==strlen(s2));
	int stringsAreTheSame=!(strcmp(s1,s2));
	return lengthIsTheSame && stringsAreTheSame;

}
int stringsAreEqualN(char* s1,char*s2,int n){

	int lengthIsTheSame= (strnlen(s1,n)==strnlen(s2,n));
	int stringsAreTheSame=!(strncmp(s1,s2,n));
	return lengthIsTheSame && stringsAreTheSame;

}
int stringsAreEqualTheFinalString(char* s1,char*s2,int n){

	int lengthIsTheSame= (strnlen(s1,n)==strnlen(s2,n));
	int stringsAreTheSame=!(strncmp(s1,s2,n));
	return lengthIsTheSame && stringsAreTheSame;

}


void skip_comments(FILE* fp){

	int curr_char=0;

	skip_comment:
	while(isspace((curr_char=fgetc(fp)))&&curr_char!=EOF);
	ungetc(curr_char,fp);

	if(curr_char==';'){
	
	 if((curr_char=fgetc(fp))!=EOF){
	         ungetc(curr_char,fp);
	         while(((curr_char=fgetc(fp))!='\n')&&curr_char!=EOF);
	         ungetc(curr_char,fp);
	 }

	}

	while(isspace((curr_char=fgetc(fp)))&&curr_char!=EOF);
	ungetc(curr_char,fp);
	if(curr_char==';'){
	 goto skip_comment;
	}
}

