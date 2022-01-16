#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h> 
#include "client_manager.h"
#include "client_net.h"
#include "protocol.h"
#define REPLY_ACCEPTED 1
#define REPLY_DECLINED 0
#define PORT 2021
#define IP "10.0.2.15"
#define BUFF_SIZE 100
struct ClientManager
{
	Client *m_clientNet;
};

Client* ConnectToClientNET()
{
	Client *myClient;
	myClient = CreateClient(PORT, IP);
	if(NULL == myClient)
	{
		return NULL;
	}
	ConnectToServer(myClient);
	return myClient;
}

ClientManagerSignal PackAndSendViaClientNET(char *_userName, char *_pass, Tag _action, Client *_myClient)
{
	RegisterOrLoginRequest *msg;
	GrpReqStruct *msgGrp;
	LogOutOrSeeGroupsReq *msgLogOrGrp;
	char *buffer;
	int sizeOfPacket;
	if(NULL == _userName || NULL ==_pass )
	{
		return CLNT_MNG_SEND_FAIL;
	}

	buffer = calloc(1, BUFF_SIZE * sizeof(char));
	if(_action == REGISTER_REQUEST || _action == LOGIN_REQUEST)
	{
		msg = (RegisterOrLoginRequest*)calloc(1, sizeof(RegisterOrLoginRequest));
		if(NULL == msg)
		{
			return CLNT_MNG_SEND_FAIL;
		}
		msg->m_tag = _action;
		strcpy(msg->m_username, _userName);
		strcpy(msg->m_password, _pass);
		sizeOfPacket = ProtocolPack(msg, buffer, msg->m_tag);
		SendToServer(GetSocket(_myClient), buffer, sizeOfPacket);
		free(msg);
	}
	if(_action == GROUP_CREATE_REQUEST || _action == GROUP_JOIN_REQUEST || GROUP_LEAVE_REQUEST)
	{
		msgGrp = (GrpReqStruct*)calloc(1, sizeof(GrpReqStruct));
		if(NULL == msgGrp)
		{
			return CLNT_MNG_SEND_FAIL;
		}
		msgGrp->m_tag = _action;
		strcpy(msgGrp->m_username, _userName);
		strcpy(msgGrp->m_groupName, _pass);

		sizeOfPacket = ProtocolPack(msgGrp, buffer, msgGrp->m_tag);
		SendToServer(GetSocket(_myClient), buffer, sizeOfPacket);
		free(msgGrp);
	}
	if(_action == GROUP_ALL_AVAILABLE_REQUEST || _action == LOGOUT_REQUEST)
	{
		msgLogOrGrp = (LogOutOrSeeGroupsReq*)calloc(1, sizeof(LogOutOrSeeGroupsReq));
		if(NULL == msgLogOrGrp)
		{
			return CLNT_MNG_SEND_FAIL;
		}
		msgLogOrGrp->m_tag = _action;
		strcpy(msgLogOrGrp->m_username, _userName);
		sizeOfPacket = ProtocolPack(msgLogOrGrp, buffer, msgLogOrGrp->m_tag);
		SendToServer(GetSocket(_myClient), buffer, sizeOfPacket);
		free(msgLogOrGrp);
	}
	free(buffer);
	return CLNT_MNG_SEND_OK;	
}

char ReceiveViaClientNET(Client *_clnt)
{
	Reply *reply;
	GroupCreateReply *replyGrp;
	SeeAllGroupsReply *replySeeGroups;
	Tag tag;
	char *buffer;
	if(NULL == _clnt)
	{
		return;
	}
	buffer = calloc(1, 2048 * sizeof(char));
	/* add tests later */ 
	ReceiveFromServer(_clnt, buffer);
	tag = ProtocolGetTag(buffer);
	printf("TAG: %d\n", tag);
	if(tag == REGISTER_REPLY || tag == LOGIN_REPLY || tag == LOGOUT_REPLY ) /* register / login / logout */
	{
		printf("Client reply: %d\n", reply->m_reply);
		ProtocolUnpack(reply, buffer, tag);
		return reply->m_reply;
	}
	if(tag == GROUP_CREATE_REPLY || tag == GROUP_JOIN_REPLY || GROUP_LEAVE_REPLY)
	{
		ProtocolUnpack(replyGrp, buffer, tag);
		printf("Client tag: %d\n", replyGrp->m_tag);
		printf("Client reply: %d\n", replyGrp->m_reply);
		printf("Client ip: %s\n", replyGrp->m_ip);
		printf("Client port: %s\n", replyGrp->m_port);
		return replyGrp->m_reply;
	}
	if(tag == GROUP_ALL_AVAILABLE_REPLY) /* client asked to see all groups */
	{
		replySeeGroups = (SeeAllGroupsReply*)calloc(1, sizeof(SeeAllGroupsReply));
		printf("HERE \n");
		ProtocolUnpack(replySeeGroups, buffer, tag);
/*		printf("Client reply: %d\n", replySeeGroups->m_reply);*/
		printf("Client See All Groups: %s\n", replySeeGroups->m_allGroupNames);
		free(replySeeGroups);
		return replySeeGroups->m_reply;
	}
	free(buffer);
}

void ClientDisconnected(Client *_clnt)
{
	DestroyClient(&_clnt);
}

/* END */
