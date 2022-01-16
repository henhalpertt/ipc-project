#ifndef __USRMNG_H__
#define __USRMNG_H__
#include "user.h"

typedef struct UsrMng UsrMng;

typedef enum {
	USERMNG_SUCCESS,
	USERMNG_GENERAL_ERROR,
	USERMNG_DUPLICATE,
	USERMNG_UNINITIALIZED,
	USERMNG_USERNAME_UNINITIALIZED,
	USERMNG_PASSWORD_UNINITIALIZED,
	USERMNG_GROUPNAME_UNINITIALIZED,
	USERMNG_ALLOCATION_FAILED,
	USERMNG_USER_NOT_FOUND,
	USERMNG_GROUP_NOT_FOUND,
	USERMNG_WRONG_PASSWORD
} UserMngResult;


UsrMng* UsrMngCreate();

void UsrMngDestroy(UsrMng** _usrMng);

UserMngResult RegisterNewUser(UsrMng* _usrMng, char* _userName, char* _password);

UserMngResult LogInUser(UsrMng* _usrMng, char* _userName, char* _password);

UserMngResult LogOutUser(UsrMng* _usrMng, char* _userName);

UserMngResult AddGroupToUser(UsrMng* _usrMng, char* _groupName, char* _userName);

UserMngResult RemoveGroupFromUser(UsrMng* _usrMng, char* _groupName, char* _userName);

UserMngResult RemoveAllGroupsFromUser(UsrMng* _usrMng, char* _userName);



#endif /* __USRMNG_H__ */
