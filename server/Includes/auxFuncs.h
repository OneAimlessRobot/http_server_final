#ifndef AUXFUNCS_H
#define AUXFUNCS_H

char* randStr(int size);
char** randStrArr(int sizeOfStrs,int size);

void freeStrArr(char** arr,int size);
int* getRandIntArr(int min,int max,int size);

double genRanddouble(double min, double max);
int genRandInt(int min, int max);
void swap(void** var1, void** var2);
int64_t min(int64_t arg1, int64_t arg2);
char *get_file_extension(const char *path);

int makeargv(char *s, char *argv[ARGVMAX]);

void replaceStringCharacter(char *s,char dirty,char wantthisoneinstead);

int findInStringArr(char* arr[],char* nullTermStr);

int get_string_arr_size(char*args[]);

void print_string_arr(FILE* fstream,char * args[]);

void splitString(char *input, char *delimiter, char *pair[2]);

int makeargvdelim(char *s,char* delim, char *argv[ARGVMAX], int maxlen);

int make_str_arr(char *s,char*delim, char *argv[ARGVMAX],int maxlen);


int stringsAreEqual(char* s1,char* s2);

#endif
