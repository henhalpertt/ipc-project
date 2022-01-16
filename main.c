#include <stdio.h>
#include <string.h>
#include "client_manager.h"
#define NEWLINE printf("\n")
#define REGISTER 1
#define LOGIN 2
#define EXIT 3 
/* Insert all prints to UI module */ 

int main(int argc, char *argv[])
{
	/* 3 way handshake - TCP */
	ConnectClientNET();
/*	ConnectServerNET();*/
	/* Load a file w/ users and passwords */
	int action;
	char userName[30];
	char pass[30];
	/* ask for an action: [1 - register, 2 - LOGIN, 3 - EXIT ]*/
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
/*			PackAndSendViaClientNET(userName, pass, REGISTER);*/
			break;
		case LOGIN:
			printf("Login");
			break;
		case EXIT:
			printf("Exit");
			break;
		default:
			printf("please select a valid action");
	}
	return 0;
}
