#ifndef __USER_H__
#define __USER_H__

typedef struct User User;

typedef enum {
	USER_SUCCESS,
	USER_UNINITIALIZED,
	USER_USERNAME_COPY_UNINITIALIZED,
	USER_PASSWORD_COPY_UNINITIALIZED,
	USER_ISACTIVE_COPY_UNINITIALIZED,
	USER_GROUPNAME_UNINITIALIZED,
	USER_GROUP_ALREADY_EXISTS,
	USER_GROUP_NOT_FOUND,
	USER_GENERAL_ERROR
} UserResult;

User* UserCreate(char* _userName, char* _password);

void UserDestroy(User** _user);

UserResult ActivateUser(User* _user);

UserResult DeactivateUser(User* _user);

UserResult GetUserName(User* _user, char* _userNameCopy);

UserResult GetPassword(User* _user, char* _passwordCopy);

UserResult IsActive(User* _user, int* _isActiveCopy);

UserResult AddGroup(User* _user, char* _groupName);

UserResult RemoveGroup(User* _user, char* _groupName);

UserResult RemoveAllGroups(User* _user);




#endif /* __USER_H__ */
