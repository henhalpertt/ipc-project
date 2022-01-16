#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashMap.h"
#include "group.h"
#define USER_MAP_CAPACITY 100

struct Group
{
	char m_groupName[30];
	char m_groupIP[30];
	HashMap* m_users;
	size_t m_numOfUsers;
};


size_t GrpHashFunction(void* _key)
{
	return atoi((char*)(_key)) % USER_MAP_CAPACITY;
}

int GrpEqualityFunction(void* _firstKey, void* _secondKey)
{
	return !strcmp((char*)_firstKey, (char*)_secondKey) ? 1 : 0;
}


Group* GroupCreate(char* _groupName, char* _groupIP)
{
	Group* group;
	
	if (_groupName == NULL || _groupIP == NULL)
	{
		return NULL;
	}
	
	if ((group = (Group*)malloc(sizeof(Group))) == NULL)
	{
		return NULL;
	}
	
	if ((group->m_users = HashMapCreate(USER_MAP_CAPACITY, GrpHashFunction, GrpEqualityFunction)) == NULL)
	{
		return NULL;
	}
	
	strcpy(group->m_groupName, _groupName);
	strcpy(group->m_groupIP, _groupIP);
	group->m_numOfUsers = 0;
	
	return group;
}

void GroupDestroy(Group** _group, char** _groupIPOut)
{
	if (_group == NULL || *_group == NULL)
	{
		return;
	}
	
	HashMapDestroy(&((*_group)->m_users), NULL, NULL);
	
	*_groupIPOut = (*(_group))->m_groupIP;
	
	free(*_group);
	*_group = NULL;
}

GroupResult AddUser(Group* _group, char* _userName, char* _groupIPOut)
{
	MapResult status;
	
	if (_group == NULL)
	{
		return GROUP_UNINITIALIZED;
	}
	
	if (_userName == NULL)
	{
		return GROUP_USERNAME_UNINITIALIZED;
	}
	
	if (_groupIPOut == NULL)
	{
		return GROUP_GROUPIPOUT_UNINITIALIZED;
	}
	
	if ((status = HashMapInsert(_group->m_users, _userName, _userName)) != MAP_SUCCESS)
	{
		if (status == MAP_KEY_DUPLICATE_ERROR)
		{
			return GROUP_USER_ALREADY_EXISTS;
		}
	}
	
	strcpy(_groupIPOut, _group->m_groupIP);
	
	return GROUP_SUCCESS;
}

GroupResult RemoveUser(Group* _group, char* _userName)
{
	MapResult status;
	void* key, *value;
	
	if (_group == NULL)
	{
		return GROUP_UNINITIALIZED;
	}
	
	if (_userName == NULL)
	{
		return GROUP_USERNAME_UNINITIALIZED;
	}
	
	if ((status = HashMapRemove(_group->m_users, _userName, &key, &value)) != MAP_SUCCESS)
	{
		if (status == MAP_KEY_NOT_FOUND_ERROR)
		{
			return GROUP_USER_NOT_FOUND;
		}
	}
	
	return GROUP_SUCCESS;
}

GroupResult GetGroupName(Group*_group, char*_groupNameCopy)
{
	if (_group == NULL)
	{
		return GROUP_UNINITIALIZED;
	}
	
	if (_groupNameCopy == NULL)
	{
		return GROUP_GROUPNAMECOPY_UNINITIALIZED;
	}	

	strcpy(_groupNameCopy, _group->m_groupName);
	
	return GROUP_SUCCESS;
}






char* GetGroupNameTest(Group*_group)
{
	return _group->m_groupName;
}

char* GetGroupIPTest(Group*_group)
{
	return _group->m_groupIP;
}

HashMap* GetGroupUsersTest(Group*_group)
{
	return _group->m_users;
}

size_t GetGroupNumOfUsersTest(Group*_group)
{
	return _group->m_numOfUsers;
}
