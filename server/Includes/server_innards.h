#ifndef SERVER_INNARDS_H
#define SERVER_INNARDS_H
#define FIELDLENGTH 1024


void initializeServer(void);

int getMaxNumOfClients(void);

int getCurrNumOfClients(void);

void cleanup(void);
#endif

