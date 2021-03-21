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
	SOCKET Socket = INVALID_SOCKET;
	int i = 0;
	int bytes_to_send = 0;	
	SOCKADDR_IN RecvAddr;
	SOCKADDR_IN service;
	int bind_res, listen_res;
	char summary_msg[SUMMARY_MSG];
	struct sockaddr_in ChannelAddr;
	int ChannelAddrSize = sizeof(ChannelAddr);
	int recv_msg_size = 0;



	//init WinSock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printf("Error at WSAStartup()\n");
		return ERROR_CODE;
	}

	//Create Socket
	Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (Socket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return ERROR_CODE;
	}

	// Bind socket
	char* sender_port = "635";
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(MY_ADDRESS);
	service.sin_port = htons(atoi(sender_port));

	bind_res = bind(Socket, (SOCKADDR*)&service, sizeof(service));
	if (bind_res == SOCKET_ERROR)
	{
		printf("bind() had failed. Error Code %ld. Closing program\n", WSAGetLastError());
		closesocket(Socket);
		return ERROR_CODE;
	}

	//init Recv addr
	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(atoi(channel_port));
	RecvAddr.sin_addr.s_addr = inet_addr(channel_ip);

	//FIXME: replace with real file content
	char* StrBuffer = NULL;
	StrBuffer = "This is a test, make sure everything is working well. This text should be long eonugh to be forwarded in more than one chunk";

	while (1) //FIXME: change to file read until EOF
	{
		if (0 == (strlen(StrBuffer) + 1) / MAX_MSG_LEN) //FIXME: change to size of bytes you read from file
			bytes_to_send = (strlen(StrBuffer) + 1) % MAX_MSG_LEN;
		else
			bytes_to_send = MAX_MSG_LEN;
		//printf("bytes_to_send: %d\n", bytes_to_send);
		int send_suceed = SendString(StrBuffer, Socket, RecvAddr, bytes_to_send);
		if (send_suceed == ERROR_CODE)
		{
			return ERROR_CODE;
		}

		if (0 == ((strlen(StrBuffer) + 1) / MAX_MSG_LEN)) //FIXME: remove after adding the file reading func
			break;

		StrBuffer += bytes_to_send;

	}

	//wait for summary msg
	int recv_suceed = ReceiveString(summary_msg, Socket, INFINITE, &ChannelAddr, &ChannelAddrSize, &recv_msg_size);
	if (recv_suceed == ERROR_CODE)
	{
		return ERROR_CODE;
	}

	printf("%s\n", summary_msg);

	//finish
	closesocket(Socket);
	WSACleanup();

	return 0;

}