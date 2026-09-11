#ifndef LOAD_DATE_H
#define LOAD_DATE_H

#define MICROTIMESIZESTRINGTHING 50

extern char LE_TIMEUREUR_DATE[MICROTIMESIZESTRINGTHING],
	LE_TIMEUREUR_LAST_MODIFIED_DATE[MICROTIMESIZESTRINGTHING],
	ETag[MICROTIMESIZESTRINGTHING];
	
int isDateReq(char* target);

void build_date_buff(void);

void build_last_modified_buff(char* full_file_path);

void the_time_func(char* unused);

#endif
