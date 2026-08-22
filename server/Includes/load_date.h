#ifndef LOAD_DATE_H
#define LOAD_DATE_H

#define DATE_COMMAND "LC_TIME=C date \"+%a, %d %b %Y %H:%M:%S %Z\""

#define MICROTIMESIZESTRINGTHING 50

extern char LE_TIMEUREUR[MICROTIMESIZESTRINGTHING];

int isDateReq(char* target);

void the_time_func(char* unused);

#endif