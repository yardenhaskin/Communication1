#include "reciever.h"
#include "messages.h"

int checkQ1(int* beforeDecoding15) {
	int q1;
	q1 = beforeDecoding15[2] ^ beforeDecoding15[4] ^ beforeDecoding15[6] ^ beforeDecoding15[8] ^ beforeDecoding15[10] ^ beforeDecoding15[12] ^ beforeDecoding15[14];
	return q1;
}
int checkQ2(int* beforeDecoding15) {
	int q2;
	q2 = beforeDecoding15[2] ^ beforeDecoding15[5] ^ beforeDecoding15[6] ^ beforeDecoding15[9] ^ beforeDecoding15[10] ^ beforeDecoding15[13] ^ beforeDecoding15[14];
	return q2;
}
int checkQ4(int* beforeDecoding15) {
	int q4;
	q4 = beforeDecoding15[4] ^ beforeDecoding15[5] ^ beforeDecoding15[6] ^ beforeDecoding15[11] ^ beforeDecoding15[12] ^ beforeDecoding15[13] ^ beforeDecoding15[14];
	return q4;
}
int checkQ8(int* beforeDecoding15) {
	int q8;
	q8 = beforeDecoding15[8] ^ beforeDecoding15[9] ^ beforeDecoding15[10] ^ beforeDecoding15[11] ^ beforeDecoding15[12] ^ beforeDecoding15[13] ^ beforeDecoding15[14];
	return q8;
}

int errorFixed(int* beforeDecoding15, int q1, int q2, int q4, int q8) {
	int errorFixed;
	errorFixed = (beforeDecoding15[0] ^ q1) + (beforeDecoding15[1] ^ q2) + (beforeDecoding15[3] ^ q4) + (beforeDecoding15[7] ^ q8);
	return errorFixed;
}

int calculateBitToFix(int* beforeDecoding15, int q1, int q2, int q4, int q8) {
	int bitToFix;
	bitToFix = (beforeDecoding15[0] ^ q1) + 2 * (beforeDecoding15[1] ^ q2) + 4 * (beforeDecoding15[3] ^ q4) + 8 * (beforeDecoding15[7] ^ q8);
	return bitToFix;

}

int decode_hamming(int* decodedResult11Bit, int* beforeDecoding15)
{
	int q1, q2, q4, q8;
	int BitToFix = 0;
	int BitToFix_idx = 0;
	

	q1 = checkQ1(beforeDecoding15);
	q2 = checkQ2(beforeDecoding15);
	q4 = checkQ4(beforeDecoding15);
	q8 = checkQ8(beforeDecoding15);

	if (errorFixed(beforeDecoding15,q1,q2,q4,q8))
	{
		errors_fixed += 1;
		BitToFix = 0;
	}
	else
		BitToFix = calculateBitToFix(beforeDecoding15,  q1, q2, q4,q8);

	if (BitToFix)
		flip_bit(beforeDecoding15, BitToFix); // fixes the bit and adds to errors count 

	decodedResult11Bit[0] = beforeDecoding15[2];
	decodedResult11Bit[1] = beforeDecoding15[4];
	decodedResult11Bit[2] = beforeDecoding15[5];
	decodedResult11Bit[3] = beforeDecoding15[6];
	decodedResult11Bit[4] = beforeDecoding15[8];
	decodedResult11Bit[5] = beforeDecoding15[9];
	decodedResult11Bit[6] = beforeDecoding15[10];
	decodedResult11Bit[7] = beforeDecoding15[11];
	decodedResult11Bit[8] = beforeDecoding15[12];
	decodedResult11Bit[9] = beforeDecoding15[13];
	decodedResult11Bit[10] = beforeDecoding15[14];
	// res vector is zero and there were no errors 
	if (!BitToFix) {
		return 0;
	}
	return BitToFix;
}

