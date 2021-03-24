#ifndef __reciever_h__
#define __reciever_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "winsock2.h"

#define PACKET_DATA_SIZE 88// = 11(bits)*8(data bytes per message)  in bytes
#define PACKET_TOTAL_SIZE 120// = 15(bits)*8(total bytes per message) in bytes
#define BYTE_SIZE 8
#define DECODED_SIZE 11
#define ENCODED_SIZE 15




///functions
int checkQ1(int* beforeDecoding15);
int checkQ2(int* beforeDecoding15);
int checkQ4(int* beforeDecoding15);
int checkQ8(int* beforeDecoding15);
int errorFixed(int* beforeDecoding15, int q1, int q2, int q4, int q8);
int calculateBitToFix(int* beforeDecoding15, int q1, int q2, int q4, int q8);
int decode_hamming(int* decodedResult11Bit, int* beforeDecoding15);
void flip_bit(int* res_15, int bitToFix);
void writeToFile(unsigned char* send_buffer, int buffer_size);
void IntArrayToSendBuffer(int* data_array_int, unsigned char* send_buffer, int buffer_size);
void error_handler(unsigned char buffer[PACKET_TOTAL_SIZE], int buffer_size);
int checkBuffer(unsigned char buffer[PACKET_TOTAL_SIZE]);





#endif // __reciever.h__
