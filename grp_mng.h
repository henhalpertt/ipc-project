#ifndef __GRPMNG_H__
#define __GRPMNG_H__
#include "vector.h"
#include "group.h"

typedef struct GrpMng GrpMng;

typedef enum {
	GROUPMNG_SUCCESS,
	GROUPMNG_UNINITIALIZED,
	GROUPMNG_GROUP_NAME_UNINITIALIZED,
	GROUPMNG_USER_NAME_UNINITIALIZED,
	GROUPMNG_GROUPIPOUT_UNINITIALIZED,
	GROUPMNG_DUPLICATE,
	GROUPMNG_GROUP_DOES_NOT_EXIST,
	GROUPMNG_USER_DOES_NOT_EXIST,
	GROUPMNG_GENERAL_ERROR
} GroupMngResult;



GrpMng* GrpMngCreate();

void GrpMngDestroy(GrpMng** _grpMng);

GroupMngResult AddNewGroup(GrpMng* _grpMng, char* _groupName, char* _groupIPout);

GroupMngResult DeleteGroup(GrpMng* _grpMng, char* _groupName);

GroupMngResult AddUserToGroup(GrpMng* _grpMng, char* _userName, char* _groupName, char* _groupIPout);

GroupMngResult RemoveUserFromGroup(GrpMng* _grpMng, char* _userName, char* _groupName);

GroupMngResult RemoveUserFromAllGroups(GrpMng* _grpMng, char* _userName);

GroupMngResult GetAllGroupNames(GrpMng* _grpMng, char* _groupNames);









#endif /* __GRPMNG_H__ */
