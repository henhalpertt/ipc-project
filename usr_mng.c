#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashMap.h"
#include "user.h"
#include "usr_mng.h"
#define MAP_CAPACITY 100


struct UsrMng
{
	HashMap* m_userMap;
};


size_t UsrMngHashFunction(void* _key)
{
	return atoi((char*)(_key)) % MAP_CAPACITY;
}

int UsrMngEqualityFunction(void* _firstKey, void* _secondKey)
{
	return !strcmp((char*)_firstKey, (char*)_secondKey) ? 1 : 0;
}

UsrMng* UsrMngCreate()
{
	UsrMng* usrMng;
	
	if ((usrMng = (UsrMng*)malloc(sizeof(UsrMng))) == NULL)
	{
		return NULL;
	}
	
	if ((usrMng->m_userMap = HashMapCreate(MAP_CAPACITY, UsrMngHashFunction, UsrMngEqualityFunction)) == NULL)
	{
		return NULL;
	}
	
	return usrMng;
}

void UsrMngDestroy(UsrMng** _usrMng)
{
	if (_usrMng == NULL || *_usrMng == NULL)
	{
		return;
	}
	
	HashMapDestroy(&((*_usrMng)->m_userMap), NULL, NULL);
	
	free(*_usrMng);
	*_usrMng = NULL;
}

UserMngResult RegisterNewUser(UsrMng* _usrMng, char* _userName, char* _password)
{
	User* user;
	MapResult status;
	char* newUserName;
	
	if (_usrMng == NULL)
	{
		return USERMNG_UNINITIALIZED;
	}
	if (_userName == NULL)
	{
		return USERMNG_USERNAME_UNINITIALIZED;
	}
	if (_password == NULL)
	{
		return USERMNG_PASSWORD_UNINITIALIZED;
	}
	
	user = UserCreate(_userName, _password);
	
	if ((newUserName = (char*)malloc(30 * sizeof(char))) == NULL)
	{
		return USERMNG_GENERAL_ERROR;
	}
	
	strcpy(newUserName, _userName);
	
	status = HashMapInsert(_usrMng->m_userMap, newUserName, user);
	if (status == MAP_KEY_DUPLICATE_ERROR)
	{
		UserDestroy(&user);
		return USERMNG_DUPLICATE;
	}
	else if (status != MAP_SUCCESS)
	{
		UserDestroy(&user);
		return USERMNG_GENERAL_ERROR;
	}
	else
	{
		return USERMNG_SUCCESS;
	}
}

UserMngResult LogInUser(UsrMng* _usrMng, char* _userName, char* _password)
{
	User* user;
	MapResult status;
	char passwordCopy[30];
	
	if (_usrMng == NULL)
	{
		return USERMNG_UNINITIALIZED;
	}
	if (_userName == NULL)
	{
		return USERMNG_USERNAME_UNINITIALIZED;
	}
	if (_password == NULL)
	{
		return USERMNG_PASSWORD_UNINITIALIZED;
	}
	if ((status = HashMapFind(_usrMng->m_userMap, _userName, (void**)&user)) != MAP_SUCCESS)
	{
		if (status == MAP_KEY_NOT_FOUND_ERROR)
		{
			return USERMNG_USER_NOT_FOUND;
		}
		else
		{

			return USERMNG_GENERAL_ERROR;
		}
	}
	
	GetPassword(user, passwordCopy);
	if(strcmp(_password, passwordCopy) == 0)
	{
		printf("33\n");
		ActivateUser(user);
		return USERMNG_SUCCESS;
	}
	return USERMNG_WRONG_PASSWORD;
}	

