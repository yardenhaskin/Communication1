#include "sender.h"

size_t readOneByte() {
	size_t result;
	result = fread(&readed_byte, 1, 1, fp);
	return result > 0 ? 1 : 0;
}
int open_file(char* file_name) {
	fp = fopen(file_name, "rb");
	if (fp == NULL) {
		fprintf(stderr, "couldn't open file. please try again");
		exit(-1);
	}
}
//read single bit from file, return -1 if EOF
int readBit() {
	int result;
	reading_mask >>= 1;
	// if end of byte or first call-> reads 8 bits to buffer and initialize the mask
	if (reading_mask == 0) {
		if (!readOneByte())
		{
			return END_OF_FILE; //EOF is -1
		}
		reading_mask = 1 << (7);
	}
	//if there is a "1" in the result -> outpout 1
	result = ((reading_mask & readed_byte) > 0);
	return(result);
}
int findRedundentC1(int* data) {
	int c1;
	c1 = data[0] ^ data[1] ^ data[3] ^ data[4] ^ data[6] ^ data[8] ^ data[10];
	return c1;
}
int findRedundentC2(int* data) {
	int c2;
	c2 = data[0] ^ data[2] ^ data[3] ^ data[5] ^ data[6] ^ data[9] ^ data[10];
	return c2;
}
int findRedundentC3(int* data) {
	int c3;
	c3 = data[1] ^ data[2] ^ data[3] ^ data[7] ^ data[8] ^ data[9] ^ data[10];
	return c3;
}
int findRedundentC4(int* data) {
	int c4;
	c4 = data[4] ^ data[5] ^ data[6] ^ data[7] ^ data[8] ^ data[9] ^ data[10];
	return c4;
}
//creating 15b from the 11b input data
void create_hamming(int* data, int* data_after_hamming) {

	data_after_hamming[0] = findRedundentC1(data);
	data_after_hamming[1] = findRedundentC2(data);
	data_after_hamming[2] = data[0];
	data_after_hamming[3] = findRedundentC3(data);
	data_after_hamming[4] = data[1];
	data_after_hamming[5] = data[2];
	data_after_hamming[6] = data[3];
	data_after_hamming[7] = findRedundentC4(data);
	data_after_hamming[8] = data[4];
	data_after_hamming[9] = data[5];
	data_after_hamming[10] = data[6];
	data_after_hamming[11] = data[7];
	data_after_hamming[12] = data[8];
	data_after_hamming[13] = data[9];
	data_after_hamming[14] = data[10];
	return;
}

//output :base_array = {base_arry, seconed_array}
//this function concatenates "size" ints from second array to the end of initial first array
void concatenate(int* first_array, int* seconed_array, int size, int last_index) {

	for (int i = 0; i < size; i++) {
		first_array[last_index + i] = seconed_array[i];
	}
}

// this function converts int array into unsigned char array and writes it to file 
void intArrayToUnsignedChar(int* data_array_int, unsigned char* send_buffer, int num_of_bytes)
{
	int temp = 0;
	int  current_val = 0;

	for (int i = 0; i < num_of_bytes; i++)
	{
		current_val = 0;
		for (int j = 7; j >= 0; j--)
		{
			temp = data_array_int[7 - j + (i * 8)];
			temp *= pow(2, j);
			current_val += temp;
		}
		send_buffer[i] = current_val;
	}
}
