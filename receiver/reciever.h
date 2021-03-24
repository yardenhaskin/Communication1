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


// Data for traffic received

int totalBytes = 0;
int errors_found = 0;
int errors_fixed = 0;
int writeCount = 0;
int packet_bytes_int_arr[PACKET_DATA_SIZE * BYTE_SIZE];

///functions
int checkQ1(int* beforeDecoding15);
int checkQ2(int* beforeDecoding15);
int checkQ4(int* beforeDecoding15);
int checkQ8(int* beforeDecoding15);
int errorFixed(int* beforeDecoding15, int q1, int q2, int q4, int q8);
int calculateBitToFix(int* beforeDecoding15, int q1, int q2, int q4, int q8);
int decode_hamming(int* decodedResult11Bit, int* beforeDecoding15);
void flip_bit(int* res_15, int bitToFix);
void writeToFile(unsigned char* send_buffer);
void IntArrayToSendBuffer(int* data_array_int, unsigned char* send_buffer);
void error_handler(unsigned char buffer[PACKET_TOTAL_SIZE]);
int checkBuffer(unsigned char buffer[PACKET_TOTAL_SIZE]);


int w = sizeof(struct sockaddr);
FILE* fp; //output
SOCKET s;
#pragma once
