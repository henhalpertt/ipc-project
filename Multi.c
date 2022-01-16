#include "Multi.h"
#include <stdio.h>
#include <string.h>
#define SIZE 300
#define INVALID_PORT(X) ((X) != 2025)
int MultiCreate(char* _ipAddresOfGroup, int _groupPort, char* _groupName, char* _userName)
{
	char str[SIZE];
	char str2[SIZE];
	FILE* fileToSavePId;/*file*/
	char str[SIZE];/*file*/

	if (INVALID_PORT(_groupPort))
	{
		return ERROR;
	}
	/*file put in func*/
	sprintf(str, "multicastPidSavedFile/%s%s", _groupName, _username);
	fileToSavePId = fopen(str, "w");/*for each group a seperate file - improve - make the same file for all groups*/
	if (NULL == fileToSavePId)
	{
		printf("Problem with file opening\n");
		return 0;
	}
	fclose(fileToSavePId);
	/*endfile*/
	sprintf(str, "gnome-terminal -- ./sender.out '%s' '%d' '%s' '%s'", _ipAddresOfGroup, _groupPort, _groupName, _userName);
	system(str);
	sleep(2);
	sprintf(str2, "gnome-terminal -- ./listener.out '%s' '%d' '%s' '%s'", _ipAddresOfGroup, _groupPort, _groupName, _userName);
	system(str2);
	return OK;
}
int MultiDestroy(char* _groupName, char*_username)
{
	FILE* file;
	char str[SIZE - 100];
	char str2[SIZE];
	int lengthOfString;
	sprintf(str, "multicastPidSavedFile/%s%s", _groupName, _username);
	file = fopen(str, "r");
	if (NULL == file)
	{
		return ERROR;
	}
	fgets(str, SIZE - 100, file);
	lengthOfString = strlen(str);
	str[lengthOfString - 1] = '\0';
	sprintf(str2, "kill -9 %s", str);
	printf("%s", str2);
	system(str2);
	while (feof(file) == 0)
	{
		if (fgets(str, SIZE - 100, file) == 0)
		{
			break;
		}
		lengthOfString = strlen(str);
		str[lengthOfString - 1] = '\0';
		sprintf(str2, "kill -9 %s", str);
		printf("%s", str2);
		system(str2);
		fclose(file);
	}
	return OK;
}
