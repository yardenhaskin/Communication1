#include "main.h"
#include "messages.h"

int main(int argc, char* argv[])
{

	//check number of arguments
	if (argc != 4)
	{
		printf("There are too many or not enough arguments");
		return ERROR_CODE;
	}

	char* channel_ip = argv[1];
	char* channel_port = argv[2];
	char* file_name = argv[3];
	SOCKET SendSocket = INVALID_SOCKET;

	//init WinSock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printf("Error at WSAStartup()\n");
		return ERROR_CODE;
	}

	//Create Socket for sending data
	SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (SendSocket == INVALID_SOCKET) {
		wprintf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return ERROR_CODE;
	}

	SOCKADDR_IN RecvAddr;

	//init Recv addr
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(atoi(channel_port));
	RecvAddr.sin_addr.s_addr = inet_addr(channel_ip);

	//FIXME: replace with real file content
	char* StrBuffer = NULL;
	StrBuffer = "This is a test, make sure everything is working well. This text should be long eonugh to be forwarded in more than one chunk";

	int send_suceed = SendString(StrBuffer, SendSocket, RecvAddr);
	if (send_suceed == ERROR_CODE)
	{
		return ERROR_CODE;
	}

	//finish
	closesocket(SendSocket);
	WSACleanup();

	return 0;

}