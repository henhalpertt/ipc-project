#include "protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int PackRegisterOrLoginRequest(RegisterOrLoginRequest* _request, char* _buffer);
static int UnpackRegisterOrLoginRequest(RegisterOrLoginRequest* _request, char* _buffer);
static int PackLogOutOrSeeGroupsRequest(LogOutOrSeeGroupsReq* _request, char* _buffer);
static int UnpackLogOutOrSeeGroupsRequest(LogOutOrSeeGroupsReq* _request, char* _buffer);
static int PackGroupCreateReply(GroupCreateReply* _reply, char* _buffer);
static int UnpackGroupCreateReply(GroupCreateReply* _reply, char* _buffer);
static int PackSeeAllGroupsReply(SeeAllGroupsReply* _reply, char *_buffer);
static int UnpackSeeAllGroupsReply(SeeAllGroupsReply* _reply, char *_buffer);
static int PackGrpReq(GrpReqStruct* _request, char* _buffer);
static int UnpackGrpReq(GrpReqStruct* _request, char* _buffer);
static int PackReply(Reply*_reply, char* _buffer);
static int UnpackReply(Reply*_reply, char* _buffer);

int ProtocolPack(void *_message, char *_buffer, Tag _tag)
{	
	if(_message == NULL || _buffer == NULL || _tag < 1 || _tag > NUMBER_OF_TAGS - 2)/*check how to do >14*/
	{
		return -1;
	}
    switch(_tag)
	{
		/* REQ - pack three members */ 
		case REGISTER_REQUEST:
        case LOGIN_REQUEST:
			return PackRegisterOrLoginRequest((RegisterOrLoginRequest*) _message, _buffer);
			break;
		
		/* REQ - pack two members (tag member and user member) */
		case LOGOUT_REQUEST:
		case GROUP_ALL_AVAILABLE_REQUEST:
			return PackLogOutOrSeeGroupsRequest((LogOutOrSeeGroupsReq*) _message, _buffer);
			break;
		
		case GROUP_CREATE_REQUEST:
        case GROUP_JOIN_REQUEST:
        case GROUP_LEAVE_REQUEST:
        	return PackGrpReq((GrpReqStruct*)_message, _buffer);
        	break;
        
		/* REPLY - pack two members (tag member and reply (0 or 1)) */
        case REGISTER_REPLY:
        case LOGIN_REPLY:
        case LOGOUT_REPLY:
        case GROUP_LEAVE_REPLY:
            return PackReply((Reply*)_message, _buffer);
            break;
           
        /* REPLY - pack 4 members (tag + reply (0 or 1) + ip + port) */
        case GROUP_CREATE_REPLY:
        case GROUP_JOIN_REPLY:
			return PackGroupCreateReply((GroupCreateReply*)_message, _buffer);
			break;
		
		/* REPLY - SEE all groups */
		case GROUP_ALL_AVAILABLE_REPLY:
			return PackSeeAllGroupsReply((SeeAllGroupsReply*)_message, _buffer);
		
        default:
            return -1;
    }
}

int ProtocolUnpack(void *_message, char *_buffer, Tag _tag)
{
    if(_message == NULL || _buffer == NULL || _tag < 1 || _tag > 14)
	{
		return -1;
	}
    switch(_tag)
	{
		/* REQ - unpack three members */  
		case REGISTER_REQUEST:
        case LOGIN_REQUEST:
			return UnpackRegisterOrLoginRequest((RegisterOrLoginRequest*) _message, _buffer);
			break;
			
		/* REQ - unpack two members (tag member and user member) */	
		case LOGOUT_REQUEST:
		case GROUP_ALL_AVAILABLE_REQUEST:
			return UnpackLogOutOrSeeGroupsRequest((LogOutOrSeeGroupsReq*) _message, _buffer);
			break;
		
		case GROUP_CREATE_REQUEST:
        case GROUP_JOIN_REQUEST:
        case GROUP_LEAVE_REQUEST:
        	return UnpackGrpReq((GrpReqStruct*)_message, _buffer);
        	break;
		
		/* REPLY - unpack two members (tag member and reply (0 or 1)) */
        case REGISTER_REPLY:
        case LOGIN_REPLY:
        case LOGOUT_REPLY:
        case GROUP_LEAVE_REPLY:
            return UnpackReply((Reply*)_message, _buffer);
            break;
        
        /* REPLY - unpack 4 members (tag + reply (0 or 1) + ip + port) */
        case GROUP_CREATE_REPLY:
        case GROUP_JOIN_REPLY:
        	return UnpackGroupCreateReply((GroupCreateReply*)_message, _buffer);
        	break; 
        
        /* REPLY - See all groups */
        case GROUP_ALL_AVAILABLE_REPLY:
			return UnpackSeeAllGroupsReply((SeeAllGroupsReply*)_message, _buffer);
        /* error */ 
        default:
            return 0;
    }
}

