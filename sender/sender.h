#ifndef __sender_h__
#define __sender_h__

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "winsock2.h"
#include <stdbool.h>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")

#define BYTE_SIZE 8
#define INT_SIZE_IN_BYTES 4
#define BEFORE_HAMMING 11
#define AFTER_HAMMING 15
#define END_OF_FILE -5


#define PACKET_DATA_SIZE 88// = 11(bits)*8(data bytes per message)  in bytes
#define PACKET_TOTAL_SIZE 120// = 15(bits)*8(total bytes per message) in bytes


WSADATA wsaData;
SOCKET s;

char readed_byte;
FILE* fp;
unsigned char reading_mask;

size_t readOneByte();
int readBit();
int findRedundentC1(int* data);
int findRedundentC2(int* data);
int findRedundentC3(int* data);
int findRedundentC4(int* data);
int open_file(char* file_name);
void create_hamming(int* data, int* data_after_hamming);
void concatenate(int* first_array, int* seconed_array, int size, int last_index);
void intArrayToUnsignedChar(int* data_array_int, unsigned char* send_buffer, int num_of_bytes);

#endif __sender_h__
