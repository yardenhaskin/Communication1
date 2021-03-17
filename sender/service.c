#pragma comment(lib,"ws2_32.lib")

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h> // for printf
#include <string.h>    // for stringsstren
#include <stdlib.h>    // for the dynamic data - calloc
#include <WinSock2.h>   // for the socket , htons,ntohs,inet_addr
#include <math.h>
#include < stdint.h > //for the usage of uint data types
# define  bool int
#define chunk_Size 120
///output Buffer is in size of 120 bits (15 chars) 
char* processChunksOfData(char* data) {
    char temp[3]=0;
    char* outputBuffer[chunk_Size + 1] = NULL;
    while (data) {
        

    }
}


//this function calculates the parity by index
unsigned char getXorByIndex(unsigned char bit,unsigned char str) {
    bit = bit & str;
    bit = getParity(bit);
    return bit;
}
//str is the data we process. i.e 00000-11bits data (2 bytes)
//We return 15 bits from 11bits
unsigned char encrypt(unsigned char str){
    unsigned char charWithRedundents, charWithoutRedundents;
    unsigned char redundent1, redundent2, redundent3, redundent4;
    ///masks are planned by the formula of Hamming code
    unsigned char maskForredundent1 = 0x06D5;
    unsigned char maskForredundent2 = 0x05B3;
    unsigned char maskForredundent3 = 0x038f;
    unsigned char maskForredundent4 = 0x007f;
    redundent1 = getXorByIndex(maskForredundent1, str);
    redundent2 = getXorByIndex(maskForredundent2, str);
    redundent3 = getXorByIndex(maskForredundent3, str);
    redundent4 = getXorByIndex(maskForredundent4, str);
    charWithRedundents = getCharWithRedundentBits(redundent1, redundent2, redundent3, redundent4); // this is a char with redundent bits turned of (the ones who spoused to be turned)
    charWithoutRedundents = getCharWithoutRedundentBits(str); //this is a representation of the data after sparsing it and leave space for redundents
    return (charWithRedundents | charWithoutRedundents);
}
unsigned char getFirstBit(unsigned char redundent) {
    if (redundent)
        return (1 << 15);
    return 0;
}
unsigned char getsecondBit(unsigned char redundent) {
    if (redundent)
        return (1 << 14);
    return 0;
}
unsigned char getthirdBit(unsigned char redundent) {
    if (redundent)
        return (1 << 12);
    return 0;
}
unsigned char getforthBit(unsigned char redundent) {
    if (redundent)
        return (1 << 8);
    return 0;
}
//this function returns an unsigned char with the redundent bits
unsigned char getCharWithRedundentBits(unsigned char redundent1, unsigned char redundent2, unsigned char redundent3, unsigned char redundent4) {
    unsigned char result;
    redundent1 = getFirstBit(redundent1);
    redundent2 = getsecondBit(redundent2);
    redundent3 = getthirdBit(redundent3);
    redundent4 = getforthBit(redundent4);
    result = (redundent1 | redundent2 | redundent3 | redundent4);
    return result;
}
unsigned char getCharWithoutRedundentBits(unsigned char str) {
    unsigned char result = 0;
    unsigned char firstBit;
    unsigned char secondToForthBits;
    unsigned char lastGroupOfBits;
    firstBit = str & 0x0400;
    firstBit = firstBit << 3 ;
    secondToForthBits = str & 0x0380;
    secondToForthBits = secondToForthBits << 1;
    lastGroupOfBits = str & 0x00ff;
    result = result | firstBit | secondToForthBits| lastGroupOfBits;
    return result;
}

///function to find number of bits on in a binary represantation 
//1 =odd , 0 =even
bool getParity(unsigned char n)
{
    bool parity = 0;
    while (n)
    {
        parity = !parity;
        n = n & (n - 1);
    }
    return parity;
}


int maskElevenBitsFromNumber(unsigned char number) {
    unsigned char mask = 0x7ff;
    return (number & mask);

}

//uint32_t byte1 = (value >> 24);          

//uint32_t byte2 = (value >> 16) & 0xff;    

//uint32_t byte3 = (value >> 8) & 0xff;   

//uint32_t byte4 = value & 0xff;
///function to extract 11 bits from |number" from position p
//not sure about this function
int bitExtracted(unsigned char number, int p)
{
    return (((1 << 11) - 1) & (number >> (p - 1)));
}

int getElevenBitsBlocks(char* buffer) {

}