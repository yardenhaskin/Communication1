#include "messages.h"

/**
 * SendBuffer() uses a socket to send a buffer.
 *
 * Input:
 *		Buffer - the buffer containing the data to be sent.
 *		BytesToSend - the number of bytes from the Buffer to send.
 *		sd - the socket used for communication.
 *
 * Output:
 *		TRNS_SUCCEEDED - if sending succeeded
 *		TRNS_FAILED - otherwise
 */
int SendBuffer(const char* Buffer, int BytesToSend, SOCKET sd, SOCKADDR_IN RecvAddr)
{
	const char* CurPlacePtr = Buffer;
	int BytesTransferred;
	int RemainingBytesToSend = BytesToSend;


	while (RemainingBytesToSend > 0)
	{
		/* send does not guarantee that the entire message is sent */
		BytesTransferred = sendto(sd, CurPlacePtr, RemainingBytesToSend, 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
		if (BytesTransferred == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAETIMEDOUT || WSAGetLastError() == WSAECONNRESET)
				return ERROR_CODE;
			printf("send() failed, error %d\n", WSAGetLastError());
			return ERROR_CODE;
		}

		RemainingBytesToSend -= BytesTransferred;
		CurPlacePtr += BytesTransferred; // <ISP> pointer arithmetic
	}

	return 0;
}

/*
 * SendMsg() uses a socket to send a string.
 * Str - the string to send.
 * sd - the socket used for communication.
 */
int SendMsg(const char* Str, SOCKET sd, SOCKADDR_IN RecvAddr, int BytesToSend)
{
	/* Send the the request to the server on socket sd */
	int TotalStringSizeInBytes;
	int SendRes;


	/* The request is sent in two parts. First the Length of the string (stored in
	   an int variable ), then the string itself. */

	TotalStringSizeInBytes = BytesToSend;

	SendRes = SendBuffer(
		(const char*)(&TotalStringSizeInBytes),
		(int)(sizeof(TotalStringSizeInBytes)), // sizeof(int) 
		sd, RecvAddr);

	if (SendRes != 0)
	{
		return ERROR_CODE;
	}
	SendRes = SendBuffer(
		(const char*)(Str),
		(int)(TotalStringSizeInBytes),
		sd, RecvAddr);

	return 0;
}




/*
 * Input:
 * ReceiveBuffer() uses a socket to receive a buffer.
 * OutputBuffer - pointer to a buffer into which data will be written
 * OutputBufferSize - size in bytes of Output Buffer
 * BytesReceivedPtr - output parameter. if function returns TRNS_SUCCEEDED, then this
 *					  will point at an int containing the number of bytes received.
 * sd - the socket used for communication.
 *
 * Output:
 * TRNS_SUCCEEDED - if receiving succeeded
 * TRNS_DISCONNECTED - if the socket was disconnected
 * TRNS_FAILED - otherwise
 */
int ReceiveBuffer(char* OutputBuffer, int BytesToReceive, SOCKET sd, int timeout, SOCKADDR_IN* SenderAddr, int* SenderAddrSize)
{
	char* CurPlacePtr = OutputBuffer;
	int BytesJustTransferred;
	int RemainingBytesToReceive = BytesToReceive;

	while (RemainingBytesToReceive > 0)
	{
		/* send does not guarantee that the entire message is sent */
		BytesJustTransferred = recvfrom(sd, CurPlacePtr, RemainingBytesToReceive, 0, (SOCKADDR*)SenderAddr, SenderAddrSize);
		if (BytesJustTransferred == SOCKET_ERROR)
		{
			if (WSAEWOULDBLOCK == WSAGetLastError())// no accept - loop again
				return NREADY;
			if (WSAGetLastError() == WSAETIMEDOUT || WSAGetLastError() == WSAECONNRESET)
				return ERROR_CODE;
			printf("recv() failed, error %d\n", WSAGetLastError());
			return ERROR_CODE;
		}
		else if (BytesJustTransferred == 0)
			return ERROR_CODE; // recv() returns zero if connection was gracefully disconnected.

		RemainingBytesToReceive -= BytesJustTransferred;
		CurPlacePtr += BytesJustTransferred;
	}

	return 0;
}

/*
 * ReceiveMsg() uses a socket to receive a string, and stores it in dynamic memory.
 *
 * Input:
 * -------
 * OutputStrPtr - a pointer to a char-pointer that is initialized to NULL, as in:
 *
 *		char *Buffer = NULL;
 *		ReceiveMsg( &Buffer, ___ );
 *
 * a dynamically allocated string will be created, and (*OutputStrPtr) will point to it.
 *
 * sd - the socket used for communication.
 *
 * Output:
 * TRNS_SUCCEEDED - if receiving and memory allocation succeeded
 * TRNS_DISCONNECTED - if the socket was disconnected
 * TRNS_FAILED - otherwise
 */
int ReceiveMsg(char* OutputStr, SOCKET sd, int timeout, SOCKADDR_IN* SenderAddr, int* SenderAddrSize, int* recv_msg_size)
{
	/* Recv the request from the server on socket sd */
	int TotalStringSizeInBytes;
	int RecvRes;

	/* The request is received in two parts. First the Length of the string (stored in
	   an int variable ), then the string itself. */

	RecvRes = ReceiveBuffer(
		(char*)(&TotalStringSizeInBytes),
		(int)(sizeof(TotalStringSizeInBytes)), // 4 bytes
		sd,
		timeout, SenderAddr, SenderAddrSize);

	if (RecvRes != 0) return RecvRes;

	RecvRes = NREADY;
	while (RecvRes == NREADY)
	{
		RecvRes = ReceiveBuffer(
			(char*)(OutputStr),
			(int)(TotalStringSizeInBytes),
			sd,
			timeout, SenderAddr, SenderAddrSize);
	}

	*recv_msg_size = TotalStringSizeInBytes;

	return RecvRes;
}