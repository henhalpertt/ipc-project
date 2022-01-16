#ifndef __MULTI_H__
#define __MULTI_H__
#define OK 0
#define ERROR 1

int MultiCreate(char* _ipAddresOfGroup, int _groupPort, char* _groupName, char* _userName);
int MultiDestroy(char* _groupName, char*_username);

#endif /* __MULTI_H__ */