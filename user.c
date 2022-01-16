#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashMap.h"
#include "user.h"
#define GROUP_MAP_CAPACITY 100

struct User
{
	char* m_userName;
	char* m_password;
	int m_isActive;
	HashMap* m_groups;
};

size_t UsrHashFunction(void* _key)
{
	return atoi((char*)(_key)) % GROUP_MAP_CAPACITY;
}

int UsrEqualityFunction(void* _firstKey, void* _secondKey)
{
	return !strcmp((char*)_firstKey, (char*)_secondKey) ? 1 : 0;
}

User* UserCreate(char* _userName, char* _password)
{
	User* user;
	if (_userName == NULL || _password == NULL)
	{
		return NULL;
	}
	if ((user = (User*)malloc(sizeof(User))) == NULL)
	{
		return NULL;
	}
	
	if ((user->m_userName = (char*)malloc(30 * sizeof(char))) == NULL)
	{
		return NULL;
	}
	
	if ((user->m_password = (char*)malloc(30 * sizeof(char))) == NULL)
	{
		return NULL;
	}
	
	if ((user->m_groups = HashMapCreate(GROUP_MAP_CAPACITY, UsrHashFunction, UsrEqualityFunction)) == NULL)
	{
		return NULL;
	}
	
	strcpy(user->m_userName, _userName);
	strcpy(user->m_password, _password);
	user->m_isActive = 0;
	
	return user;
}

void UserDestroy(User** _user)
{
	if (_user == NULL || *_user == NULL)
	{
		return;
	}
	
	HashMapDestroy(&((*_user)->m_groups), NULL, NULL);
	
	free(*_user);
	*_user = NULL;
}

UserResult ActivateUser(User* _user)
{
	if (_user == NULL)
	{
		return USER_UNINITIALIZED;
	}	
	
	_user->m_isActive = 1;
	
	return USER_SUCCESS;
}

UserResult DeactivateUser(User* _user)
{
	if (_user == NULL)
	{
		return USER_UNINITIALIZED;
	}	
	
	_user->m_isActive = 0;
	
	return USER_SUCCESS;
}

UserResult GetUserName(User* _user, char* _userNameCopy)
{
	if (_user == NULL)
	{
		return USER_UNINITIALIZED;
	}
	
	if (_userNameCopy == NULL)
	{
		return USER_USERNAME_COPY_UNINITIALIZED;
	}
	
	strcpy(_userNameCopy, _user->m_userName);
	
	return USER_SUCCESS;
}

UserResult GetPassword(User* _user, char* _passwordCopy)
{
	if (_user == NULL)
	{
		return USER_UNINITIALIZED;
	}
	
	if (_passwordCopy == NULL)
	{
		return USER_PASSWORD_COPY_UNINITIALIZED;
	}
	
	strcpy(_passwordCopy, _user->m_password);
	
	return USER_SUCCESS;
}

UserResult IsActive(User* _user, int* _isActiveCopy)
{
	if (_user == NULL)
	{
		return USER_UNINITIALIZED;
	}
	
	if (_isActiveCopy == NULL)
	{
		return USER_ISACTIVE_COPY_UNINITIALIZED;
	}	
	
	*_isActiveCopy = _user->m_isActive;
	
	return USER_SUCCESS;
}

UserResult AddGroup(User* _user, char* _groupName)
{
	MapResult status;
	
	if (_user == NULL)
	{
		return USER_UNINITIALIZED;
	}
	
	if (_groupName == NULL)
	{
		return USER_GROUPNAME_UNINITIALIZED;
	}	
	
	if ((status = HashMapInsert(_user->m_groups, _groupName, _groupName)) != MAP_SUCCESS)
	{
		if (status == MAP_KEY_DUPLICATE_ERROR)
		{
			return USER_GROUP_ALREADY_EXISTS;
		}
	}
	
	return USER_SUCCESS;
}
	
	

UserResult RemoveGroup(User* _user, char* _groupName)
{
	MapResult status;
	void* key, *value;
	
	if (_user == NULL)
	{
		return USER_UNINITIALIZED;
	}
	
	if (_groupName == NULL)
	{
		return USER_GROUPNAME_UNINITIALIZED;
	}
	
	if ((status = HashMapRemove(_user->m_groups, _groupName, &key, &value)) != MAP_SUCCESS)
	{
		if (status == MAP_KEY_NOT_FOUND_ERROR)
		{
			return USER_GROUP_NOT_FOUND;
		}
	}
	
	return USER_SUCCESS;
}

UserResult RemoveAllGroups(User* _user)
{
	if (_user == NULL)
	{
		return USER_UNINITIALIZED;
	}
	
	HashMapDestroy(&(_user->m_groups), NULL, NULL);
	
	if ((_user->m_groups = HashMapCreate(GROUP_MAP_CAPACITY, UsrHashFunction, UsrEqualityFunction)) == NULL)
	{
		return USER_GENERAL_ERROR;
	}
	
	return USER_SUCCESS;
}


	


char* GetUserNameTest(User* _user)
{
	return _user->m_userName;
}

char* GetPasswordTest(User* _user)
{
	return _user->m_password;
}

int GetIsActiveTest(User* _user)
{
	return _user->m_isActive;
}

HashMap* GetGroupsTest(User* _user)
{
	return _user->m_groups;
}
