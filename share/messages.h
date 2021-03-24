#ifndef __messages_h__
#define __messages_h__

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

//include
#pragma comment(lib, "ws2_32.lib")

#include "winsock2.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>

//functions
int SendBuffer(const char* Buffer, int BytesToSend, SOCKET sd, SOCKADDR_IN RecvAddr);
int SendString(const char* Str, SOCKET sd, SOCKADDR_IN RecvAddr, int BytesToSend);
int ReceiveBuffer(char* OutputBuffer, int BytesToReceive, SOCKET sd, int timeout, SOCKADDR_IN* SenderAddr, int* SenderAddrSize);
int ReceiveString(char* OutputStr, SOCKET sd, int timeout, SOCKADDR_IN* SenderAddr, int* SenderAddrSize, int* recv_msg_size);


//functions
void construct_summary_msg(char* summary_msg, int total_msg_size, int total_written, int total_corrected);
DWORD end_thread_action(void* not_relevant);


//consts
#define STRINGS_ARE_EQUAL( Str1, Str2 ) ( strcmp( (Str1), (Str2) ) == 0 )
static const int ERROR_CODE = -1;
#define MY_ADDRESS "127.0.0.1"
#define PACKET_TOTAL_SIZE 120// = 15(bits)*8(total bytes per message) in bytes
#define INET_ADDRSTRLEN 30
#define SUMMARY_MSG 200
#define MAX_INT_LEN 15
#define NREADY -2

#endif // __messages.h__