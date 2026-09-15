#ifndef LOAD_DATE_H
#define LOAD_DATE_H

#define MICROTIMESIZESTRINGTHING 50

extern char LE_TIMEUREUR_DATE[MICROTIMESIZESTRINGTHING],
	LE_TIMEUREUR_LAST_MODIFIED_DATE[MICROTIMESIZESTRINGTHING],
	LE_TIMEUREUR_LAST_ACCESS_DATE[MICROTIMESIZESTRINGTHING],
	ETag[ETAG_SIZE];

extern timer_struct_t struct_for_date,
				struct_for_mod_date,
				struct_for_access_date;
				
int isDateReq(char* target);

int build_file_stat(char* full_file_path, char* modified_date_buff, struct stat* the_stat);

void build_date_buff(void);

void build_last_modified_buff(char* full_file_path);

void the_time_func(char* unused);

#endif
