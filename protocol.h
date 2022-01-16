#ifndef __PROTOCOL__H__
#define __PROTOCOL__H__
#define MAX_SIZE 30
#define BUFFER_SIZE 4096
#define REPLY_SUCCESS 0
#define REPLY_FAIL 1
#define MAX_SIZE_GROUPS 1000

typedef enum
{
    ERR_OK,
    ERROR_WRONG_INPUT,
    ERROR_PROTOCOL_PACK
}ProtocolError;

typedef enum
{
    REGISTER_REQUEST = 1,
    REGISTER_REPLY = 2,
    LOGIN_REQUEST = 3, 
    LOGIN_REPLY = 4,
    LOGOUT_REQUEST = 5,
    LOGOUT_REPLY = 6 ,
    GROUP_CREATE_REQUEST = 7,
    GROUP_CREATE_REPLY = 8,
    GROUP_JOIN_REQUEST = 9,
    GROUP_JOIN_REPLY = 10,
    GROUP_LEAVE_REQUEST = 11,
    GROUP_LEAVE_REPLY = 12,
    GROUP_ALL_AVAILABLE_REQUEST = 13,
    GROUP_ALL_AVAILABLE_REPLY = 14,
    ERROR = 15,
    NUMBER_OF_TAGS = 16
}Tag;

/*  registerr / login */
typedef struct RegisterOrLoginRequest
{
    char m_username[MAX_SIZE];
    char m_password[MAX_SIZE];
    Tag m_tag;
}RegisterOrLoginRequest;

typedef struct Reply
{
    char m_reply;
    Tag m_tag;
}Reply;

/* group create / join structs  */
typedef struct GrpReqStruct
{
	char m_username[MAX_SIZE];
    char m_groupName[MAX_SIZE]; 
    Tag m_tag;
}GrpReqStruct;

typedef struct GroupCreateReply
{
	Tag m_tag;
	char m_reply;
	char m_ip[15];
	char m_port[5];
}GroupCreateReply;

typedef struct LogOutOrSeeGroupsReq
{
    char m_username[MAX_SIZE];
    Tag m_tag;
}LogOutOrSeeGroupsReq;

/* see all groups reply */
typedef struct SeeAllGroupsReply
{
/*	char m_eos;*/
	char m_allGroupNames[MAX_SIZE_GROUPS];
	char m_reply;
	Tag m_tag;
	
}SeeAllGroupsReply;

int ProtocolPack(void *_message, char *_buffer, Tag _tag);

int ProtocolUnpack(void *_message, char *_buffer, Tag _tag);

int ProtocolCheckPack(char *_buffer, int _size);

Tag ProtocolGetTag(char *_buffer);

#endif /*__PROTOCOL__H__*/

