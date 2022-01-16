#ifndef __GEN_SERVER_H__
#define __GEN_SERVER_H__
typedef enum
{
	SERVER_OK = 0,
	PARAMS_FAIL
}ServerSignal;

typedef enum
{
	ACCEPT_ERROR = 0,
	RECV_ERROR,
	SEND_ERROR
}ERRORSForAPP;

typedef void(*NewClient)(int);
typedef void(*GotMsg)(char *buffer, size_t bufferSize, void *_contxt, int _sock); /* contxt will hold struct of server manager */
typedef void(*FailMsg)(ERRORSForAPP);
typedef void(*CloseClient)(int _clientID);
typedef struct Server Server;

Server *CreateServer(NewClient _UserFuncNewclient,
					 GotMsg _UserFuncGotMsg,  
					 FailMsg _UserFuncfailMsg, 
					 CloseClient _UserFuncCloseClient,
					 size_t _userPort,
					 int _ipType,
					 char *_ip, 
					 void* _contxt);

void DestroyServer(Server** _server);

void RunServer(Server* _server);

void SendToClient(char *_bufferMsg, int _messageSize, int _clientID);

#endif /*__GEN_SERVER_H__ */