UserMngResult LogOutUser(UsrMng* _usrMng, char* _userName)
{
	User* user;
	MapResult status;
	
	if (_usrMng == NULL)
	{
		return USERMNG_UNINITIALIZED;
	}
	if (_userName == NULL)
	{
		return USERMNG_USERNAME_UNINITIALIZED;
	}
	
	if ((status = HashMapFind(_usrMng->m_userMap, _userName, (void**)&user)) != MAP_SUCCESS)
	{
		if (status == MAP_KEY_NOT_FOUND_ERROR)
		{
			return USERMNG_USER_NOT_FOUND;
		}
		else
		{
			return USERMNG_GENERAL_ERROR;
		}
	}
	
	DeactivateUser(user);
	return USERMNG_SUCCESS;
}

UserMngResult AddGroupToUser(UsrMng* _usrMng, char* _groupName, char* _userName)
{
	User* user;
	MapResult status;
	
	if (_usrMng == NULL)
	{
		return USERMNG_UNINITIALIZED;
	}
	
	if (_groupName == NULL)
	{
		return USERMNG_GROUPNAME_UNINITIALIZED;
	}
	
	if (_userName == NULL)
	{
		return USERMNG_USERNAME_UNINITIALIZED;
	}
	
	if ((status = HashMapFind(_usrMng->m_userMap, _userName, (void**)&user)) != MAP_SUCCESS)
	{
		if (status == MAP_KEY_NOT_FOUND_ERROR)
		{
			return USERMNG_USER_NOT_FOUND;
		}
		else
		{
			return USERMNG_GENERAL_ERROR;
		}
	}
	
	if ((status = AddGroup(user, _groupName)) != USER_SUCCESS)
	{
		if (status == USER_GROUP_ALREADY_EXISTS)
		{
			return USERMNG_DUPLICATE;
		}
		else
		{
			return USERMNG_GENERAL_ERROR;
		}
	}
	
	return USERMNG_SUCCESS;	
}	

UserMngResult RemoveGroupFromUser(UsrMng* _usrMng, char* _groupName, char* _userName)
{
	User* user;
	MapResult status;
	
	if (_usrMng == NULL)
	{
		return USERMNG_UNINITIALIZED;
	}
	
	if (_groupName == NULL)
	{
		return USERMNG_GROUPNAME_UNINITIALIZED;
	}
	
	if (_userName == NULL)
	{
		return USERMNG_USERNAME_UNINITIALIZED;
	}
	
	if ((status = HashMapFind(_usrMng->m_userMap, _userName, (void**)&user)) != MAP_SUCCESS)
	{
		if (status == MAP_KEY_NOT_FOUND_ERROR)
		{
			return USERMNG_USER_NOT_FOUND;
		}
		else
		{
			return USERMNG_GENERAL_ERROR;
		}
	}

	if ((status = RemoveGroup(user, _groupName)) != USER_SUCCESS)
	{
		if (status == USER_GROUP_NOT_FOUND)
		{
			return USERMNG_GROUP_NOT_FOUND;
		}
		else
		{
			return USERMNG_GENERAL_ERROR;
		}
	}
	
	return USERMNG_SUCCESS;	
}

UserMngResult RemoveAllGroupsFromUser(UsrMng* _usrMng, char* _userName)
{
	User* user;
	MapResult status;
	
	if (_usrMng == NULL)
	{
		return USERMNG_UNINITIALIZED;
	}
		
	if (_userName == NULL)
	{
		return USERMNG_USERNAME_UNINITIALIZED;
	}
	
	if ((status = HashMapFind(_usrMng->m_userMap, _userName, (void**)&user)) != MAP_SUCCESS)
	{
		if (status == MAP_KEY_NOT_FOUND_ERROR)
		{
			return USERMNG_USER_NOT_FOUND;
		}
		else
		{
			return USERMNG_GENERAL_ERROR;
		}
	}

	RemoveAllGroups(user);
	
	return USERMNG_SUCCESS;
}







HashMap* GetHashMap(UsrMng* _usrMng)
{
	return _usrMng->m_userMap;
}

size_t GetHashMapSize(UsrMng* _usrMng)
{
	return HashMapSize(_usrMng->m_userMap);
}
