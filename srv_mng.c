#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "usr_mng.h"
#include "grp_mng.h"
#include "server_net.h"
#include "srv_mng.h"
#include "protocol.h"
#define TRUE 1
#define SUCCESS 1
#define FAILURE 0
#define PORT 2021
#define IPV6_TYPE 0
#define IPV4_TYPE 1
#define IP "127.0.0.1"

struct SrvMng
{
	UsrMng* m_userManager;
	GrpMng* m_groupManager;
	Server* m_serverNet;
};

void GotMessage(char* _buffer, size_t _messageSize, void* _context, int _clientID)
{
	Tag tag;
	int replyPacketSize;
	char reply, internalBuffer[4096];
	char ip[15];
	char port[10] = "2021";
	
	RegisterOrLoginRequest *unpackedData;
	GrpReqStruct *unpackedDataGrp;
	LogOutOrSeeGroupsReq *unpackedDataSeeGrpsOrLogOut;
	
	Reply dataToSend;
	GroupCreateReply dataToSendGroup;
	SeeAllGroupsReply dataToSendSeeGrps;
	SrvMng* srvMng = (SrvMng*)_context;
	char *EmptyStrForGroups;
	
/*	InitStruct(unpackedData1);*/
	if (_buffer == NULL || _context == NULL)
	{
		return;
	}
		
	/*if (ProtocolCheckPack(_buffer, _messageSize) != TRUE)
	{
		 handle 
	}*/
	
	tag = ProtocolGetTag(_buffer);
	printf("TAG: %d\n", tag);
	printf("SRVRbuffer: %s\n", _buffer);
	switch (tag)
	{
		case REGISTER_REQUEST: 
				if ((unpackedData = (RegisterOrLoginRequest*)calloc(30, sizeof(RegisterOrLoginRequest))) == NULL)
				{
					printf("error\n");
				}
				ProtocolUnpack(unpackedData, _buffer, tag);
				printf("unpacked reg username: %s\n", unpackedData->m_username);
				printf("unpacked  reg pass : %s\n", unpackedData->m_password);
				if (RegisterNewUser(srvMng->m_userManager, unpackedData->m_username, unpackedData->m_password) == USERMNG_SUCCESS)
				{
					dataToSend.m_reply = SUCCESS;
				}
				else
				{
					dataToSend.m_reply = FAILURE;
				}
				dataToSend.m_tag = REGISTER_REPLY;
				replyPacketSize = ProtocolPack(&dataToSend, internalBuffer, REGISTER_REPLY);
				SendToClient(internalBuffer, replyPacketSize, _clientID);
				break;
		case LOGIN_REQUEST: 
		
				if ((unpackedData = (RegisterOrLoginRequest*)calloc(30, sizeof(RegisterOrLoginRequest))) == NULL)
				{
					printf("error\n");
				}	
				ProtocolUnpack(unpackedData, _buffer, tag);
				printf("unpacked log username: %s\n", unpackedData->m_username);
				printf("unpacked log pass : %s\n", unpackedData->m_password);
				if (LogInUser(srvMng->m_userManager, unpackedData->m_username, unpackedData->m_password) == USERMNG_SUCCESS)
				{
					printf("HERE\n");
					dataToSend.m_reply = SUCCESS;
				}
				else
				{
					dataToSend.m_reply = FAILURE;
				}
				free(unpackedData);
				dataToSend.m_tag = LOGIN_REPLY;
				replyPacketSize = ProtocolPack(&dataToSend, internalBuffer, LOGIN_REPLY);
				SendToClient(internalBuffer, replyPacketSize, _clientID);
				break;
		case LOGOUT_REQUEST:
				if ((unpackedDataSeeGrpsOrLogOut = (LogOutOrSeeGroupsReq*)calloc(30, sizeof(LogOutOrSeeGroupsReq))) == NULL)
				{
					printf("error\n");
				}
				ProtocolUnpack(unpackedDataSeeGrpsOrLogOut, _buffer, tag);
				printf("unpacked log username: %s\n", unpackedDataSeeGrpsOrLogOut->m_username);
				if (LogOutUser(srvMng->m_userManager, unpackedDataSeeGrpsOrLogOut->m_username) == USERMNG_SUCCESS)
				{
					RemoveAllGroupsFromUser(srvMng->m_userManager, unpackedDataSeeGrpsOrLogOut->m_username);
					RemoveUserFromAllGroups(srvMng->m_groupManager, unpackedDataSeeGrpsOrLogOut->m_username);
					dataToSend.m_reply = SUCCESS;
				}
				else
				{
					dataToSend.m_reply = FAILURE;
				}
				dataToSend.m_tag = LOGOUT_REPLY;
				replyPacketSize = ProtocolPack(&dataToSend, internalBuffer, LOGOUT_REPLY);
				SendToClient(internalBuffer, replyPacketSize, _clientID);
				free(unpackedDataSeeGrpsOrLogOut);
				break;
		case GROUP_CREATE_REQUEST:
		case GROUP_JOIN_REQUEST:
				if ((unpackedDataGrp = (GrpReqStruct*)calloc(30, sizeof(GrpReqStruct))) == NULL)
				{
					printf("error\n");
				}
				ProtocolUnpack(unpackedDataGrp, _buffer, tag); /* unpack username and grp name */
				printf("unpacked group tag: %d\n", unpackedDataGrp->m_tag);
				printf("unpacked group name: %s\n", unpackedDataGrp->m_groupName);
				printf("unpacked group username: %s\n", unpackedDataGrp->m_username);
				
				if(tag == GROUP_CREATE_REQUEST)
				{
					if (AddNewGroup(srvMng->m_groupManager, unpackedDataGrp->m_groupName, ip) == USERMNG_SUCCESS)
					{
						dataToSendGroup.m_reply = SUCCESS;
						strcpy(dataToSendGroup.m_port, port);
						strcpy(dataToSendGroup.m_ip, ip);
						printf("NEW IP %s\n\n", dataToSendGroup.m_ip);
						dataToSendGroup.m_tag = GROUP_CREATE_REPLY;
					}
					else
					{
						dataToSendGroup.m_reply = FAILURE;
						strcpy(dataToSendGroup.m_port, "port");
						strcpy(dataToSendGroup.m_ip, "ip");
						dataToSendGroup.m_tag = GROUP_CREATE_REPLY;
					}
				}
				if(tag == GROUP_JOIN_REQUEST)
				{
					if (AddUserToGroup(srvMng->m_groupManager,unpackedDataGrp->m_username, unpackedDataGrp->m_groupName, ip) == USERMNG_SUCCESS)
					{
						dataToSendGroup.m_reply = SUCCESS;
						strcpy(dataToSendGroup.m_port, port);
						strcpy(dataToSendGroup.m_ip, ip);
						printf("NEW IP %s\n\n", dataToSendGroup.m_ip);
						dataToSendGroup.m_tag = GROUP_JOIN_REPLY;
					}
					else
					{
						dataToSendGroup.m_reply = FAILURE;
						strcpy(dataToSendGroup.m_port, "port");
						strcpy(dataToSendGroup.m_ip, "ip");
						dataToSendGroup.m_tag = GROUP_JOIN_REPLY;
					}
				}
				replyPacketSize = ProtocolPack(&dataToSendGroup, internalBuffer, GROUP_CREATE_REPLY);
				printf("Srvr buffer: %s\n", internalBuffer);
				printf("packSize: %d\n", replyPacketSize);
				SendToClient(internalBuffer, replyPacketSize, _clientID);
				break;
				
		case GROUP_ALL_AVAILABLE_REQUEST:
				EmptyStrForGroups = (char*)calloc(MAX_SIZE_GROUPS ,sizeof(char));
				if(EmptyStrForGroups == NULL)
				{
					return;
				}
				if(GetAllGroupNames(srvMng->m_groupManager, EmptyStrForGroups) == GROUPMNG_SUCCESS)
				{
					dataToSendSeeGrps.m_reply = SUCCESS;
					strcpy(dataToSendSeeGrps.m_allGroupNames , EmptyStrForGroups);
				}
				else
				{
					dataToSendSeeGrps.m_reply = FAILURE;
					strcpy(dataToSendSeeGrps.m_allGroupNames , "No Groups");
				}
				dataToSendSeeGrps.m_tag = GROUP_ALL_AVAILABLE_REPLY;
				replyPacketSize = ProtocolPack(&dataToSendSeeGrps, internalBuffer, GROUP_ALL_AVAILABLE_REPLY);
				printf("Srvr buffer: %s\n", internalBuffer);
				printf("packSize: %d\n", replyPacketSize);
				SendToClient(internalBuffer, replyPacketSize, _clientID);
				free(EmptyStrForGroups);
				break;
				
		case GROUP_LEAVE_REQUEST:
				if ((unpackedDataGrp = (GrpReqStruct*)calloc(30, sizeof(GrpReqStruct))) == NULL)
				{
					printf("error\n");
				}
				ProtocolUnpack(unpackedDataGrp, _buffer, tag); /* unpack username and grp name */
				printf("unpacked group tag: %d\n", unpackedDataGrp->m_tag);
				printf("unpacked group name: %s\n", unpackedDataGrp->m_groupName);
				printf("unpacked group username: %s\n", unpackedDataGrp->m_username);
				if(RemoveUserFromGroup(srvMng->m_groupManager, unpackedDataGrp->m_username, unpackedDataGrp->m_groupName) == USERMNG_SUCCESS)
				{
					dataToSend.m_reply = SUCCESS;
				}
				else
				{
					dataToSend.m_reply = FAILURE;
				}
				dataToSend.m_tag = GROUP_LEAVE_REPLY;
				replyPacketSize = ProtocolPack(&dataToSend, internalBuffer, GROUP_LEAVE_REPLY);
				SendToClient(internalBuffer, replyPacketSize, _clientID);
				break;
	}
}

