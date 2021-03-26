#include "main.h"
#include "messages.h"
#include "reciever.h"

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
	char received_msg[PACKET_TOTAL_SIZE];
	int recv_msg_size = 0;
	int i = 0;
	int total_msg_size = 0, total_written = 0, total_corrected = 0;
	int send_suceed, msg_written, msg_corrected;
	char summary_msg[SUMMARY_MSG];

	SOCKET Socket = INVALID_SOCKET;
	SOCKADDR_IN service;
	int bind_res, listen_res;
	struct sockaddr_in ChannelAddr;
	int ChannelAddrSize = sizeof(ChannelAddr);
	char channel_ip[INET_ADDRSTRLEN];
	char channel_port[INET_ADDRSTRLEN];
	HANDLE end_thread;
	u_long iMode = 1; //Non-Blocking socket
	/////////////////////***
	char file_buffer[ENCODED_SIZE];
	//unsigned char* buffer_PACKET_TOTAL_SIZE = malloc(sizeof(char) * PACKET_TOTAL_SIZE); // create place in stack for the message received 
	unsigned char rcv_buffer[ENCODED_SIZE];
	int curr_bytes_received;

	if (open_file(output_file_name) != 0)
		return(ERROR_CODE);
	memset(rcv_buffer, 0, ENCODED_SIZE);// Set all elements to 0
	//////////////////*****

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
		printf("socket failed with error: %ld\n", WSAGetLastError());
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

	//create thread to poll end
	end_thread = CreateThread(
		NULL,
		0,
		(LPTHREAD_START_ROUTINE)end_thread_action,
		NULL,
		0,
		NULL);

	if (end_thread == NULL) {
		printf("Error creating exit_thread.. exiting program.\n");
		closesocket(Socket);
		WSACleanup();
		return ERROR_CODE;
	}

	printf("Type \"End\" when done\n");

	//Non-blocking socket
	iResult = ioctlsocket(Socket, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
	{
		printf("ioctlsocket failed with error: %ld\n", iResult);
		CloseHandle(end_thread);
		closesocket(Socket);
		WSACleanup();
		return ERROR_CODE;
	}

	//set timer for the Socket
	static int timeout = TIMEOUT_MS;
	setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

	while (1)
	{
		if (WAIT_OBJECT_0 == WaitForSingleObject(end_thread, 0)) // the user entered End
			break;

		int recv_suceed = ReceiveString(received_msg, Socket, INFINITE, &ChannelAddr, &ChannelAddrSize, &recv_msg_size);
		if (recv_suceed == ERROR_CODE)
		{
			CloseHandle(end_thread);
			closesocket(Socket);
			WSACleanup();
			return ERROR_CODE;
		}
		else if (recv_suceed == NREADY) //no msg sent yet - this is a non-blocking port!
			continue;

		//for (int l = 0; l < recv_msg_size; l++)
		//{
		//	printf("%c", received_msg[l]);
		//}
		//printf("recv_msg_size: %d", recv_msg_size);


		inet_ntop(AF_INET, &ChannelAddr.sin_addr, channel_ip, sizeof(channel_ip));
		itoa(ntohs(ChannelAddr.sin_port), channel_port, 10);
		//printf("\nchannel IP is: %s\nchannel port is: %s\n", channel_ip, channel_port);
		////////////////////***********
		 
		msg_corrected = 0;
		msg_written = 0;
		error_handler(received_msg, recv_msg_size, &msg_written, &msg_corrected);


	//////////////********

		total_msg_size += recv_msg_size;
		total_written += msg_written;
		total_corrected += msg_corrected;
	}

	//end received


	//printf("received: %d bytes\nwrote: %d bytes\ndetected & corrected %d errors\n", recv_msg_size, total_written, total_corrected);
	construct_summary_msg(summary_msg, total_msg_size, total_written, total_corrected);
	printf("%s\n", summary_msg);

	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(channel_ip);
	service.sin_port = htons(atoi(channel_port));


	send_suceed = SendString(summary_msg, Socket, service, strlen(summary_msg) + 1);
	if (send_suceed == ERROR_CODE)
	{
		CloseHandle(end_thread);
		closesocket(Socket);
		WSACleanup();
		return ERROR_CODE;
	}

	//finish
	CloseHandle(end_thread);
	closesocket(Socket);
	WSACleanup();

	return 0;

}


void construct_summary_msg(char* summary_msg, int total_msg_size, int total_written, int total_corrected)
{
	char total_msg_size_str[MAX_INT_LEN];
	char total_written_str[MAX_INT_LEN];
	char total_corrected_str[MAX_INT_LEN];

	//convert ints to char*
	itoa(total_msg_size, total_msg_size_str, 10);
	itoa(total_written, total_written_str, 10);
	itoa(total_corrected, total_corrected_str, 10);

	//build msg
	strcpy(summary_msg, "received: ");
	strcat(summary_msg, total_msg_size_str);
	strcat(summary_msg, " bytes\nwritten: ");
	strcat(summary_msg, total_written_str);
	strcat(summary_msg, " bytes\ndetected & corrected: ");
	strcat(summary_msg, total_corrected_str);
	strcat(summary_msg, " errors");

}

DWORD end_thread_action(void* not_relevant)
{
	char str_from_user[PACKET_TOTAL_SIZE];

	while (1)
	{
		(void)scanf("%s", str_from_user);
		if (STRINGS_ARE_EQUAL(str_from_user, "End"))
			return 1;
	}
}