#ifndef AUXFUNCS_H
#define AUXFUNCS_H


char* randStr(int size);
char** randStrArr(int sizeOfStrs,int size);
/*
 
 https://stackoverflow.com/questions/5820810/case-insensitive-string-comparison-in-c
*/
int strcicmp(char const *a, char const *b);

void freeStrArr(char** arr,int size);
int* getRandIntArr(int min,int max,int size);

int does_dir_exist_aux(char* path);

double genRanddouble(double min, double max);
int genRandInt(int min, int max);
void swap(void** var1, void** var2);


char *get_file_extension(const char *path);

void replaceStringCharacter(char *s,char dirty,char wantthisoneinstead);

int findInStringArr(char* arr[],char* nullTermStr);

int get_string_arr_size(char*args[]);

void print_string_arr(FILE* fstream,char * args[]);

void splitString(char *input, char *delimiter, char *pair[2]);

void generic_error_print(char* prefix_string);

int makeargvdelim(char *s,char* delim, char *argv[ARGVMAX], int maxlen);

int make_str_arr(char *s,char*delim, char *argv[ARGVMAX],int maxlen);

int build_arg_line_arr(char* the_separated_parameters,arg_line arg_arr[],uint64_t max_len);

void super_mkdir(const char *dir);

int stringsAreEqual(char* s1,char* s2);
char from_hex(char c);
int url_decode(char *src, char *dst, char** reserved_characters_arr);
int stringsAreEqualN(char* s1,char*s2,int n);

int findInStringArrN(char* arr[],char* nullTermStr,int n);
void skip_comments(FILE* fp);
#endif