// the function flipps a bit if it is needed 
void flip_bit(int* res_15, int bitToFix)
{
	int flipped_bit;
	//pick the flipped bit
	flipped_bit = res_15[bitToFix - 1];
	errors_fixed += 1;


	
	//flip it back
	if (flipped_bit == 0)
		res_15[bitToFix] = 1;
	else
		res_15[bitToFix] = 0;

	return;
}
void writeToFile(unsigned char* send_buffer, int buffer_size) {
	if (fwrite(send_buffer, sizeof(char), buffer_size, fp) < 0) {
		fprintf(stderr, "Error accured while trying to write to the file. please try again\n");
		return(ERROR_CODE);
	}
}
// this function converts int array into unsigned char array and writes it to file 
void IntArrayToSendBuffer(int* data_array_int, unsigned char* send_buffer, int buffer_size)
{
	int temp = 0;
	int i = 0, j = 0, current = 0;

	for (i = 0; i < buffer_size; i++)
	{
		current = 0;
		for (j = 7; j >= 0; j--)
		{

			temp = data_array_int[7 - j + (i * 8)];
			temp *= pow(2, j);
			current += temp;
		}
		send_buffer[i] = current;
	}
	writeToFile(send_buffer, buffer_size);
	//if (fwrite(send_buffer, sizeof(char), PACKET_DATA_SIZE, fp) < 0) {
	//	fprintf(stderr, "Error accured while trying to write to the file. please try again\n");
	//	exit(-1);
	//}
}


/*
* Error handler function - converts the 15 bit data to int array, decode it to 11 bit without redundent, and stors it in int array
* After packet is done converts the array into unsigned char array . and than writes it to file
*/
int checkBuffer(unsigned char buffer[PACKET_TOTAL_SIZE]) {
	if (!buffer) {
		fprintf(stderr, "error accured. PLease provide a buffer\n");
		return -1;
	}
	return 0;
}
void error_handler(unsigned char buffer[PACKET_TOTAL_SIZE], int buffer_size, int* buffer_written, int* buffer_corrected) {
	if (checkBuffer(buffer)<0) {
		return;
	}

	unsigned char send_buffer[PACKET_DATA_SIZE]; // Bufferto contain original data sent 
	int bufferIndex=0;  // currentent index of send buffer
	int currentBit=0, bitToFix;
	int count_15 = 0;
	unsigned char mask = 1;
	unsigned char current;
	int word_count = 0;
	int frame_count = 0;
	int beforeDecoding15[ENCODED_SIZE];
	int decodedResult11Bit[DECODED_SIZE];
	
	


	memset(send_buffer, 0, PACKET_DATA_SIZE); // fill the buffer with  PACKET_DATA_SIZE zeros
	count_15 = ENCODED_SIZE - 1;

	for (int i = 0; i < buffer_size; i++) {
		current = buffer[i];

		for (int j = 0; j < BYTE_SIZE; j++) {
			beforeDecoding15[ENCODED_SIZE - 1 - count_15] = (current & 0x80) > 0; // msb of letter = 10000000
			current = current << 1; 
			count_15 -= 1;
			if (count_15 < 0)
			{
				bitToFix = decode_hamming(decodedResult11Bit, beforeDecoding15); // will fill decoded resolution to the int array - before flipping the bit 
				//*buffer_corrected += 1; FIXME



				for (int k = 0; k < DECODED_SIZE; k++)
				{
					packet_bytes_int_arr[k + bufferIndex] = decodedResult11Bit[k]; // fill buffer array with currentent hamming decoded 
				}
				bufferIndex += DECODED_SIZE;
				count_15 = ENCODED_SIZE - 1;
			}
			// Go through each byte and parse it to bits - after 15 through it to mat mul and get put the result in sendbuffer - when I accomulate 11 bits 

		}

	} // finished reading and decoding the whole packet, time to write it to file 

	IntArrayToSendBuffer(packet_bytes_int_arr, send_buffer, buffer_size);
}