static int PackRegisterOrLoginRequest(RegisterOrLoginRequest* _request, char* _buffer)
{
    unsigned int lenghthUserName;
    unsigned int lenghthPassword;
    int lenghthTotal;
    int bufferPlace = 0;
    if(_request -> m_username ==  NULL || _request -> m_password == NULL )/*do I need to check tag?*/
    {
        return -1;
    }
    lenghthUserName = strlen(_request -> m_username);
    lenghthPassword = strlen(_request -> m_password);
    lenghthTotal = lenghthUserName + lenghthPassword + 4;/*4 - tag + lenghthTotal + lenghthUserName + lenghthPassword*/
    _buffer[bufferPlace++] = (unsigned char) _request -> m_tag;
    _buffer[bufferPlace++] = (unsigned char)lenghthTotal;
    _buffer[bufferPlace++] = (unsigned char)lenghthUserName;
    strncpy(&_buffer[bufferPlace], _request -> m_username, lenghthUserName);
    bufferPlace = bufferPlace + lenghthUserName;
    _buffer[bufferPlace++] = (unsigned char)lenghthPassword;
    strncpy(&_buffer[bufferPlace], _request -> m_password, lenghthPassword);
    return lenghthTotal;
}
	
static int UnpackRegisterOrLoginRequest(RegisterOrLoginRequest* _request, char* _buffer)
{
    int lenghthUserName;
    int lenghthPassword;
    int bufferPlace = 0;
    _request->m_tag = (Tag)_buffer[0];
    lenghthUserName = (int)_buffer[2];
    strncpy(_request->m_username, &_buffer[3], lenghthUserName);
    bufferPlace = 3+lenghthUserName;
    lenghthPassword = _buffer[bufferPlace++];
    strncpy(_request -> m_password, &_buffer[bufferPlace], lenghthPassword);
    return REPLY_SUCCESS;
}

/* REQ - pack LOGOUT Request OR see available groups Request*/
static int PackLogOutOrSeeGroupsRequest(LogOutOrSeeGroupsReq* _request, char* _buffer)
{
    unsigned int lenghthUserName;
    int lenghthTotal;
    int bufferPlace = 0;
    if(_request -> m_username ==  NULL)
    {
        return -1;
    }
    lenghthUserName = strlen(_request->m_username);
    lenghthTotal = lenghthUserName + 3;/* tag + lengthTotal + lengthUserName */
    _buffer[bufferPlace++] = (unsigned char) _request -> m_tag;
    _buffer[bufferPlace++] = (unsigned char)lenghthTotal;
    _buffer[bufferPlace++] = (unsigned char)lenghthUserName;
    strncpy(&_buffer[bufferPlace], _request -> m_username, lenghthUserName);
    return lenghthTotal;
}

static int UnpackLogOutOrSeeGroupsRequest(LogOutOrSeeGroupsReq* _request, char* _buffer)
{
    int lenghthUserName;
    _request->m_tag = (Tag)_buffer[0];
    lenghthUserName = (int)_buffer[2];
    strncpy(_request->m_username, &_buffer[3], lenghthUserName);
    return REPLY_SUCCESS;
}

