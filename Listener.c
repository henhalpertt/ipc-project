#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <linux/types.h>
#include <time.h>
#include <unistd.h>
#include <netinet/ip.h>
#define VALID_MAIN_ARGC 5
#define TRUE 1
#define FILE_SIZE 100
#define TO_RECIEVE_SIZE 4096

int main(int argc, char *argv[])
{
	FILE* file;
	char str[FILE_SIZE];
	int sock;
    struct sockaddr_in sin;
    struct ip_mreq mreq;
    char* iPofTheGroup, *groupName, *username;
    int portOfTheGroup;
    unsigned int yes;
    if(argc != VALID_MAIN_ARGC)
    {
		printf("main argc invalid\n");
        return 1;
    }
    iPofTheGroup = argv[1];
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

		perror("socket failure");
		sleep(10);
		exit(0);
	}
	/*init socket*/
	yes = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) < 0)
	{
		perror("FAIL SOCKET");
		sleep(10);
		exit(0);
	}
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(iPofTheGroup);
	sin.sin_port = htons(portOfTheGroup);
	mreq.imr_multiaddr.s_addr = inet_addr(iPofTheGroup);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)) < 0)
	{
		perror("FAIL SOCKET");
		sleep(10);
		exit(0);
	}

	/*end init socket*/
	/*BIND*/
	if (bind(sock, (struct sockaddr*)&sin, sizeof(sin)) < 0)
	{
		perror("FAIL");
		sleep(10);
		exit(0);
	}
	/*END BIND*/
	/*end init*/
    while(TRUE)
    {
		socklen_t sinLength = sizeof(sin);
		char toRecieve[TO_RECIEVE_SIZE];
		int readBytes;
		readBytes = recvfrom(sock, toRecieve, TO_RECIEVE_SIZE, 0, (struct sockaddr*)&sin, &sinLength);
		if (readBytes < 0)
		{
			perror("recieve failure");
		}
		printf("%s\n", toRecieve);
    }
    return 0;
}
