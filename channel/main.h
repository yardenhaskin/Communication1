#ifndef __main_h__
#define __main_h__

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

//includes
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <math.h>
#include <time.h>
#include <float.h>
#pragma comment(lib, "ws2_32.lib")

#include "winsock2.h"
#include "ws2tcpip.h"

int generate_noise(unsigned char* received_msg, int single_bit_err_prob, int recv_msg_size, int* bits_flipped);
void generate_noise_for_byte(unsigned char* received_msg, double p, int byte_index, int* bits_flipped);
int get_num_of_flips(int num_of_bits, double p);
int choose(n, k);
double dblrand();

#define MAX_RAND_DOUBLE 35184372088831




#endif // __main_h__