/* ------- Replies ---------- */ 
static int PackReply(Reply* _reply, char* _buffer)
{
    int lengthReply;
    int lenghthTotal = 4;
    if(!(_reply->m_tag == REGISTER_REPLY || _reply->m_tag == LOGIN_REPLY || _reply->m_tag == LOGOUT_REPLY || _reply->m_tag == GROUP_LEAVE_REPLY))
    {
        return -1;
    }
    lengthReply = sizeof(_reply -> m_reply);
    _buffer[0] = (unsigned char)_reply->m_tag;
    _buffer[1] = (unsigned char)lenghthTotal;
    _buffer[2] = (unsigned char)lengthReply;
    _buffer[3] = (unsigned char)_reply->m_reply;
    return lenghthTotal;
}

static int UnpackReply(Reply* _reply, char* _buffer)
{
    _reply->m_tag = (Tag)_buffer[0];
    _reply->m_reply = (int)_buffer[3];
    return REPLY_SUCCESS;
}

Tag ProtocolGetTag(char *_buffer)
{
    Tag tag;
	if(_buffer == NULL)
	{
		return ERROR;
	}
	tag = (Tag)_buffer[0];
	return tag;
}

static int PackGroupCreateReply(GroupCreateReply* _reply, char* _buffer)
{
	int totalLength;
	int replyLength;
	int ipLength;
	int portLength;
	int bufferPlace=0;
/*	if(_reply->m_tag != GROUP_CREATE_REPLY || _reply->m_tag != GROUP_JOIN_REPLY)*/
/*	{*/
/*		return -1;*/
/*	}*/
	replyLength = sizeof(_reply->m_reply);
	ipLength = strlen(_reply->m_ip); 
	portLength = strlen(_reply->m_port);
	totalLength = 5 + replyLength + ipLength + portLength; /* Values possibly greater than 1 byte: ip port reply*/
/*	 [tag, totLength, replyLength . . . ] */
	_buffer[bufferPlace++] = (unsigned char)_reply->m_tag;
    _buffer[bufferPlace++] = (unsigned char)totalLength;
    _buffer[bufferPlace++] = (unsigned char)replyLength;
    _buffer[bufferPlace++] = (unsigned char)_reply->m_reply;
    
    _buffer[bufferPlace++] = (unsigned char)ipLength;
    strncpy(&_buffer[bufferPlace], _reply->m_ip, ipLength);
    
	 bufferPlace += ipLength;
    _buffer[bufferPlace++] = (unsigned char)portLength;
    strncpy(&_buffer[bufferPlace], _reply->m_port, portLength);
    return totalLength;
}    

static int UnpackGroupCreateReply(GroupCreateReply* _reply, char* _buffer)
{
/*idx:    0        1          2              3             4           5         5+ipLength   5+ipLength+1        */
	/*[ [tag] [lengthT] [reply length] [reply Value] [ip length] [... ip ....] [length port] [... port ...]   ] */
	int ipLength;
	int portLength;
/*	int replyLength;*/
	int bufferPlace = 0;
	
	_reply->m_tag = (Tag)_buffer[0];
	_reply->m_reply = (int)_buffer[3];
	ipLength = (int)_buffer[4];
	
	strncpy(_reply->m_ip, &_buffer[5], ipLength);
	bufferPlace = 5 + ipLength;
	
	portLength = (int)_buffer[bufferPlace++];
	strncpy(_reply->m_port, &_buffer[bufferPlace], portLength);
	return REPLY_SUCCESS;
}
/* --- SEE ALL GROUPS REPLY  ----  */
static int PackSeeAllGroupsReply(SeeAllGroupsReply* _reply, char *_buffer)
{
	/* [tag, Lentotal, LenReply(is 1), Valuereply, lenGroupNames,  ValueGroupsNames ] */
	int totalLength;
	int replyLength;
	int groupNamesLength;
	int bufferPlace = 0;
	if(_reply->m_tag != GROUP_ALL_AVAILABLE_REPLY)
	{
		return -1;
	}
	replyLength = sizeof(_reply->m_reply);
	groupNamesLength = strlen(_reply->m_allGroupNames);
	totalLength = 4 +  groupNamesLength; /* tag and total length slots */ 
	
	_buffer[bufferPlace++] = (unsigned char)_reply->m_tag;
    _buffer[bufferPlace++] = (unsigned char)totalLength;
    _buffer[bufferPlace++] = (unsigned char)replyLength;
    _buffer[bufferPlace++] = (unsigned char)_reply->m_reply;
    strncpy(&_buffer[bufferPlace], _reply->m_allGroupNames, groupNamesLength);
    return totalLength;
}
static int UnpackSeeAllGroupsReply(SeeAllGroupsReply* _reply, char *_buffer)
{
	/*idx:0        1          2              3                4                   5            */
	/*[ [tag] [lengthT] [reply length] [reply Value] [GroupNames length] [... GroupNames ....]    ] */
	int lengthGroupNames;
	
	_reply->m_tag = (unsigned char)_buffer[0];
	_reply->m_reply = (unsigned char)_buffer[3];
	lengthGroupNames = (unsigned char)_buffer[4];
	strncpy(_reply->m_allGroupNames, &_buffer[4], lengthGroupNames);
	return REPLY_SUCCESS;
}

