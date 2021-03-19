#include "main.h"
#include "messages.h"

int main(int argc, char* argv[])
{

	//check number of arguments
	if (argc != 3)
	{
		printf("There are too many or not enough arguments");
		return ERROR_CODE;
	}

	char* receiver_port = argv[1];
	char* output_file_name = argv[2];
	char received_msg[MAX_MSG_LEN];

	SOCKET Socket = INVALID_SOCKET;
	SOCKADDR_IN service;
	int bind_res, listen_res;
	struct sockaddr_in ChannelAddr;
	int ChannelAddrSize = sizeof(ChannelAddr);


	//init WinSock
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printf("Error at WSAStartup()\n");
		return ERROR_CODE;
	}

	//Create Socket for receiving data
	Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (Socket == INVALID_SOCKET) {
		wprintf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return ERROR_CODE;
	}

	// Bind socket
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(MY_ADDRESS);
	service.sin_port = htons(atoi(receiver_port));

	bind_res = bind(Socket, (SOCKADDR*)&service, sizeof(service));
	if (bind_res == SOCKET_ERROR)
	{
		printf("bind() had failed. Error Code %ld. Closing program\n", WSAGetLastError());
		closesocket(Socket);
		return ERROR_CODE;
	}

	int recv_suceed = ReceiveString(received_msg, Socket, INFINITE, &ChannelAddr, &ChannelAddrSize);
	if (recv_suceed == ERROR_CODE)
	{
		return ERROR_CODE;
	}

	printf(received_msg);

	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ChannelAddr.sin_addr, ip, sizeof(ip));
	char port[INET_ADDRSTRLEN];
	itoa(ntohs(ChannelAddr.sin_port), port, 10);
	printf("\nchannel IP is: %s\nchannel port is: %s\n", ip, port);


	//finish
	closesocket(Socket);
	WSACleanup();

	return 0;

}