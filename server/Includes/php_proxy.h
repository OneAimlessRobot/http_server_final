#ifndef PHP_PROXY_H
#define PHP_PROXY_H


int isPHPRequest(char* resourceTarget);

char* generatePHPResult(char* dir);

void deletePHPResult(void);


#endif
