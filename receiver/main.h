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
#pragma comment(lib, "ws2_32.lib")

#include "winsock2.h"
#include "ws2tcpip.h"

#define TIMEOUT_MS 1000

#endif // __main_h__