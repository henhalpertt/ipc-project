#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#define VALID_MAIN_ARGC 5
#define TRUE 1
#define FILE_SIZE 50
#define TO_SEND_SIZE 4096
#define FROM_STDIN_SIZE 200

int main(int argc, char *argv[])
{
	FILE* file;
	char str[FILE_SIZE];
	int sock;
	struct sockaddr_in sin;
	char* groupIp, *groupName, *username;
	int portOfTheGroup;
	char toSend[TO_SEND_SIZE];
	char fromSTDIN[FROM_STDIN_SIZE];
	int sentBytes;
	if (argc != VALID_MAIN_ARGC)
	{
		printf("main argc invalid\n");
		return 1;
	}
	groupIp = argv[1];
	portOfTheGroup = atoi(argv[2]);
	groupName = argv[3];
	username = argv[4];
	/*save pid into file*/
	sprintf(str, "multicastPidSavedFile/%s%s", groupName, username);
	file = fopen(str, "a");
	if (NULL == file)
	{
		return;
	}
	fprintf(file, "%d\n", getpid());
	fclose(file);
	/*end save pid into file*/
	/*init*/
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		perror("FAIL OPEN SOCKET\n");
		exit(0);
	}
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(groupIp);
	sin.sin_port = htons(portOfTheGroup);
	/*end init*/
/*	UIWelcomeToGroup(groupName);*/
	printf("I am the sender\n");
	while (TRUE)
	{
		fgets(fromSTDIN, TO_SEND_SIZE, stdin);
		sprintf(toSend, "%s:%s", username, fromSTDIN);
		/*send*/
		sentBytes = sendto(sock, toSend, strlen(toSend) + 1, 0, (struct sockaddr*)&sin, sizeof(sin));
		if (sentBytes < 0)
		{
			perror("FAIL SEND");
			exit(0);
		}
		/*end send*/
	}
	return 0;
}
