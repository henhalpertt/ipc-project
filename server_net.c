#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
/*#include "../DS/sharedModules/dllg.h"*/
/*#include "../DS/sharedModules/dllIter.h"*/
/*#include "../DS/sharedModules/actions.h"*/
#include "genlist.h"
#include "listitr.h"
#include "listinternal.h"
#include "server_net.h"
#define PORT 2021
#define TRUE 1
#define FALSE 0
#define TIMEOUT 1000 /* timeout if server is not in use. in [sec] */
#define BUFFER_SIZE 100
#define FAIL -1
#define NUM_CLIENTS 1
#define MAX_CLIENT_CAPACITY 1020
#define IPV6_TYPE 0
#define IPV4_TYPE 1
#define ACTIVE 1

int gFlag = ACTIVE;
struct Server
{
	NewClient		m_userFuncNewclient;
	GotMsg 			m_userFuncGotMsg;
	FailMsg			m_userFuncfailMsg;
	CloseClient		m_userFuncCloseClient;
	List 			*m_clients;
	void* 			m_contxt; /* SRVR manager struct */
	size_t 			m_numclients;
	size_t 			m_maxClients;
	size_t 			m_port;
	int 			m_listenSocket;
	int 			m_ipType;
	int 			m_buffSize;
	char 			*m_ip;
};

typedef struct
{
	int m_socket;
}Socket;

static Socket* NewSocket(int _socket)
{
	Socket *newSock;
	newSock = (Socket*)malloc(sizeof(Socket));
	if(newSock == NULL)
	{
		return NULL;
	}
	newSock->m_socket = _socket;
	return newSock;
}

static List* CreateListOfSockets()
{
	return ListCreate();
}

static void PushSocketToList(List* _lst, Socket *_sock)
{
	if(_lst == NULL || _sock ==  NULL)
	{
		return;
	}
	ListPushTail(_lst, _sock);
}

static void NoBlocking(int socket)
{
	int flags;
	if (FAIL == (flags = fcntl(socket, F_GETFL)))
	{
		perror("error at fcntl. F_GETFL.");
		return;
	}
	if(FAIL == fcntl(socket, F_SETFL, flags | O_NONBLOCK))
	{
		perror("error at fcntl. F_SETFL.");
		return;
	}
}

static ServerSignal CheckParams(size_t _userPort, int _ipType, char *_ip, GotMsg _UserFuncGotMsg)
{
	if(_ipType > 1 ||_ip == NULL || _UserFuncGotMsg == NULL)
	{
		return PARAMS_FAIL;
	}
	return SERVER_OK;
}

Server *CreateServer(NewClient _UserFuncNewclient, GotMsg _UserFuncGotMsg,
									 FailMsg _UserFuncfailMsg,CloseClient _UserFuncCloseClient, 
									 						size_t _userPort, int _ipType, char *_ip,
									 						void* _contxt)
{
	List* lst;
	Server *_newServer;
	if(CheckParams(_userPort, _ipType, _ip, _UserFuncGotMsg) != SERVER_OK)
	{
		printf("invalid parameters");
		return NULL;
	}
	_newServer = (Server*)calloc(1, sizeof(Server));
	if(_newServer == NULL)
	{
		return NULL;
	}
	_newServer->m_userFuncNewclient = 	_UserFuncNewclient;
	_newServer->m_userFuncGotMsg = 	  	_UserFuncGotMsg;
	_newServer->m_userFuncfailMsg =   	_UserFuncfailMsg;
	_newServer->m_userFuncCloseClient = _UserFuncCloseClient;
	_newServer->m_maxClients = 			MAX_CLIENT_CAPACITY;
	_newServer->m_port = 				_userPort;
	_newServer->m_ipType = 				_ipType;
	_newServer->m_ip = 					_ip;
	_newServer->m_contxt = 				_contxt;
	lst = (List*)malloc(sizeof(List));
	if(lst == NULL)
	{
		return NULL;
	}
	lst = CreateListOfSockets();
	_newServer->m_clients = lst;
}

void DestroySocket(void* _data)
{
	free(_data);
}

void DestroyServer(Server** _server)
{
	if(_server == NULL || *_server == NULL)
	{
		return;
	}
	if((*_server)->m_numclients != 0)
	{
		ListDestroy(&(*_server)->m_clients, DestroySocket);
	}
	free(*_server);
	*_server = NULL;
}

/*void PrintValues(ListItr _begin, size_t _size)*/
/*{	*/
/*	int i;*/
/*	Socket *sock;*/
/*	ListItr next;*/
/*	printf("\nValues: ");*/
/*	for(i=0; i<_size; i++)*/
/*	{*/
/*		sock = (Socket*)ListItrGet(_begin);*/
/*		printf("%d ", sock->m_socket);*/
/*		_begin = ListItrNext(_begin);*/
/*	}*/
/*}*/

static int SetServer()
{
	int listenSD;
	int	rc;
	int on = 1;
	struct sockaddr_in addr;
	listenSD = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSD < 0)
	{
	  perror("socketINIT");
	  exit(-1);
	}
	rc = setsockopt(listenSD, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));
	if (rc < 0)
	{
	  perror("setsockopt");
	  close(listenSD);
	  exit(-1);
	}
	
	/* non-blocking */ 
	NoBlocking(listenSD); 
	/* init */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
