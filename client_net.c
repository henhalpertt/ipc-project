#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h> 
#include <arpa/inet.h>
#include "client_net.h"
#include "genlist.h"
#include "listitr.h"
/*#include "../DS/sharedModules/actions.h"*/
#define STR_SIZE 100
#define BUFF_SIZE 4096
#define DATA_CAP 110
#define PORT 2021
#define NUM_CLIENTS 1
#define DISCONNECTED 0
#define CONNECTED 1
/* in makefile later: gcc -ansi -pedantic main.c client_manager.c client_net.c protocol.c server_net.c srv_mng.c user.c usr_mng.c client_net.c -L. -l:libdyn.so -o main.out*/ 

struct Client
{
	size_t m_status;
	size_t m_clntID;
	int m_mySocket;
	struct sockaddr_in m_sin;
	ClientGotMsg m_clientMsg;
	int m_port;
	char *m_ip;
};

static int InitTCP()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("socket failed");
		return -1;
	}
	return sock;
}

Client* CreateClient(int _port, char *_ip)
{
	Client *newclnt;
	struct sockaddr_in sin;
	int sock;
	sock = InitTCP();
	
	newclnt = (Client*)malloc(sizeof(Client));
	if(newclnt == NULL)
	{
		return NULL;
	}
	newclnt->m_status = DISCONNECTED;
	newclnt->m_mySocket = sock;
	newclnt->m_sin = sin;
	newclnt->m_port = _port;
	newclnt->m_ip = _ip;
	return newclnt;
}
 
ClientSignal ConnectToServer(Client *_clnt)
{
	memset(&(_clnt->m_sin), 0 ,sizeof(_clnt->m_sin));
	(_clnt->m_sin).sin_family = AF_INET;
	(_clnt->m_sin).sin_addr.s_addr = inet_addr(_clnt->m_ip);
	(_clnt->m_sin).sin_port = htons(_clnt->m_port);
	if (connect((_clnt->m_mySocket), (struct sockaddr *) &(_clnt->m_sin), sizeof(_clnt->m_sin)) < 0)
	{
		perror("connection failed");
		return CLIENT_CONN_FAILURE;
	}
	return CLIENT_OK;
}

ClientSignal SendToServer(int sock, char *_buffer, int _sizeOfPacket)
{
	int sent_bytes;
	sent_bytes = send(sock, _buffer, _sizeOfPacket, MSG_NOSIGNAL);
	if (sent_bytes < 0)
	{
		perror("CLNT: send failed");
		return SEND_FAILURE;
	}
	return CLIENT_OK;
}

ClientSignal ReceiveFromServer(Client *_clnt, char *_buffer)
{
	int read_bytes = recv(_clnt->m_mySocket, _buffer, 2048, 0);
	if (read_bytes == 0)
	{
		return CONN_LOST;
	}
	else if (read_bytes < 0)
	{
		perror("CLNT: recv failed");
		return RECV_FAIL;
	}
	else if (read_bytes == 0)
	{
		printf("connection closed");
	}
	return CLIENT_OK;
}
int GetSocket(Client *_clnt)
{
	if(NULL == _clnt || _clnt->m_mySocket < 1)
	{
		return -1;
	}
	return _clnt->m_mySocket;
}

void* DestroyClient(Client** _clnt)
{
	if(_clnt == NULL || *_clnt == NULL)
	{
		return;
	}
	if((*_clnt)->m_mySocket > 0)
	{
		close((*_clnt)->m_mySocket);
	}
	free(*_clnt);
	*_clnt = NULL;
}



/* END */


