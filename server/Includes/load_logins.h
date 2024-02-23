#ifndef LOAD_LOGINS_H
#define LOAD_LOGINS_H
typedef struct login{
	
	char* mem;
	char* fields[2];

}login;

extern login** currLogins;
extern login** currAdmins;

char* find_login_pw_in_login_arr(char* user_name,login** logins);

void loadLogins(void);
void loadAdmins(void);

void freeLogins(login*** logins);
#endif
