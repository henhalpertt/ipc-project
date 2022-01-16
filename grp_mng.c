#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashMap.h"
#include "genqueue.h"
#include "vector.h"
#include "group.h"
#include "grp_mng.h"
#define MAP_CAPACITY 100
#define QUEUE_SIZE 100
#define IP_STRING_SIZE 30


struct GrpMng
{
	HashMap* m_groupMap;
	Queue* m_groupsID;
};


size_t GrpMngHashFunction(void* _key)
{
	return atoi((char*)(_key)) % MAP_CAPACITY;
}

int GrpMngEqualityFunction(void* _firstKey, void* _secondKey)
{
	return !strcmp((char*)_firstKey, (char*)_secondKey) ? 1 : 0;
}

static int GroupRemoveUser(const void* _key, void* _value, void* _context);
static int InsertGroupNameToVector(const void* _key, void* _value, void* _context);
static int GetGroupNameFromVector(void* _element, size_t _index, void* _context);
static int integerLength(int _num);
static void myItoA(int _num, char _str[]);

GrpMng* GrpMngCreate()
{
	GrpMng* grpMng;
	int i;
	char *multiCastIP, uniqueIP[5], IP[] = "224.0.0.";
	
	if ((grpMng = (GrpMng*)malloc(sizeof(GrpMng))) == NULL)
	{
		return NULL;
	}
	
	if ((grpMng->m_groupMap = HashMapCreate(MAP_CAPACITY, GrpMngHashFunction, GrpMngEqualityFunction)) == NULL)
	{
		return NULL;
	}
	
	if ((grpMng->m_groupsID = QueueCreate(QUEUE_SIZE)) == NULL)
	{
		return NULL;
	}
	
	for (i = 1; i <= QUEUE_SIZE; i++)
	{
		if ((multiCastIP = (char*)malloc(IP_STRING_SIZE * sizeof(char))) == NULL)
		{
			return NULL;
		}
		
		myItoA(i, uniqueIP);
		strcpy(multiCastIP, IP);
		strcat(multiCastIP, uniqueIP);
		
		if (QueueInsert(grpMng->m_groupsID, multiCastIP) != QUEUE_SUCCESS)
		{
			return NULL;
		}
	}
	
				
	return grpMng;
}

void GrpMngDestroy(GrpMng** _grpMng)
{
	if (_grpMng == NULL || *_grpMng == NULL)
	{
		return;
	}
	
	HashMapDestroy(&((*_grpMng)->m_groupMap), NULL, NULL);
	QueueDestroy(&((*_grpMng)->m_groupsID), NULL);
	
	free(*_grpMng);
	*_grpMng = NULL;
}

GroupMngResult AddNewGroup(GrpMng* _grpMng, char* _groupName, char* _groupIPout)
{
	Group* group;
	char* groupIP;
	MapResult status;
	char* newGroupName;
	
	if (_grpMng == NULL)
	{
		return GROUPMNG_UNINITIALIZED;
	}
	
	if (_groupName == NULL)
	{
		return GROUPMNG_GROUP_NAME_UNINITIALIZED;
	}
	
	if (_groupIPout == NULL)
	{
		return GROUPMNG_GROUPIPOUT_UNINITIALIZED;
	}
	
	if (QueueRemove(_grpMng->m_groupsID, (void**)&groupIP) != QUEUE_SUCCESS)
	{
		return GROUPMNG_GENERAL_ERROR;
	}
	
	if ((group = GroupCreate(_groupName, groupIP)) == NULL)
	{
		return GROUPMNG_GENERAL_ERROR;
	}
		
	if ((newGroupName = (char*)malloc(30 * sizeof(char))) == NULL)
	{
		return GROUPMNG_GENERAL_ERROR;
	}
	
	strcpy(newGroupName, _groupName);
	
	if ((status = HashMapInsert(_grpMng->m_groupMap, newGroupName, group)) != MAP_SUCCESS)
	{
		if (status == MAP_KEY_DUPLICATE_ERROR)
		{
			return GROUPMNG_DUPLICATE;
		}
		else
		{
			return GROUPMNG_GENERAL_ERROR;
		}
	}
	
	strcpy(_groupIPout, groupIP);
	
	return GROUPMNG_SUCCESS;
}

GroupMngResult DeleteGroup(GrpMng* _grpMng, char* _groupName)
{
	char* key, *groupIP;
	Group* group;
	MapResult status;
	
	if (_grpMng == NULL)
	{
		return GROUPMNG_UNINITIALIZED;
	}
	
	if (_groupName == NULL)
	{
		return GROUPMNG_GROUP_NAME_UNINITIALIZED;
	}
	
	if ((status = HashMapRemove(_grpMng->m_groupMap, _groupName, (void**)&key, (void**)&group)) != MAP_SUCCESS)
	{
		if (status == MAP_KEY_NOT_FOUND_ERROR)
		{
			return GROUPMNG_GROUP_DOES_NOT_EXIST;
		}
		else
		{
			return GROUPMNG_GENERAL_ERROR;
		}
	}
	
	GroupDestroy(&group, &groupIP);
	
	if (QueueInsert(_grpMng->m_groupsID, groupIP) != QUEUE_SUCCESS)
	{
		return GROUPMNG_GENERAL_ERROR;
	}
	
	return GROUPMNG_SUCCESS;	
}