static int PackGrpReq(GrpReqStruct* _request, char* _buffer)
{
    unsigned int lengthUserName;
    unsigned int grpNameLen;
    unsigned int lengthTotal;
    int bufferPlace = 0;
    if(_request ->m_username ==  NULL || _request->m_groupName == NULL || _buffer == NULL )
    {
    	printf("NOOOO");
        return -1;
    }
    lengthUserName = strlen(_request -> m_username);
    grpNameLen = strlen(_request -> m_groupName);
    lengthTotal = lengthUserName + grpNameLen + 4;/*4 - tag + lengthTotal + lengthUserName + grpNameLen*/
    _buffer[bufferPlace++] = (unsigned char) _request -> m_tag;
    _buffer[bufferPlace++] = (unsigned char)lengthTotal;
    _buffer[bufferPlace++] = (unsigned char)lengthUserName;
    strncpy(&_buffer[bufferPlace], _request -> m_username, lengthUserName);
    bufferPlace = bufferPlace + lengthUserName;
    _buffer[bufferPlace++] = (unsigned char)grpNameLen;
    strncpy(&_buffer[bufferPlace], _request -> m_groupName, grpNameLen);
    return lengthTotal;
}

static int UnpackGrpReq(GrpReqStruct* _request, char* _buffer)
{
    int lenghthUserName;
    int grpNameLen;
    int bufferPlace = 0;
    _request->m_tag = (Tag)_buffer[0];
    lenghthUserName = (int)_buffer[2];
    strncpy(_request->m_username, &_buffer[3], lenghthUserName);
    bufferPlace = 3+lenghthUserName;
    grpNameLen = _buffer[bufferPlace++];
    strncpy(_request->m_groupName, &_buffer[bufferPlace], grpNameLen);
    return REPLY_SUCCESS;
}

/* test*/ 
/*int main()*/
/*{*/
/*	char buffer[100];*/
/*	GroupCreateReply reply, emptyReply;*/
/*	int size;*/
/*	RegisterOrLoginRequest message, emptyMessage;*/
/*	message.m_tag = GROUP_CREATE_REQUEST;*/
/*	strcpy(message.m_username, "hen");*/
/*	strcpy(message.m_password, "group5");*/
/*	PackRegisterOrLoginRequest(&message, buffer);*/
/*	*/
/*	UnpackRegisterOrLoginRequest(&emptyMessage, buffer);*/
/*	printf("tag: %d\n", emptyMessage.m_tag);*/
/*	printf("name: %s\n", emptyMessage.m_username);*/
/*	printf("group name: %s", emptyMessage.m_password);*/
/*	*/
/*	reply.m_reply = 1;*/
/*	strcpy(reply.m_ip, "10.2.0.15");*/
/*	strcpy(reply.m_port, "3005");*/
/*	reply.m_tag = GROUP_CREATE_REPLY;*/
/*	*/
/*	size = PackGroupCreateReply(&reply, buffer);*/
/*	*/
/*	UnpackGroupCreateReply(&emptyReply, buffer);*/
/*	printf("tag: %d", emptyReply.m_tag);*/
/*	printf("ip: %s", emptyReply.m_ip);*/
/*	printf("port: %s", emptyReply.m_port);*/
/*	printf("reply: %d", emptyReply.m_reply);*/
/*	printf("size = %d", size);*/
/*	return 0;*/
/*}*/


/* END */



