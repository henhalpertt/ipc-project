#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "client_manager.h"
#include "protocol.h"
#define NEWLINE "\n"
#define NOT_REGISTERED 0
#define NOT_LOGGED_IN 0
#define OPEN_CONN 0
#define REPLY_YES 1
#define REPLY_NO 0
typedef enum 
{
	REGISTER=1,
	LOGIN=2,
	EXIT=3,
	CREATE_GRP=4,
	SEE_GRPS=5,
	JOIN_GRP=6,
	LEAVE_GRP=7,
	LOGOUT=8
}UserActions;

int CheckReply(char *action ,char reply)
{
	if(reply == REPLY_YES)
	{
		printf("%s Success\n", action);
		return REPLY_YES;
	}
	printf("%s Failed. Please check your input\n", action);
	return -1;
}

void LoginPage(Client *_clnt, char *_userName, char *_pass)
{
	char grpName[30];
	int returnToMainPage;
	int action;
	char reply;
	returnToMainPage = 0;
	while(1)
	{
		printf("Select Your action: \n 4 - Create a Group \n 5 - See existing Groups \n 6 - Join Group\n 7 - Leave Group \n 8 - Log Out");
		scanf("%d", &action);
		switch (action)
		{
			case CREATE_GRP:
					/* enter group name, must be unique  */
					printf("Select a name for your group: ");
					scanf("%s", grpName);
					printf("grp name = %s", grpName);
					/* for groups */
					PackAndSendViaClientNET(_userName, grpName, GROUP_CREATE_REQUEST, _clnt);
					reply = ReceiveViaClientNET(_clnt);
					CheckReply("Group Create" , reply);
/*					if(CheckReply("Group Create" , reply) == 1)*/
/*					{*/
/*						OpenNewWindow(_clnt, _userName, _pass, grpName);*/
/*					}*/
					break;
					
			case SEE_GRPS:
					PackAndSendViaClientNET(_userName, grpName, GROUP_ALL_AVAILABLE_REQUEST, _clnt);
					reply = ReceiveViaClientNET(_clnt);
					CheckReply("Group Availability" , reply);
					break;
					
			case JOIN_GRP:
					/* enter group name, must be unique  */
					printf("Enter Group Name: ");
					scanf("%s", grpName);
					PackAndSendViaClientNET(_userName, grpName, GROUP_JOIN_REQUEST, _clnt);
					reply = ReceiveViaClientNET(_clnt);
					CheckReply("Joining Group" , reply);
/*					if(CheckReply("Joining Group" , reply) == 1)*/
/*					{*/
/*						OpenNewWindow(_clnt, _userName, _pass, grpName);*/
/*					}*/
					break;
			case LEAVE_GRP:
					printf("Enter Group Name: ");
					scanf("%s", grpName);
					PackAndSendViaClientNET(_userName, grpName, GROUP_LEAVE_REQUEST, _clnt);
					break;
					
			case LOGOUT:
					PackAndSendViaClientNET(_userName, grpName, LOGOUT_REQUEST, _clnt);
					reply = ReceiveViaClientNET(_clnt);
					if(CheckReply("Logging Out" , reply) == 1)
					{
						returnToMainPage = 1;
					}
					return;
		}
	}
}

int main(void)
{
	int action;
	char userName[30];
	char pass[30];
	int exitFlg = NOT_REGISTERED,	loginFlg = NOT_LOGGED_IN,	registFlg = OPEN_CONN;
	int exitStatus;
	Client *clnt;
	char reply;
	/* Load a file w/ users and passwords */
	/* ask for an action: [1 - register, 2 - LOGIN, 3 - EXIT ]*/
	clnt = ConnectToClientNET();
	while(!exitFlg)
	{
		printf("Would you like to: \n 1 - Register \n 2 - Login \n 3 - EXIT\n");
		scanf("%d", &action);
		switch (action)
		{
			case REGISTER:
				printf("registration\n");
				printf("Enter Your UserName: ");
				scanf("%s", userName);
				NEWLINE;
				printf("Enter Your Pass: ");
				scanf("%s", pass);
				NEWLINE;
				printf("user - %s\n", userName);
				printf("password - %s\n", pass);
				PackAndSendViaClientNET(userName, pass, REGISTER_REQUEST, clnt);
				reply = ReceiveViaClientNET(clnt);
				CheckReply("Registration", reply);
				registFlg = 1;
				break; 
			case LOGIN:
				printf("Login\n");
				printf("Enter Your UserName: ");
				scanf("%s", userName);
				NEWLINE;
				printf("Enter Your Pass: ");
				scanf("%s", pass);
				PackAndSendViaClientNET(userName, pass, LOGIN_REQUEST, clnt);
				reply = ReceiveViaClientNET(clnt);
				if(CheckReply("Login", reply) == 1)
				{
					LoginPage(clnt, userName, pass);
				}
				loginFlg = 1;
				break;
			case EXIT:
				printf("Client is Disconnected from server...\n");
				ClientDisconnected(clnt);
				exitFlg = 1;
				sleep(1);
				break;
			default:
				printf("please select a valid action");
		}
	}
	return 0;
}

