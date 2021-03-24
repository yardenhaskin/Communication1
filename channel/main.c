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
	int single_bit_err_prob = atoi(argv[4]);
	int random_seed = atoi(argv[5]);
	char received_msg[MAX_MSG_LEN];
	char noise_msg[MAX_MSG_LEN];
	int recv_msg_size = 0;
	int total_msg_size = 0;
	int bits_flipped = 0;
	int i = 0;
	int recv_suceed;
	int send_suceed;
	int first_recv = 1;

	SOCKET Socket = INVALID_SOCKET;
	SOCKADDR_IN service;
	int bind_res, listen_res;
	struct sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);
	SOCKADDR_IN ReceiverAddr;
	char sender_ip[INET_ADDRSTRLEN];
	char sender_port[INET_ADDRSTRLEN];
	char sender_or_receiver_ip[INET_ADDRSTRLEN];
	char sender_or_receiver_port[INET_ADDRSTRLEN];

	srand(random_seed);


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
	service.sin_port = htons(atoi(channel_port));

	bind_res = bind(Socket, (SOCKADDR*)&service, sizeof(service));
	if (bind_res == SOCKET_ERROR)
	{
		printf("bind() had failed. Error Code %ld. Closing program\n", WSAGetLastError());
		closesocket(Socket);
		return ERROR_CODE;
	}

	while(1)
	{
		recv_suceed = ReceiveString(received_msg, Socket, INFINITE, &SenderAddr, &SenderAddrSize, &recv_msg_size);
		if (recv_suceed == ERROR_CODE)
		{
			return ERROR_CODE;
		}

		if (first_recv)		//save sender's ip and port
		{
			first_recv = 0;
			inet_ntop(AF_INET, &SenderAddr.sin_addr, sender_ip, sizeof(sender_ip));
			itoa(ntohs(SenderAddr.sin_port), sender_port, 10);
		}
		else
		{
			inet_ntop(AF_INET, &SenderAddr.sin_addr, sender_or_receiver_ip, sizeof(sender_or_receiver_ip));
			itoa(ntohs(SenderAddr.sin_port), sender_or_receiver_port, 10);
		}

		if (STRINGS_ARE_EQUAL(sender_or_receiver_ip, receiver_ip) &
			STRINGS_ARE_EQUAL(sender_or_receiver_port, receiver_port)) // msg is from reciever - send back to sender
			break;

		//printf("msg is: %s\n", received_msg);
		//printf("msg size is: %d\n", recv_msg_size);

		//Noise the channel

		generate_noise(received_msg, single_bit_err_prob, recv_msg_size, &bits_flipped);
		//printf("noised msg is: %s\n", noise_msg);



		//send noisy msg to the receiver
		ReceiverAddr.sin_family = AF_INET;
		ReceiverAddr.sin_addr.s_addr = inet_addr(receiver_ip);
		ReceiverAddr.sin_port = htons(atoi(receiver_port));

		send_suceed = SendString(received_msg, Socket, ReceiverAddr, recv_msg_size);
		if (send_suceed == ERROR_CODE)
		{
			return ERROR_CODE;
		}

		total_msg_size += recv_msg_size;
	}

	//send back to sender and finish
	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_addr.s_addr = inet_addr(sender_ip);
	ReceiverAddr.sin_port = htons(atoi(sender_port));

	send_suceed = SendString(received_msg, Socket, ReceiverAddr, recv_msg_size);
	if (send_suceed == ERROR_CODE)
	{
		return ERROR_CODE;
	}
	printf("sender: %s\n", sender_ip);
	printf("reveiver: %s\n", sender_or_receiver_ip);
	printf("%d bytes, flliped %d bits\n", total_msg_size, bits_flipped);

	//finish
	closesocket(Socket);
	WSACleanup();

	return 0;

}


int generate_noise(unsigned char* received_msg, int single_bit_err_prob, int recv_msg_size, int* bits_flipped)
{
	double p = single_bit_err_prob / pow(2, 16);

	int i;
	for (i = 0; i < recv_msg_size; i++)
	{
		generate_noise_for_byte(received_msg, p, i, bits_flipped);
	}

}


void generate_noise_for_byte(unsigned char* received_msg, double p, int byte_index, int* bits_flipped)
{
	int i;
	unsigned char mask = 0x00, temp_mask = 0x00, result;
	int index;
	int num_of_flips = get_num_of_flips(8, p);
	if (num_of_flips != 0)
		printf("num of flips in Byte %d is: %d\n", byte_index, num_of_flips); //FIXME: remove after debug

	*bits_flipped += num_of_flips;

	for (i = 1; i <= num_of_flips; i++)
	{
		while (mask == (mask || temp_mask)) //make sure different bit flips
		{
			temp_mask = 0x01;
			//get random bit
			index = rand() % 7;
			temp_mask = temp_mask << index;
		}

		mask = temp_mask;

		printf("flip bit is: %d\n\n", index); //FIXME: remove after debug

		//flip bit
		 result = received_msg[byte_index] ^ mask;
		 received_msg[byte_index] = result;
	}
}

int get_num_of_flips(int num_of_bits, double p)
{

	double threshold = (double)rand() / RAND_MAX;
	int i;
	int choose_result;
	double no_flip_prob;
	double flip_prob;
	double probability_of_i_bit_flips = 0;
	for (i = 0; i <= num_of_bits; i++)
	{
		choose_result = choose(num_of_bits, i);
		no_flip_prob = pow((1 - p), (num_of_bits - i));
		flip_prob = pow(p, i);
		probability_of_i_bit_flips += choose_result * no_flip_prob * flip_prob;
		if (threshold < probability_of_i_bit_flips)
			return i;
	}
}

int choose(int n, int k)
{
	if (k == 0)
	{
		return 1;
	}
	return ((n * choose(n - 1, k - 1)) / k);
}