GroupMngResult AddUserToGroup(GrpMng* _grpMng, char* _userName, char* _groupName, char* _groupIPout)
{
	Group* group;
	MapResult status;
	
	if (_grpMng == NULL)
	{
		return GROUPMNG_UNINITIALIZED;
	}
	
	if (_userName == NULL)
	{
		return GROUPMNG_USER_NAME_UNINITIALIZED;
	}
	
	if (_groupName == NULL)
	{
		return GROUPMNG_GROUP_NAME_UNINITIALIZED;
	}
	
	if (_groupIPout == NULL)
	{
		return GROUPMNG_GROUPIPOUT_UNINITIALIZED;
	}
	
	if ((status = HashMapFind(_grpMng->m_groupMap, _groupName, (void**)&group)) != MAP_SUCCESS)
	{
		if (status == MAP_KEY_NOT_FOUND_ERROR)
		{
			return GROUPMNG_GROUP_DOES_NOT_EXIST;
		}
		else
		{
			return GROUPMNG_GENERAL_ERROR;
		}
	}
	
	if ((status = AddUser(group, _userName, _groupIPout)) != GROUP_SUCCESS)
	{
		if (status == GROUP_USER_ALREADY_EXISTS)
		{
			return GROUPMNG_DUPLICATE;
		}
		else
		{
			return GROUPMNG_GENERAL_ERROR;
		}
	}
	
	return GROUPMNG_SUCCESS;	
}	

GroupMngResult RemoveUserFromGroup(GrpMng* _grpMng, char* _userName, char* _groupName)
{
	Group* group;
	MapResult status;
	
	if (_grpMng == NULL)
	{
		return GROUPMNG_UNINITIALIZED;
	}
	
	if (_userName == NULL)
	{
		return GROUPMNG_USER_NAME_UNINITIALIZED;
	}
	
	if (_groupName == NULL)
	{
		return GROUPMNG_GROUP_NAME_UNINITIALIZED;
	}
	
	if ((status = HashMapFind(_grpMng->m_groupMap, _groupName, (void**)&group)) != MAP_SUCCESS)
	{
		if (status == MAP_KEY_NOT_FOUND_ERROR)
		{
			return GROUPMNG_GROUP_DOES_NOT_EXIST;
		}
		else
		{
			return GROUPMNG_GENERAL_ERROR;
		}
	}

	if ((status = RemoveUser(group, _userName)) != GROUP_SUCCESS)
	{
		if (status == GROUP_USER_NOT_FOUND)
		{
			return GROUPMNG_USER_DOES_NOT_EXIST;
		}
		else
		{
			return GROUPMNG_GENERAL_ERROR;
		}
	}
	
	return GROUPMNG_SUCCESS;	
}	
	
GroupMngResult RemoveUserFromAllGroups(GrpMng* _grpMng, char* _userName)
{
	if (_grpMng == NULL)
	{
		return GROUPMNG_UNINITIALIZED;
	}
	
	if (_userName == NULL)
	{
		return GROUPMNG_USER_NAME_UNINITIALIZED;
	}
	
	HashMapForEach(_grpMng->m_groupMap, GroupRemoveUser, _userName);

	return GROUPMNG_SUCCESS;
}

GroupMngResult GetAllGroupNames(GrpMng* _grpMng, char* _groupNames)
{
	Vector* groupNames;
	
	if (_grpMng == NULL)
	{
		return GROUPMNG_UNINITIALIZED;
	}
	
	groupNames = VectorCreate(100, 0);	
	HashMapForEach(_grpMng->m_groupMap, InsertGroupNameToVector, groupNames);
	VectorForEach(groupNames, GetGroupNameFromVector, _groupNames);
	VectorDestroy(&groupNames, NULL);
		
	return GROUPMNG_SUCCESS;
}


static int GroupRemoveUser(const void* _key, void* _value, void* _context)
{
	RemoveUser((Group*)_value, (char*)_context);
	
	return 1;
}

static int InsertGroupNameToVector(const void* _key, void* _value, void* _context)
{
	VectorAppend((Vector*)_context, (void*)_key);
	
	return 1;
}

static int GetGroupNameFromVector(void* _element, size_t _index, void* _context)
{
	strcat((char*)_context, (char*)_element);
	strcat((char*)_context, ", ");
	
	return 1;
}



static int integerLength(int _num)
{
	int i = 0;
	
	while (_num != 0)
	{
		_num /= 10;
		i++;
	}
	
	return i;
}

static void myItoA(int _num, char _str[])
{
	int i, sizeInt = integerLength(_num), sizeStr = strlen(_str);
	
	while (_num != 0 )
	{	
		for (i = sizeInt-1; i >= 0; i--)
		{ 
			_str[i] = _num % 10 + '0';
			_num /= 10;
		}
	}
	
	_str[sizeInt] = '\0';
}


HashMap* GetHashMap2(GrpMng* _grpMng)
{
	return _grpMng->m_groupMap;
}

size_t GetHashMapSize2(GrpMng* _grpMng)
{
	return HashMapSize(_grpMng->m_groupMap);
}

Queue* GetQueue(GrpMng* _grpMng)
{
	return _grpMng->m_groupsID;
}