SrvMng* SrvMngCreate()
{
	SrvMng* srvMng;
	
	if ((srvMng = (SrvMng*)malloc(sizeof(SrvMng))) == NULL)
	{
		return NULL;
	}
	
	if ((srvMng->m_userManager = UsrMngCreate()) == NULL)
	{
		return NULL;
	}
	
	if ((srvMng->m_groupManager = GrpMngCreate()) == NULL)
	{
		return NULL;
	}
	
	if ((srvMng->m_serverNet = CreateServer(NULL, GotMessage, NULL, NULL, PORT, IPV6_TYPE, IP, srvMng)) == NULL)
	{
		return NULL;
	}
	
	return srvMng;
}

void SrvMngDestroy(SrvMng** _srvMng)
{
	if (_srvMng == NULL || *_srvMng == NULL)
	{
		return;
	}
	
	UsrMngDestroy(&((*_srvMng)->m_userManager));
	GrpMngDestroy(&((*_srvMng)->m_groupManager));
	DestroyServer(&((*_srvMng)->m_serverNet));
	
	free(*_srvMng);
	*_srvMng = NULL;
}







	
	
	
	
	
	
	
	
void SrvMngActivate(SrvMng* _srvMng)
{
	RunServer(_srvMng->m_serverNet);
}


UsrMng* GetUsrMng(SrvMng* _srvMng)
{
	return _srvMng->m_userManager;
}

Server* GetSrvNet(SrvMng* _srvMng)
{
	return _srvMng->m_serverNet;
}

