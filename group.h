#ifndef __GROUP_H__
#define __GROUP_H__

typedef struct Group Group;

typedef enum {
	GROUP_SUCCESS,
	GROUP_UNINITIALIZED,
	GROUP_USERNAME_UNINITIALIZED,
	GROUP_GROUPNAMECOPY_UNINITIALIZED,
	GROUP_GROUPIPOUT_UNINITIALIZED,
	GROUP_USER_ALREADY_EXISTS,
	GROUP_USER_NOT_FOUND
} GroupResult;

Group* GroupCreate(char* _groupName, char* _groupIP);

void GroupDestroy(Group** _group, char** _groupIPOut);

GroupResult AddUser(Group* _group, char* _userName, char* _groupIPOut);

GroupResult RemoveUser(Group* _group, char* _userName);

GroupResult GetGroupName(Group*_group, char*_groupNameCopy);


#endif /* __GROUP_H__ */
