#ifndef __CLIENT_MNG_H__
#define __CLIENT_MNG_H__ 
#include "client_net.h"
#include "protocol.h"
typedef enum
{
	CLNT_MNG_CONN_SUCCESS=0,
	CLNT_MNG_CONN_FAIL,
	CLNT_MNG_SEND_OK,
	CLNT_MNG_SEND_FAIL,
	CLNT_MNG_RECV_OK,
	CLNT_MNG_RECV_FAIL,
	CLNT_MNG_DESTROY_OK,
	CLNT_MNG_CLNT_UNINITIALIZED
}ClientManagerSignal;


Client* ConnectToClientNET();

ClientManagerSignal PackAndSendViaClientNET(char *_userName, char *_pass, Tag _action, Client *_myClient);

char ReceiveViaClientNET(Client *_clnt);

void ClientDisconnected(Client *_clnt);


#endif /*__CLIENT_MNG_H__ */
