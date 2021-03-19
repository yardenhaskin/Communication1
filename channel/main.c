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

	SOCKET Socket = INVALID_SOCKET;
	SOCKADDR_IN service;
	int bind_res, listen_res;
	struct sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);
	SOCKADDR_IN ReceiverAddr;





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

	int recv_suceed = ReceiveString(received_msg, Socket, INFINITE, &SenderAddr, &SenderAddrSize);
	if (recv_suceed == ERROR_CODE)
	{
		return ERROR_CODE;
	}

	printf("msg is: %s\n", received_msg);

	//Noise the channel
	srand(random_seed);
	generate_noise(received_msg, noise_msg, single_bit_err_prob);
	printf("noised msg is: %s\n", noise_msg);



	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_addr.s_addr = inet_addr(receiver_ip);
	ReceiverAddr.sin_port = htons(atoi(receiver_port));

	int send_suceed = SendString(received_msg, Socket, ReceiverAddr);
	if (send_suceed == ERROR_CODE)
	{
		return ERROR_CODE;
	}


	//finish
	closesocket(Socket);
	WSACleanup();

	return 0;

}


int generate_noise(unsigned char* received_msg, unsigned char* noise_msg, int single_bit_err_prob)
{
	double p = single_bit_err_prob / pow(2, 16);
	strcpy(noise_msg, received_msg);

	int i;
	for (i = 0; i < strlen(received_msg); i++)
	{
		generate_noise_for_byte(received_msg, noise_msg, p, i);
	}

}


void generate_noise_for_byte(unsigned char* received_msg, unsigned char* noise_msg, double p, int byte_index)
{
	int i;
	unsigned char mask = 0x00, temp_mask = 0x00, result;
	int index;
	int num_of_flips = get_num_of_flips(8, p);
	if (num_of_flips != 0)
		printf("num of flips in Byte %d is: %d\n", byte_index, num_of_flips);

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

		printf("flip bit is: %d\n", index);

		//flip bit
		 result = noise_msg[byte_index] ^ mask;
		 noise_msg[byte_index] = result;
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