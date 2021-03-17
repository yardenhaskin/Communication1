#include "main.h"
#include "messages.h"

int main(int argc, char* argv[])
{

	//check number of arguments
	if (argc != 6)
	{
		printf("There are too many or not enough arguments");
		return ERROR_CODE;
	}

	char* channel_port = argv[1];
	char* receiver_ip = argv[2];
	char* receiver_port = argv[3];
	char* single_bit_err_prob = argv[4];
	char* random_seed = argv[5];
	char* received_msg[MAX_MSG_LEN];

	SOCKET Socket = INVALID_SOCKET;
	SOCKADDR_IN service;
	int bind_res, listen_res;
	struct sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);


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
	service.sin_port = htons(atoi(channel_port));

	bind_res = bind(Socket, (SOCKADDR*)&service, sizeof(service));
	if (bind_res == SOCKET_ERROR)
	{
		printf("bind() had failed. Error Code %ld. Closing program\n", WSAGetLastError());
		closesocket(Socket);
		return ERROR_CODE;
	}



	int recv_suceed = ReceiveString(received_msg, Socket, INFINITE, &SenderAddr, &SenderAddrSize);
	if (recv_suceed == ERROR_CODE)
	{
		return ERROR_CODE;
	}

	printf(received_msg);

	//finish
	closesocket(Socket);
	WSACleanup();

	return 0;

}