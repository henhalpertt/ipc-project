#include <stdio.h>
#define PORT 2021
#define IPV6_TYPE 0
#define IPV4_TYPE 1
#define IP "127.0.0.1"
#include "server_net.h"

/* server manager */

typedef struct 
{
	int m_tag;
}ServerManager;
extern gFlag;
static void DummyMsg(char *buffer, size_t _messageSize, void* _contxt, int _socket)
/* socket of client
	contxt is struct manager  */
{
	SendToClient(buffer, _messageSize, 5, _socket);
/*	manager = (Manager*)_contxt;*/
/*	printf("DummyMsg-For debug\n");*/
/*	manager->m_message = buffer;*/
	
	/* use protocol */
/*	checkIFSizeIsValid();*/
/*	protocolGetTag()*/
/*	GetTag  /* extract tag */
/*	Unpack(); --> struct of user/ group  */
				  /* kind of struct for the protocol */
	
	/* struct containing username & pass, check with user manager (insert or decline)*/
	/* send reply to protocol for packing; register reply  */
	/* reply: success / error using send() in server net */
/*	printf("test");*/
}

int main(void)
{
	ServerManager *serverMng;
	Server *server;
	server = CreateServer(NULL,
					 DummyMsg,  
					 NULL, 
					 NULL,
					 PORT,
					 IPV6_TYPE, 
					 IP,
					 (void*)serverMng
					 );
	RunServer(server);
	return 0;
}
