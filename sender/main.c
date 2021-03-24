#include "main.h"
#include "messages.h"
#include "sender.h"

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
	/////////////////////////////////////////////////////
	int iResult = 0;
	int bytes_sent;
	int bytesLeftToSend;
	int totalBytesSent = 0;
	int end_of_file = 0;
	int next_bit;
	char* ip;
	char receive_buffer[PACKET_TOTAL_SIZE];

	int data[11];
	int data_after_hamming[15];
	int bits_from_file;
	int bytes_to_file;
	int data_counter;
	int ready_bits;
	int ready_bits_buffer[PACKET_TOTAL_SIZE * BYTE_SIZE * INT_SIZE_IN_BYTES];
	unsigned char send_buffer[PACKET_TOTAL_SIZE];

	///////////////////////////////////////////////


	//init WinSock
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
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
	//////////////////*****from here
	if (open_file(file_name) != 0)
		return(ERROR_CODE);
	// sender functionality: reading from file -> creating a message -> sending messages
	while (true) {
		//initialize "ready_bits_buffer(array of zeros
		memset(ready_bits_buffer, 0, (PACKET_TOTAL_SIZE * BYTE_SIZE * INT_SIZE_IN_BYTES));
		if (feof(fp)) {
			break;
		}

		ready_bits = 0;
		data_counter = 0;
		bits_from_file = 0;

		//read file and creating the hamming code, put it in a buffer ( array of ints)
		while (bits_from_file < (PACKET_DATA_SIZE * BYTE_SIZE)) { //iteration to read bits from file to fill a message

			next_bit = readBit();
			//if EOF continue to sending the messages
			if (next_bit == -1) {
				end_of_file = 1;
				break;
			}

			data[data_counter] = next_bit;
			if (data_counter == BEFORE_HAMMING - 1) {///means end of the array
				create_hamming(data, data_after_hamming); // creating hammig code 
				concatenate(ready_bits_buffer, data_after_hamming, AFTER_HAMMING, ready_bits); // adding it to buffer
				ready_bits += AFTER_HAMMING;
				data_counter = 0;
			}
			else
				data_counter++;

			bits_from_file++;
		}
		intArrayToUnsignedChar(ready_bits_buffer, send_buffer, PACKET_TOTAL_SIZE);

		//finished reading and prepering data for a single packet
		//now we'll send it
		bytes_to_send = bits_from_file * 15 / 88;
		int send_suceed = SendString(send_buffer, Socket, RecvAddr, bytes_to_send);
		if (send_suceed == ERROR_CODE)
		{
			return ERROR_CODE;
		}

		//		totalBytesSent = totalBytesSent + FRAME_SIZE;	
	}
	///////



	//////////////////*****
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