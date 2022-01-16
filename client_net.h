#ifndef __CLIENT_NET_H__
#define __CLIENT_NET_H__
/*#include <stdlib.h>*/
typedef struct Client Client;

typedef void(*ClientGotMsg)(char *buffer, size_t bufferSize, void *_contxt, int _sock);
typedef enum
{
	SEND_FAILURE=0,
	RECV_FAIL,
	CLIENT_INIT_FAILURE,
	CLIENT_INIT_SUCCESS,
	SEND_SUCCESS,
	CLIENT_CONN_FAILURE,
	CONN_LOST,
	CLIENT_OK,
	CLIENT_FAIL = -1
}ClientSignal;

Client* CreateClient(int _port, char *_ip);

void* DestroyClient(Client** _clnt);

ClientSignal ConnectToServer(Client *_clnt);

ClientSignal ReceiveFromServer(Client *_clnt, char *_buffer);

ClientSignal SendToServer(int sock, char *_buffer, int _sizeOfPacket);

ClientSignal GetSocket(Client *_clnt);

struct sockaddr_in GetSin(Client *_clnt);

#endif /*__CLIENT_NET_H__ */
