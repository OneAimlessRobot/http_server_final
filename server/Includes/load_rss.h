#ifndef LOAD_RSS_H
#define LOAD_RSS_H

int isRssFeed(char* resourceTarget);

char* generateRss(char* dir);

void deleteRssXML(void);


#endif
