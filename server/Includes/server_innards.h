#ifndef SERVER_INNARDS_H
#define SERVER_INNARDS_H
#define FIELDLENGTH 1024

void the_exit_func(void);

void clientBootstrap(void);

void initializeServer(void);

int getMaxNumOfClients(void);

int getCurrNumOfClients(void);

void cleanup(void);
#endif

