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
int SendString(const char* Str, SOCKET sd, SOCKADDR_IN RecvAddr);
int ReceiveBuffer(char* OutputBuffer, int BytesToReceive, SOCKET sd, int timeout, SOCKADDR_IN* SenderAddr, int* SenderAddrSize);
int ReceiveString(char* OutputStr, SOCKET sd, int timeout, SOCKADDR_IN* SenderAddr, int* SenderAddrSize);




//consts
static const int ERROR_CODE = -1;
#define MY_ADDRESS "127.0.0.1"
#define MAX_MSG_LEN 1500 //FIXME: change to relevant number
#define INET_ADDRSTRLEN 30
#endif // __messages.h__