/*	memcpy(&addr.sin_addr, &inaddr_any, sizeof(inaddr_any));*/
	addr.sin_port = htons(PORT);
	rc = bind(listenSD, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0)
	{
		perror("bind() failed");
		close(listenSD);
		exit(-1);
	}
	rc = listen(listenSD, 32);
	if (rc < 0)
	{
		perror("listen() failed");
		close(listenSD);
		exit(-1);
	}
	return listenSD;
}
/*	memset(&sin, 0, sizeof(sin));*/
/*	sin.sin_family = AF_INET;*/
/*	sin.sin_addr.s_addr = INADDR_ANY;*/
/*	sin.sin_port = htons(PORT);*/
	
static int SelectOperation(fd_set *_workingSet, fd_set *_masterSet, int _maxSD)
{
	struct timeval timeout;
	int rc;
	
	timeout.tv_sec  = TIMEOUT;
	timeout.tv_usec = 0;
	memcpy(_workingSet, _masterSet, sizeof(_masterSet));
/*	printf("server is waiting(select)\n");*/
	rc = select(_maxSD+1, _workingSet, NULL, NULL, &timeout);
	if (rc < 0)
	{
	 	perror("select::");
		return -1;
	}
	if (rc == 0)
	{
		printf(" select TIMEOUT. server is performing Shutdown\n");
		return -1;
	}
	return rc;
}

static void CleanMyList(List* _lst)
{
	size_t cnt;
	ListItr begin, end;
	begin = ListItrBegin(_lst);
	end = ListItrEnd(_lst);
	cnt=0;
	while(begin != end && ListItrNext(begin) != begin)
	{
		if(*((int*)ListItrGet(begin)) <= 0)
		{
			ListItrRemove(begin);
		}
		begin = ListItrNext(begin);
		cnt++;
	}
}

void RunServer(Server* _server)
{ 
	int i, len, rc;
	int descReady, endServer = FALSE;
	int listenSD, maxSD, newSD;
	int closeConnection;
	int clientCounter = 0;
	char buffer[BUFFER_SIZE];
	fd_set masterSet, workingSet;
	Socket *newSocket;
	
	listenSD = SetServer();
	FD_ZERO(&masterSet);
	maxSD = listenSD;
	FD_SET(listenSD, &masterSet);
	
	while(endServer == FALSE || gFlag != ACTIVE)
	{
	  rc = SelectOperation(&workingSet, &masterSet, maxSD);
	  if(rc < 0)
	  {
			break;
	  }
	  descReady = rc;
	  for (i=0; i <=maxSD && descReady>0; ++i)
	  {
		 if (FD_ISSET(i, &workingSet))
		 {
		    descReady -= 1;
		    if (i == listenSD)
		    {
/*		       printf("Listening socket is readable\n");*/
		          newSD = accept(listenSD, NULL, NULL); 
		          if (newSD < 0)
		          {
		             if (errno != EWOULDBLOCK)
		             {
		                perror("accept");
		                endServer = TRUE;
		                if(_server->m_userFuncfailMsg != NULL)
		       	 		{
		       	  	 		_server->m_userFuncfailMsg(ACCEPT_ERROR);
		       	 		}
		             }
		             break;
		          }
		          FD_SET(newSD, &masterSet);
		          /* add to list */
		          newSocket = NewSocket(newSD);
		          PushSocketToList(_server->m_clients, newSocket);
		          clientCounter++;
                  if(_server->m_userFuncNewclient != NULL)
		       	  {
		       	  	 _server->m_userFuncNewclient(clientCounter);
		       	  }
		          if (newSD > maxSD)
		          {
		          	maxSD = newSD;
		          }
		    }
		    else
		    {
		     	  /* not listening socket */
		       	  closeConnection = FALSE;
		          rc = recv(i, buffer, sizeof(buffer), 0);
		          if (rc < 0)
		          {
					if(_server->m_userFuncfailMsg != NULL)
		       	 	{
		       	  	 	_server->m_userFuncfailMsg(RECV_ERROR);
		       	 	}
		       	 	closeConnection = TRUE;
		          }
		          if (rc == 0)
		          {
/*		             printf("Connection closed\n");*/
		             closeConnection = TRUE;
		          }
		          len = rc;
		          if(buffer == NULL || sizeof(buffer) == 0)
		          {
		          	closeConnection = TRUE;
		          }
				  _server->m_userFuncGotMsg(buffer, rc, _server->m_contxt, i);
		          if (rc < 0)
		          {
		            perror("send");
		            if(_server->m_userFuncfailMsg != NULL)
		       	 	{
		       	  	 	_server->m_userFuncfailMsg(SEND_ERROR);
		       	 	}
		       	 	closeConnection = TRUE;
		          }
		       if (closeConnection)
		       {
		       	  printf("closing connection\n");
		          close(i);
		          FD_CLR(i, &masterSet);
  		          if(_server->m_userFuncCloseClient != NULL)
		       	  {
					  _server->m_userFuncCloseClient(clientCounter);
		       	  }
		          clientCounter--;
		          /* erase from list */
		          CleanMyList(_server->m_clients);
		          if (i == maxSD)
		          {
		             while (FD_ISSET(maxSD, &masterSet) == FALSE)
		             {
		             	maxSD -= 1;
		             }
		          }
		          break;
		       }
		    }
		 }
	  }
	}
	/* close possible active connections */ 
	for (i=0; i <= maxSD; ++i)
	{
	  if (FD_ISSET(i, &masterSet))
	  {
	  	close(i);
	  }
	}
	free(_server);
}

void SendToClient(char *_bufferMsg, int _messageSize, int _clientID)
{
	int rc;
	rc = send(_clientID, _bufferMsg, _messageSize, 0);
	if (rc < 0)
	{
		 perror("send");
		 return;
	}
}







