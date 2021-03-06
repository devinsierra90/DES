// DesProgram.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <chrono>
#include <sstream>
using namespace std;

//global variables
string textForFile = "";
unsigned long long keys48Bits[16];
string outputFileName;
clock_t start, endTime;
#define CLOCKS_PER_MS (CLOCKS_PER_SEC / 1000);

/*
SBOX mappings
*/
int map1[64] =
{
	14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,
	0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
	4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
	15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13
};
int map2[64] =
{
	15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
	3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
	0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
	13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9
};
int map3[64] =
{ 10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12
};
int map4[64] =
{
	7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
	13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
	10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
	3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14
};
int map5[64] =
{
	2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
	14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
	4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
	11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3
};

int map6[64] =
{
	12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
	10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
	9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
	4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13
};

int map7[64] =
{
	4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
	13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
	1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
	6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12
};
int map8[64] =
{
	13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
	1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
	7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
	2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11
};

/*
This method takes in the output of the DES algorithm as an unsigned long.
It then converts that number into the appopriate characters and concatenates them into a string.
That string is then appended to the string holding the text that we will write to the file at the end.
*/
void convertToText(unsigned long long number) {
	unsigned long long nextNumber = 0;
	string strToWrite = "";

	nextNumber = (number & 0xFF00000000000000);
	nextNumber >>= 56;
	strToWrite.push_back((char)nextNumber);

	nextNumber = (number & 0x00FF000000000000);
	nextNumber >>= 48;
	strToWrite.push_back((char)nextNumber);

	nextNumber = (number & 0x0000FF0000000000);
	nextNumber >>= 40;
	strToWrite.push_back((char)nextNumber);

	nextNumber = (number & 0x000000FF00000000);
	nextNumber >>= 32;
	strToWrite.push_back((char)nextNumber);

	nextNumber = (number & 0x00000000FF000000);
	nextNumber >>= 24;
	strToWrite.push_back((char)nextNumber);

	nextNumber = (number & 0x0000000000FF0000);
	nextNumber >>= 16;
	strToWrite.push_back((char)nextNumber);

	nextNumber = (number & 0x000000000000FF00);
	nextNumber >>= 8;
	strToWrite.push_back((char)nextNumber);

	nextNumber = (number & 0x00000000000000FF);
	strToWrite.push_back((char)nextNumber);

	textForFile += strToWrite;
}

/*
This method works the same as convertToText -- except -- that it is used specifically for Decrypting.
When we are decrypting and the text we want is not an even multiple of 8-bytes, we need to discard the extra stuff, which is why this method exists.
*/
void writeRemainingChars(unsigned long long block, int charsToWrite) {

	int numCharsAccounterfor = 0;
	unsigned long long nextNumber = 0;
	string strToWrite = "";



	numCharsAccounterfor++;
	nextNumber = (block & 0xFF00000000000000);
	nextNumber >>= 56;
	strToWrite.push_back((char)nextNumber);

	if (numCharsAccounterfor != charsToWrite) {
		numCharsAccounterfor++;
		nextNumber = (block & 0x00FF000000000000);
		nextNumber >>= 48;
		strToWrite.push_back((char)nextNumber);
	}

	if (numCharsAccounterfor != charsToWrite) {
		numCharsAccounterfor++;
		nextNumber = (block & 0x0000FF0000000000);
		nextNumber >>= 40;
		strToWrite.push_back((char)nextNumber);
	}

	if (numCharsAccounterfor != charsToWrite) {
		numCharsAccounterfor++;
		nextNumber = (block & 0x000000FF00000000);
		nextNumber >>= 32;
		strToWrite.push_back((char)nextNumber);
	}

	if (numCharsAccounterfor != charsToWrite) {
		numCharsAccounterfor++;
		nextNumber = (block & 0x00000000FF000000);
		nextNumber >>= 24;
		strToWrite.push_back((char)nextNumber);
	}

	if (numCharsAccounterfor != charsToWrite) {
		numCharsAccounterfor++;
		nextNumber = (block & 0x0000000000FF0000);
		nextNumber >>= 16;
		strToWrite.push_back((char)nextNumber);
	}

	if (numCharsAccounterfor != charsToWrite) {
		numCharsAccounterfor++;
		nextNumber = (block & 0x000000000000FF00);
		nextNumber >>= 8;
		strToWrite.push_back((char)nextNumber);
	}

	if (numCharsAccounterfor != charsToWrite) {
		numCharsAccounterfor++;
		nextNumber = (block & 0x00000000000000FF);
		strToWrite.push_back((char)nextNumber);
	}



	textForFile += strToWrite;
}


/*
This is the code for the final permutation executed in DES
*/
unsigned long long finalPermutation(unsigned long long block) {
	unsigned long long permutedBlock = 0;
	if (block & 0x8000000000000000) permutedBlock |= (1ull << (64 - 58));
	if (block & 0x4000000000000000) permutedBlock |= (1ull << (64 - 50));
	if (block & 0x2000000000000000) permutedBlock |= (1ull << (64 - 42));
	if (block & 0x1000000000000000) permutedBlock |= (1ull << (64 - 34));
	if (block & 0x0800000000000000) permutedBlock |= (1ull << (64 - 26));
	if (block & 0x0400000000000000) permutedBlock |= (1ull << (64 - 18));
	if (block & 0x0200000000000000) permutedBlock |= (1ull << (64 - 10));
	if (block & 0x0100000000000000) permutedBlock |= (1ull << (64 - 2));
	if (block & 0x0080000000000000) permutedBlock |= (1ull << (64 - 60));
	if (block & 0x0040000000000000) permutedBlock |= (1ull << (64 - 52));
	if (block & 0x0020000000000000) permutedBlock |= (1ull << (64 - 44));
	if (block & 0x0010000000000000) permutedBlock |= (1ull << (64 - 36));
	if (block & 0x0008000000000000) permutedBlock |= (1ull << (64 - 28));
	if (block & 0x0004000000000000) permutedBlock |= (1ull << (64 - 20));
	if (block & 0x0002000000000000) permutedBlock |= (1ull << (64 - 12));
	if (block & 0x0001000000000000) permutedBlock |= (1ull << (64 - 4));
	if (block & 0x0000800000000000) permutedBlock |= (1ull << (64 - 62));
	if (block & 0x0000400000000000) permutedBlock |= (1ull << (64 - 54));
	if (block & 0x0000200000000000) permutedBlock |= (1ull << (64 - 46));
	if (block & 0x0000100000000000) permutedBlock |= (1ull << (64 - 38));
	if (block & 0x0000080000000000) permutedBlock |= (1ull << (64 - 30));
	if (block & 0x0000040000000000) permutedBlock |= (1ull << (64 - 22));
	if (block & 0x0000020000000000) permutedBlock |= (1ull << (64 - 14));
	if (block & 0x0000010000000000) permutedBlock |= (1ull << (64 - 6));
	if (block & 0x0000008000000000) permutedBlock |= (1ull << (64 - 64));
	if (block & 0x0000004000000000) permutedBlock |= (1ull << (64 - 56));
	if (block & 0x0000002000000000) permutedBlock |= (1ull << (64 - 48));
	if (block & 0x0000001000000000) permutedBlock |= (1ull << (64 - 40));
	if (block & 0x0000000800000000) permutedBlock |= (1ull << (64 - 32));
	if (block & 0x0000000400000000) permutedBlock |= (1ull << (64 - 24));
	if (block & 0x0000000200000000) permutedBlock |= (1ull << (64 - 16));
	if (block & 0x0000000100000000) permutedBlock |= (1ull << (64 - 8));
	if (block & 0x0000000080000000) permutedBlock |= (1ull << (64 - 57));
	if (block & 0x0000000040000000) permutedBlock |= (1ull << (64 - 49));
	if (block & 0x0000000020000000) permutedBlock |= (1ull << (64 - 41));
	if (block & 0x0000000010000000) permutedBlock |= (1ull << (64 - 33));
	if (block & 0x0000000008000000) permutedBlock |= (1ull << (64 - 25));
	if (block & 0x0000000004000000) permutedBlock |= (1ull << (64 - 17));
	if (block & 0x0000000002000000) permutedBlock |= (1ull << (64 - 9));
	if (block & 0x0000000001000000) permutedBlock |= (1ull << (64 - 1));
	if (block & 0x0000000000800000) permutedBlock |= (1ull << (64 - 59));
	if (block & 0x0000000000400000) permutedBlock |= (1ull << (64 - 51));
	if (block & 0x0000000000200000) permutedBlock |= (1ull << (64 - 43));
	if (block & 0x0000000000100000) permutedBlock |= (1ull << (64 - 35));
	if (block & 0x0000000000080000) permutedBlock |= (1ull << (64 - 27));
	if (block & 0x0000000000040000) permutedBlock |= (1ull << (64 - 19));
	if (block & 0x0000000000020000) permutedBlock |= (1ull << (64 - 11));
	if (block & 0x0000000000010000) permutedBlock |= (1ull << (64 - 3));
	if (block & 0x0000000000008000) permutedBlock |= (1ull << (64 - 61));
	if (block & 0x0000000000004000) permutedBlock |= (1ull << (64 - 53));
	if (block & 0x0000000000002000) permutedBlock |= (1ull << (64 - 45));
	if (block & 0x0000000000001000) permutedBlock |= (1ull << (64 - 37));
	if (block & 0x0000000000000800) permutedBlock |= (1ull << (64 - 29));
	if (block & 0x0000000000000400) permutedBlock |= (1ull << (64 - 21));
	if (block & 0x0000000000000200) permutedBlock |= (1ull << (64 - 13));
	if (block & 0x0000000000000100) permutedBlock |= (1ull << (64 - 5));
	if (block & 0x0000000000000080) permutedBlock |= (1ull << (64 - 63));
	if (block & 0x0000000000000040) permutedBlock |= (1ull << (64 - 55));
	if (block & 0x0000000000000020) permutedBlock |= (1ull << (64 - 47));
	if (block & 0x0000000000000010) permutedBlock |= (1ull << (64 - 39));
	if (block & 0x0000000000000008) permutedBlock |= (1ull << (64 - 31));
	if (block & 0x0000000000000004) permutedBlock |= (1ull << (64 - 23));
	if (block & 0x0000000000000002) permutedBlock |= (1ull << (64 - 15));
	if (block & 0x0000000000000001) permutedBlock |= (1ull << (64 - 7));

	return permutedBlock;

}

/*
This is the code for the permutation that happens right after the SBOX substitution
*/
unsigned long long postSBoxPermute(unsigned long long bits) {

	unsigned long long permutedBits = 0;
	if (bits & 0x80000000) permutedBits |= (1ull << (32 - 9));
	if (bits & 0x40000000) permutedBits |= (1ull << (32 - 17));
	if (bits & 0x20000000) permutedBits |= (1ull << (32 - 23));
	if (bits & 0x10000000) permutedBits |= (1ull << (32 - 31));
	if (bits & 0x08000000) permutedBits |= (1ull << (32 - 13));
	if (bits & 0x04000000) permutedBits |= (1ull << (32 - 28));
	if (bits & 0x02000000) permutedBits |= (1ull << (32 - 2));
	if (bits & 0x01000000) permutedBits |= (1ull << (32 - 18));
	if (bits & 0x00800000) permutedBits |= (1ull << (32 - 24));
	if (bits & 0x00400000) permutedBits |= (1ull << (32 - 16));
	if (bits & 0x00200000) permutedBits |= (1ull << (32 - 30));
	if (bits & 0x00100000) permutedBits |= (1ull << (32 - 6));
	if (bits & 0x00080000) permutedBits |= (1ull << (32 - 26));
	if (bits & 0x00040000) permutedBits |= (1ull << (32 - 20));
	if (bits & 0x00020000) permutedBits |= (1ull << (32 - 10));
	if (bits & 0x00010000) permutedBits |= (1ull << (32 - 1));
	if (bits & 0x00008000) permutedBits |= (1ull << (32 - 8));
	if (bits & 0x00004000) permutedBits |= (1ull << (32 - 14));
	if (bits & 0x00002000) permutedBits |= (1ull << (32 - 25));
	if (bits & 0x00001000) permutedBits |= (1ull << (32 - 3));
	if (bits & 0x00000800) permutedBits |= (1ull << (32 - 4));
	if (bits & 0x00000400) permutedBits |= (1ull << (32 - 29));
	if (bits & 0x00000200) permutedBits |= (1ull << (32 - 11));
	if (bits & 0x00000100) permutedBits |= (1ull << (32 - 19));
	if (bits & 0x00000080) permutedBits |= (1ull << (32 - 32));
	if (bits & 0x00000040) permutedBits |= (1ull << (32 - 12));
	if (bits & 0x00000020) permutedBits |= (1ull << (32 - 22));
	if (bits & 0x00000010) permutedBits |= (1ull << (32 - 7));
	if (bits & 0x00000008) permutedBits |= (1ull << (32 - 5));
	if (bits & 0x00000004) permutedBits |= (1ull << (32 - 27));
	if (bits & 0x00000002) permutedBits |= (1ull << (32 - 15));
	if (bits & 0x00000001) permutedBits |= (1ull << (32 - 21));

	return permutedBits;

}

/*
this is the code that will expand the 32-bit right side to a 48-bit block, to be XORd with the key
*/
unsigned long long expandRightSide(unsigned long long block) {

	unsigned long long expandedBlock = 0;
	if (block & 0x80000000) expandedBlock |= (1ull << (48 - 2));
	if (block & 0x80000000) expandedBlock |= (1ull << (48 - 48));
	if (block & 0x40000000) expandedBlock |= (1ull << (48 - 3));
	if (block & 0x20000000) expandedBlock |= (1ull << (48 - 4));
	if (block & 0x10000000) expandedBlock |= (1ull << (48 - 5));
	if (block & 0x10000000) expandedBlock |= (1ull << (48 - 7));
	if (block & 0x08000000) expandedBlock |= (1ull << (48 - 6));
	if (block & 0x08000000) expandedBlock |= (1ull << (48 - 8));
	if (block & 0x04000000) expandedBlock |= (1ull << (48 - 9));
	if (block & 0x02000000) expandedBlock |= (1ull << (48 - 10));
	if (block & 0x01000000) expandedBlock |= (1ull << (48 - 11));
	if (block & 0x01000000) expandedBlock |= (1ull << (48 - 13));
	if (block & 0x00800000) expandedBlock |= (1ull << (48 - 12));
	if (block & 0x00800000) expandedBlock |= (1ull << (48 - 14));
	if (block & 0x00400000) expandedBlock |= (1ull << (48 - 15));
	if (block & 0x00200000) expandedBlock |= (1ull << (48 - 16));
	if (block & 0x00100000) expandedBlock |= (1ull << (48 - 17));
	if (block & 0x00100000) expandedBlock |= (1ull << (48 - 19));
	if (block & 0x00080000) expandedBlock |= (1ull << (48 - 18));
	if (block & 0x00080000) expandedBlock |= (1ull << (48 - 20));
	if (block & 0x00040000) expandedBlock |= (1ull << (48 - 21));
	if (block & 0x00020000) expandedBlock |= (1ull << (48 - 22));
	if (block & 0x00010000) expandedBlock |= (1ull << (48 - 23));
	if (block & 0x00010000) expandedBlock |= (1ull << (48 - 25));
	if (block & 0x00008000) expandedBlock |= (1ull << (48 - 24));
	if (block & 0x00008000) expandedBlock |= (1ull << (48 - 26));
	if (block & 0x00004000) expandedBlock |= (1ull << (48 - 27));
	if (block & 0x00002000) expandedBlock |= (1ull << (48 - 28));
	if (block & 0x00001000) expandedBlock |= (1ull << (48 - 29));
	if (block & 0x00001000) expandedBlock |= (1ull << (48 - 31));
	if (block & 0x00000800) expandedBlock |= (1ull << (48 - 30));
	if (block & 0x00000800) expandedBlock |= (1ull << (48 - 32));
	if (block & 0x00000400) expandedBlock |= (1ull << (48 - 33));
	if (block & 0x00000200) expandedBlock |= (1ull << (48 - 34));
	if (block & 0x00000100) expandedBlock |= (1ull << (48 - 35));
	if (block & 0x00000100) expandedBlock |= (1ull << (48 - 37));
	if (block & 0x00000080) expandedBlock |= (1ull << (48 - 36));
	if (block & 0x00000080) expandedBlock |= (1ull << (48 - 38));
	if (block & 0x00000040) expandedBlock |= (1ull << (48 - 39));
	if (block & 0x00000020) expandedBlock |= (1ull << (48 - 40));
	if (block & 0x00000010) expandedBlock |= (1ull << (48 - 41));
	if (block & 0x00000010) expandedBlock |= (1ull << (48 - 43));
	if (block & 0x00000008) expandedBlock |= (1ull << (48 - 42));
	if (block & 0x00000008) expandedBlock |= (1ull << (48 - 44));
	if (block & 0x00000004) expandedBlock |= (1ull << (48 - 45));
	if (block & 0x00000002) expandedBlock |= (1ull << (48 - 46));
	if (block & 0x00000001) expandedBlock |= (1ull << (48 - 47));
	if (block & 0x00000001) expandedBlock |= (1ull << (48 - 1));


	return expandedBlock;

}

/*
This code is for the initial permutation in DES
*/
unsigned long long initialPermutation(unsigned long long block) {

	unsigned long long permutedBlock = 0;
	if (block & 0x8000000000000000) permutedBlock |= (1ull << (64 - 40));
	if (block & 0x4000000000000000) permutedBlock |= (1ull << (64 - 8));
	if (block & 0x2000000000000000) permutedBlock |= (1ull << (64 - 48));
	if (block & 0x1000000000000000) permutedBlock |= (1ull << (64 - 16));
	if (block & 0x0800000000000000) permutedBlock |= (1ull << (64 - 56));
	if (block & 0x0400000000000000) permutedBlock |= (1ull << (64 - 24));
	if (block & 0x0200000000000000) permutedBlock |= (1ull << (64 - 64));
	if (block & 0x0100000000000000) permutedBlock |= (1ull << (64 - 32));
	if (block & 0x0080000000000000) permutedBlock |= (1ull << (64 - 39));
	if (block & 0x0040000000000000) permutedBlock |= (1ull << (64 - 7));
	if (block & 0x0020000000000000) permutedBlock |= (1ull << (64 - 47));
	if (block & 0x0010000000000000) permutedBlock |= (1ull << (64 - 15));
	if (block & 0x0008000000000000) permutedBlock |= (1ull << (64 - 55));
	if (block & 0x0004000000000000) permutedBlock |= (1ull << (64 - 23));
	if (block & 0x0002000000000000) permutedBlock |= (1ull << (64 - 63));
	if (block & 0x0001000000000000) permutedBlock |= (1ull << (64 - 31));
	if (block & 0x0000800000000000) permutedBlock |= (1ull << (64 - 38));
	if (block & 0x0000400000000000) permutedBlock |= (1ull << (64 - 6));
	if (block & 0x0000200000000000) permutedBlock |= (1ull << (64 - 46));
	if (block & 0x0000100000000000) permutedBlock |= (1ull << (64 - 14));
	if (block & 0x0000080000000000) permutedBlock |= (1ull << (64 - 54));
	if (block & 0x0000040000000000) permutedBlock |= (1ull << (64 - 22));
	if (block & 0x0000020000000000) permutedBlock |= (1ull << (64 - 62));
	if (block & 0x0000010000000000) permutedBlock |= (1ull << (64 - 30));
	if (block & 0x0000008000000000) permutedBlock |= (1ull << (64 - 37));
	if (block & 0x0000004000000000) permutedBlock |= (1ull << (64 - 5));
	if (block & 0x0000002000000000) permutedBlock |= (1ull << (64 - 45));
	if (block & 0x0000001000000000) permutedBlock |= (1ull << (64 - 13));
	if (block & 0x0000000800000000) permutedBlock |= (1ull << (64 - 53));
	if (block & 0x0000000400000000) permutedBlock |= (1ull << (64 - 21));
	if (block & 0x0000000200000000) permutedBlock |= (1ull << (64 - 61));
	if (block & 0x0000000100000000) permutedBlock |= (1ull << (64 - 29));
	if (block & 0x0000000080000000) permutedBlock |= (1ull << (64 - 36));
	if (block & 0x0000000040000000) permutedBlock |= (1ull << (64 - 4));
	if (block & 0x0000000020000000) permutedBlock |= (1ull << (64 - 44));
	if (block & 0x0000000010000000) permutedBlock |= (1ull << (64 - 12));
	if (block & 0x0000000008000000) permutedBlock |= (1ull << (64 - 52));
	if (block & 0x0000000004000000) permutedBlock |= (1ull << (64 - 20));
	if (block & 0x0000000002000000) permutedBlock |= (1ull << (64 - 60));
	if (block & 0x0000000001000000) permutedBlock |= (1ull << (64 - 28));
	if (block & 0x0000000000800000) permutedBlock |= (1ull << (64 - 35));
	if (block & 0x0000000000400000) permutedBlock |= (1ull << (64 - 3));
	if (block & 0x0000000000200000) permutedBlock |= (1ull << (64 - 43));
	if (block & 0x0000000000100000) permutedBlock |= (1ull << (64 - 11));
	if (block & 0x0000000000080000) permutedBlock |= (1ull << (64 - 51));
	if (block & 0x0000000000040000) permutedBlock |= (1ull << (64 - 19));
	if (block & 0x0000000000020000) permutedBlock |= (1ull << (64 - 59));
	if (block & 0x0000000000010000) permutedBlock |= (1ull << (64 - 27));
	if (block & 0x0000000000008000) permutedBlock |= (1ull << (64 - 34));
	if (block & 0x0000000000004000) permutedBlock |= (1ull << (64 - 2));
	if (block & 0x0000000000002000) permutedBlock |= (1ull << (64 - 42));
	if (block & 0x0000000000001000) permutedBlock |= (1ull << (64 - 10));
	if (block & 0x0000000000000800) permutedBlock |= (1ull << (64 - 50));
	if (block & 0x0000000000000400) permutedBlock |= (1ull << (64 - 18));
	if (block & 0x0000000000000200) permutedBlock |= (1ull << (64 - 58));
	if (block & 0x0000000000000100) permutedBlock |= (1ull << (64 - 26));
	if (block & 0x0000000000000080) permutedBlock |= (1ull << (64 - 33));
	if (block & 0x0000000000000040) permutedBlock |= (1ull << (64 - 1));
	if (block & 0x0000000000000020) permutedBlock |= (1ull << (64 - 41));
	if (block & 0x0000000000000010) permutedBlock |= (1ull << (64 - 9));
	if (block & 0x0000000000000008) permutedBlock |= (1ull << (64 - 49));
	if (block & 0x0000000000000004) permutedBlock |= (1ull << (64 - 17));
	if (block & 0x0000000000000002) permutedBlock |= (1ull << (64 - 57));
	if (block & 0x0000000000000001) permutedBlock |= (1ull << (64 - 25));

	return permutedBlock;
}

/*
This is the method that executes DES
*/
unsigned long long performDES(unsigned long long block, bool isFirstBlock) {

	unsigned long long blockToEncrypt = block;
	unsigned long long blockAfterInitialPermutation = initialPermutation(blockToEncrypt);

	//separate the halveds
	unsigned long long leftHalf = blockAfterInitialPermutation >> 32;
	unsigned long long rightHalf = (blockAfterInitialPermutation & 0x00000000FFFFFFFF);

	unsigned long long expandedRightHalf;
	unsigned long long sBits1, sBits2, sBits3, sBits4, sBits5, sBits6, sBits7, sBits8;
	unsigned long long recombinedSboxes = 0;
	unsigned long long postSBoxBits = 0;
	unsigned long long finalOutput = 0;
	unsigned long long leftSubN, leftSubNMinusOne, rightSubN, rightSubNMinusOne; // SUBN refers to the current round variables.
																				 // SubNMinusOne referes to the lasts rounds values

																				 // Set R0 and L0 since we are going into round 1
	rightSubNMinusOne = rightHalf;
	leftSubNMinusOne = leftHalf;

	for (int round = 1; round < 17; round++) {

		//left side = previous right side	
		leftSubN = rightSubNMinusOne;

		//expand the 32-bit right side into 48 bits and then XOR with the current key
		expandedRightHalf = expandRightSide(rightSubNMinusOne);
		expandedRightHalf ^= keys48Bits[round - 1];

		//seperate the the 48-bit expanded right side into (8) 6-bit containers to be sent into the S-box			

		sBits1 = (blockToEncrypt & 0xFC0000000000);
		sBits1 >>= 42;
		sBits2 = (blockToEncrypt & 0x03F000000000);
		sBits2 >>= 36;
		sBits3 = (blockToEncrypt & 0x000FC0000000);
		sBits3 >>= 30;
		sBits4 = (blockToEncrypt & 0x00003F000000);
		sBits4 >>= 24;
		sBits5 = (blockToEncrypt & 0x000000FC0000);
		sBits5 >>= 18;
		sBits6 = (blockToEncrypt & 0x00000003F000);
		sBits6 >>= 12;
		sBits7 = (blockToEncrypt & 0x000000000FC0);
		sBits7 >>= 6;
		sBits8 = (blockToEncrypt & 0x00000000003F);



		//rearrange the 6-bit containers in order to perform the s-boxes
		unsigned long long reorderedBits = 0;
		if (sBits1 & 0x20) reorderedBits |= (1 << 6 - 1);
		if (sBits1 & 0x10) reorderedBits |= (1 << 6 - 3);
		if (sBits1 & 0x08) reorderedBits |= (1 << 6 - 4);
		if (sBits1 & 0x04) reorderedBits |= (1 << 6 - 5);
		if (sBits1 & 0x02) reorderedBits |= (1 << 6 - 6);
		if (sBits1 & 0x01) reorderedBits |= (1 << 6 - 2);
		sBits1 = reorderedBits;

		reorderedBits = 0;
		if (sBits2 & 0x20) reorderedBits |= (1 << 6 - 1);
		if (sBits2 & 0x10) reorderedBits |= (1 << 6 - 3);
		if (sBits2 & 0x08) reorderedBits |= (1 << 6 - 4);
		if (sBits2 & 0x04) reorderedBits |= (1 << 6 - 5);
		if (sBits2 & 0x02) reorderedBits |= (1 << 6 - 6);
		if (sBits2 & 0x01) reorderedBits |= (1 << 6 - 2);
		sBits2 = reorderedBits;

		reorderedBits = 0;
		if (sBits3 & 0x20) reorderedBits |= (1 << 6 - 1);
		if (sBits3 & 0x10) reorderedBits |= (1 << 6 - 3);
		if (sBits3 & 0x08) reorderedBits |= (1 << 6 - 4);
		if (sBits3 & 0x04) reorderedBits |= (1 << 6 - 5);
		if (sBits3 & 0x02) reorderedBits |= (1 << 6 - 6);
		if (sBits3 & 0x01) reorderedBits |= (1 << 6 - 2);
		sBits3 = reorderedBits;

		reorderedBits = 0;
		if (sBits4 & 0x20) reorderedBits |= (1 << 6 - 1);
		if (sBits4 & 0x10) reorderedBits |= (1 << 6 - 3);
		if (sBits4 & 0x08) reorderedBits |= (1 << 6 - 4);
		if (sBits4 & 0x04) reorderedBits |= (1 << 6 - 5);
		if (sBits4 & 0x02) reorderedBits |= (1 << 6 - 6);
		if (sBits4 & 0x01) reorderedBits |= (1 << 6 - 2);
		sBits4 = reorderedBits;

		reorderedBits = 0;
		if (sBits5 & 0x20) reorderedBits |= (1 << 6 - 1);
		if (sBits5 & 0x10) reorderedBits |= (1 << 6 - 3);
		if (sBits5 & 0x08) reorderedBits |= (1 << 6 - 4);
		if (sBits5 & 0x04) reorderedBits |= (1 << 6 - 5);
		if (sBits5 & 0x02) reorderedBits |= (1 << 6 - 6);
		if (sBits5 & 0x01) reorderedBits |= (1 << 6 - 2);
		sBits5 = reorderedBits;

		reorderedBits = 0;
		if (sBits6 & 0x20) reorderedBits |= (1 << 6 - 1);
		if (sBits6 & 0x10) reorderedBits |= (1 << 6 - 3);
		if (sBits6 & 0x08) reorderedBits |= (1 << 6 - 4);
		if (sBits6 & 0x04) reorderedBits |= (1 << 6 - 5);
		if (sBits6 & 0x02) reorderedBits |= (1 << 6 - 6);
		if (sBits6 & 0x01) reorderedBits |= (1 << 6 - 2);
		sBits6 = reorderedBits;

		reorderedBits = 0;
		if (sBits7 & 0x20) reorderedBits |= (1 << 6 - 1);
		if (sBits7 & 0x10) reorderedBits |= (1 << 6 - 3);
		if (sBits7 & 0x08) reorderedBits |= (1 << 6 - 4);
		if (sBits7 & 0x04) reorderedBits |= (1 << 6 - 5);
		if (sBits7 & 0x02) reorderedBits |= (1 << 6 - 6);
		if (sBits7 & 0x01) reorderedBits |= (1 << 6 - 2);
		sBits7 = reorderedBits;

		reorderedBits = 0;
		if (sBits8 & 0x20) reorderedBits |= (1 << 6 - 1);
		if (sBits8 & 0x10) reorderedBits |= (1 << 6 - 3);
		if (sBits8 & 0x08) reorderedBits |= (1 << 6 - 4);
		if (sBits8 & 0x04) reorderedBits |= (1 << 6 - 5);
		if (sBits8 & 0x02) reorderedBits |= (1 << 6 - 6);
		if (sBits8 & 0x01) reorderedBits |= (1 << 6 - 2);
		sBits8 = reorderedBits;





		//send each 6-bit container into its proper sbox and receive the new bits
		//the new number should be 32 bits so we will shift left by 4 bits after obtaining the current sbox results. the newest sbox results will
		//then be added to that number 
		sBits1 = map1[sBits1];
		recombinedSboxes += sBits1;

		sBits2 = map2[sBits2];
		recombinedSboxes <<= 4;
		recombinedSboxes += sBits2;

		sBits3 = map3[sBits3];
		recombinedSboxes <<= 4;
		recombinedSboxes += sBits3;

		sBits4 = map4[sBits4];
		recombinedSboxes <<= 4;
		recombinedSboxes += sBits4;

		sBits5 = map5[sBits5];
		recombinedSboxes <<= 4;
		recombinedSboxes += sBits5;

		sBits6 = map6[sBits6];
		recombinedSboxes <<= 4;
		recombinedSboxes += sBits6;

		sBits7 = map7[sBits7];
		recombinedSboxes <<= 4;
		recombinedSboxes += sBits7;

		sBits8 = map8[sBits8];
		recombinedSboxes <<= 4;
		recombinedSboxes += sBits8;


		//after the sbox manipulation we must perform a permutation and then XOR with the left side
		postSBoxBits = postSBoxPermute(recombinedSboxes);
		postSBoxBits ^= leftSubNMinusOne;


		// reset the variables so we know the values of the left and right side for the previous round
		rightSubN = postSBoxBits;
		rightSubNMinusOne = rightSubN;
		leftSubNMinusOne = leftSubN;
		recombinedSboxes = 0;
	}

	//at this point we would set the right side equal to the left side and vica versa.  instead we will shift left the right side 32-bits and add the left side
	// for the final number to be put into the final-permutation
	rightSubN <<= 32;
	finalOutput = leftSubN + rightSubN;
	finalOutput = finalPermutation(finalOutput);

	return finalOutput;



}



//this method takes a string of text and converts it to its ASCII representation
unsigned long long convertTextToNumber(string text) {
	unsigned long long num = 0;
	for (int i = 0; i < text.length(); i++) {
		num <<= 8;
		num += (int)text[i];
	}
	return num;
}

/*
this method is to be used when we need extra characters
1)When we encrypt the original file size we need 32 bits of extra characters for the left half
2)When we encrypt a file that is not an even multiple of 8 bytes we need to pad the rest
*/
string generateRandomGarbage(int bytesNeeded) {
	string garbageTxt = "";
	char nextChar;
	srand(time(0));
	unsigned int nextRandom;
	for (int i = 0; i < bytesNeeded; i++) {
		nextRandom = rand() % 256;
		nextRandom &= 0x000000FF;
		garbageTxt.push_back((char)nextRandom);
	}

	return garbageTxt;


}

/*
This method will generate the 48 bit keys based off the 56 bit keys that have already been made
*/
unsigned long long generate48BitKeys(unsigned long long currentKey) {

	unsigned long long current56bitKey;
	unsigned long long next48BitKey = 0;
	//for encryption
	if (currentKey & 0x80000000000000) next48BitKey |= (1ull << (48 - 5));
	if (currentKey & 0x40000000000000) next48BitKey |= (1ull << (48 - 24));
	if (currentKey & 0x20000000000000) next48BitKey |= (1ull << (48 - 7));
	if (currentKey & 0x10000000000000) next48BitKey |= (1ull << (48 - 16));
	if (currentKey & 0x08000000000000) next48BitKey |= (1ull << (48 - 6));
	if (currentKey & 0x04000000000000) next48BitKey |= (1ull << (48 - 10));
	if (currentKey & 0x02000000000000) next48BitKey |= (1ull << (48 - 20));
	if (currentKey & 0x01000000000000) next48BitKey |= (1ull << (48 - 18));
	/////////////////////if( currentKey & 0x00800000000000) next48BitKey |= (1ull << (64-0));
	if (currentKey & 0x00400000000000) next48BitKey |= (1ull << (48 - 12));
	if (currentKey & 0x00200000000000) next48BitKey |= (1ull << (48 - 3));
	if (currentKey & 0x00100000000000) next48BitKey |= (1ull << (48 - 15));
	if (currentKey & 0x00080000000000) next48BitKey |= (1ull << (48 - 23));
	if (currentKey & 0x00040000000000) next48BitKey |= (1ull << (48 - 1));
	if (currentKey & 0x00020000000000) next48BitKey |= (1ull << (48 - 9));
	if (currentKey & 0x00010000000000) next48BitKey |= (1ull << (48 - 19));
	if (currentKey & 0x00008000000000) next48BitKey |= (1ull << (48 - 2));
	//////////////////////if( currentKey & 0x0000400000000000) next48BitKey |= (1ull << (64-0));
	if (currentKey & 0x00002000000000) next48BitKey |= (1ull << (48 - 14));
	if (currentKey & 0x00001000000000) next48BitKey |= (1ull << (48 - 22));
	if (currentKey & 0x00000800000000) next48BitKey |= (1ull << (48 - 11));
	//////////////////////////if( currentKey & 0x00000400000000) next48BitKey |= (1ull << (64-0));
	if (currentKey & 0x00000200000000) next48BitKey |= (1ull << (48 - 13));
	if (currentKey & 0x00000100000000) next48BitKey |= (1ull << (48 - 4));
	///////////////////////if( currentKey & 0x00000080000000) next48BitKey |= (1ull << (64-0));
	if (currentKey & 0x00000040000000) next48BitKey |= (1ull << (48 - 17));
	if (currentKey & 0x00000020000000) next48BitKey |= (1ull << (48 - 21));
	if (currentKey & 0x00000010000000) next48BitKey |= (1ull << (48 - 8));
	if (currentKey & 0x00000008000000) next48BitKey |= (1ull << (48 - 47));
	if (currentKey & 0x00000004000000) next48BitKey |= (1ull << (48 - 31));
	if (currentKey & 0x00000002000000) next48BitKey |= (1ull << (48 - 27));
	if (currentKey & 0x00000001000000) next48BitKey |= (1ull << (48 - 48));
	if (currentKey & 0x00000000800000) next48BitKey |= (1ull << (48 - 35));
	if (currentKey & 0x00000000400000) next48BitKey |= (1ull << (48 - 41));
	/////////////////////if( currentKey & 0x00000000200000) next48BitKey |= (1ull << (64-0));
	if (currentKey & 0x00000000100000) next48BitKey |= (1ull << (48 - 46));
	if (currentKey & 0x00000000080000) next48BitKey |= (1ull << (48 - 28));
	/////////////////////if( currentKey & 0x00000000040000) next48BitKey |= (1ull << (64-0));
	if (currentKey & 0x00000000020000) next48BitKey |= (1ull << (48 - 39));
	if (currentKey & 0x00000000010000) next48BitKey |= (1ull << (48 - 32));
	if (currentKey & 0x00000000008000) next48BitKey |= (1ull << (48 - 25));
	if (currentKey & 0x00000000004000) next48BitKey |= (1ull << (48 - 44));
	/////////////////////if( currentKey & 0x00000000002000) next48BitKey |= (1ull << (64-0));
	if (currentKey & 0x00000000001000) next48BitKey |= (1ull << (48 - 37));
	if (currentKey & 0x00000000000800) next48BitKey |= (1ull << (48 - 34));
	if (currentKey & 0x00000000000400) next48BitKey |= (1ull << (48 - 43));
	if (currentKey & 0x00000000000200) next48BitKey |= (1ull << (48 - 29));
	if (currentKey & 0x00000000000100) next48BitKey |= (1ull << (48 - 36));
	if (currentKey & 0x00000000000080) next48BitKey |= (1ull << (48 - 38));
	if (currentKey & 0x00000000000040) next48BitKey |= (1ull << (48 - 45));
	if (currentKey & 0x00000000000020) next48BitKey |= (1ull << (48 - 33));
	if (currentKey & 0x00000000000010) next48BitKey |= (1ull << (48 - 26));
	if (currentKey & 0x00000000000008) next48BitKey |= (1ull << (48 - 42));
	/////////////////////if( currentKey & 0x00000000000004) next48BitKey |= (1ull << (64-0));
	if (currentKey & 0x00000000000002) next48BitKey |= (1ull << (48 - 30));
	if (currentKey & 0x00000000000001) next48BitKey |= (1ull << (48 - 40));

	return next48BitKey;


}

/*
This method creates the 56 bit keys
*/
unsigned long long generate56BitKeys(unsigned long long prevKey, int round) {

	unsigned long long newKey = 0ull;

	bool bitShiftedIsOne;
	unsigned long long leftHalf;
	unsigned long long rightHalf;
	unsigned long long combinedHalves = 0;
	unsigned long long keyToAlter = prevKey;
	leftHalf = keyToAlter >> 28;
	rightHalf = keyToAlter & 0x0000000FFFFFFF;

	//handle shifting first bit in left half
	//if the variable 'bitShiftedIsOne' is set then that means the first bit was a one

	bitShiftedIsOne = leftHalf & 0x8000000;
	leftHalf &= 0x7FFFFFF;
	leftHalf <<= 1;
	if (bitShiftedIsOne) {
		leftHalf += 1;
	}

	//handle shifting first bit in right half
	bitShiftedIsOne = rightHalf & 0x8000000;
	rightHalf &= 0x7FFFFFF;
	rightHalf <<= 1;
	if (bitShiftedIsOne) {
		rightHalf += 1;
	}

	//if the current round is none of the below then we need to shift two bits not one.
	if (round != 1 && round != 2 && round != 9 && round != 16) {
		//handle left half
		bitShiftedIsOne = leftHalf & 0x8000000;
		leftHalf &= 0x7FFFFFF;
		leftHalf <<= 1;
		if (bitShiftedIsOne) {
			leftHalf += 1;
		}

		//handle right half
		bitShiftedIsOne = rightHalf & 0x8000000;
		rightHalf &= 0x7FFFFFF;
		rightHalf <<= 1;
		if (bitShiftedIsOne) {
			rightHalf += 1;
		}
	}
	//combine halves and store
	leftHalf <<= 28;
	return leftHalf + rightHalf;
}

//this method compresses the provided 64 bit key into 56 bits
unsigned long long compressUsersKey(unsigned long long int providedKey) {
	unsigned long long int compressedKey = 0;
	//key compresion to 56 bits
	if (providedKey & 0x8000000000000000) compressedKey |= (1ull << (56 - 8));
	if (providedKey & 0x4000000000000000) compressedKey |= (1ull << (56 - 16));
	if (providedKey & 0x2000000000000000) compressedKey |= (1ull << (56 - 24));
	if (providedKey & 0x1000000000000000) compressedKey |= (1ull << (56 - 56));
	if (providedKey & 0x0800000000000000) compressedKey |= (1ull << (56 - 52));
	if (providedKey & 0x0400000000000000) compressedKey |= (1ull << (56 - 44));
	if (providedKey & 0x0200000000000000) compressedKey |= (1ull << (56 - 36));
	//***********if( providedKey & 0x0200000000000000) compressedKey |= (1ull << (64-0));
	if (providedKey & 0x0080000000000000) compressedKey |= (1ull << (56 - 7));
	if (providedKey & 0x0040000000000000) compressedKey |= (1ull << (56 - 15));
	if (providedKey & 0x0020000000000000) compressedKey |= (1ull << (56 - 23));
	if (providedKey & 0x0010000000000000) compressedKey |= (1ull << (56 - 55));
	if (providedKey & 0x0008000000000000) compressedKey |= (1ull << (56 - 51));
	if (providedKey & 0x0004000000000000) compressedKey |= (1ull << (56 - 43));
	if (providedKey & 0x0002000000000000) compressedKey |= (1ull << (56 - 35));
	//********if( providedKey & 0x0002000000000000) compressedKey |= (1ull << (64-8));
	if (providedKey & 0x0000800000000000) compressedKey |= (1ull << (56 - 6));
	if (providedKey & 0x0000400000000000) compressedKey |= (1ull << (56 - 14));
	if (providedKey & 0x0000200000000000) compressedKey |= (1ull << (56 - 22));
	if (providedKey & 0x0000100000000000) compressedKey |= (1ull << (56 - 54));
	if (providedKey & 0x0000080000000000) compressedKey |= (1ull << (56 - 50));
	if (providedKey & 0x0000040000000000) compressedKey |= (1ull << (56 - 42));
	if (providedKey & 0x0000020000000000) compressedKey |= (1ull << (56 - 34));
	//*******if( providedKey & 0x0000020000000000) compressedKey |= (1ull << (64-8));
	if (providedKey & 0x0000008000000000) compressedKey |= (1ull << (56 - 5));
	if (providedKey & 0x0000004000000000) compressedKey |= (1ull << (56 - 13));
	if (providedKey & 0x0000002000000000) compressedKey |= (1ull << (56 - 21));
	if (providedKey & 0x0000001000000000) compressedKey |= (1ull << (56 - 53));
	if (providedKey & 0x0000000800000000) compressedKey |= (1ull << (56 - 49));
	if (providedKey & 0x0000000400000000) compressedKey |= (1ull << (56 - 41));
	if (providedKey & 0x0000000200000000) compressedKey |= (1ull << (56 - 33));
	//********if( providedKey & 0x0000000200000000) compressedKey |= (1ull << (64-8));
	if (providedKey & 0x0000000080000000) compressedKey |= (1ull << (56 - 4));
	if (providedKey & 0x0000000040000000) compressedKey |= (1ull << (56 - 12));
	if (providedKey & 0x0000000020000000) compressedKey |= (1ull << (56 - 20));
	if (providedKey & 0x0000000010000000) compressedKey |= (1ull << (56 - 28));
	if (providedKey & 0x0000000008000000) compressedKey |= (1ull << (56 - 48));
	if (providedKey & 0x0000000004000000) compressedKey |= (1ull << (56 - 40));
	if (providedKey & 0x0000000002000000) compressedKey |= (1ull << (56 - 32));
	//********if( providedKey & 0x0000000002000000) compressedKey |= (1ull << (64-8));
	if (providedKey & 0x0000000000800000) compressedKey |= (1ull << (56 - 3));
	if (providedKey & 0x0000000000400000) compressedKey |= (1ull << (56 - 11));
	if (providedKey & 0x0000000000200000) compressedKey |= (1ull << (56 - 19));
	if (providedKey & 0x0000000000100000) compressedKey |= (1ull << (56 - 27));
	if (providedKey & 0x0000000000080000) compressedKey |= (1ull << (56 - 47));
	if (providedKey & 0x0000000000040000) compressedKey |= (1ull << (56 - 39));
	if (providedKey & 0x0000000000020000) compressedKey |= (1ull << (56 - 31));
	//********if( providedKey & 0x0000000000020000) compressedKey |= (1ull << (64-8));
	if (providedKey & 0x0000000000008000) compressedKey |= (1ull << (56 - 2));
	if (providedKey & 0x0000000000004000) compressedKey |= (1ull << (56 - 10));
	if (providedKey & 0x0000000000002000) compressedKey |= (1ull << (56 - 18));
	if (providedKey & 0x0000000000001000) compressedKey |= (1ull << (56 - 26));
	if (providedKey & 0x0000000000000800) compressedKey |= (1ull << (56 - 46));
	if (providedKey & 0x0000000000000400) compressedKey |= (1ull << (56 - 38));
	if (providedKey & 0x0000000000000200) compressedKey |= (1ull << (56 - 30));
	//************if( providedKey & 0x0000000000000200) compressedKey |= (1ull << (64-8));
	if (providedKey & 0x0000000000000080) compressedKey |= (1ull << (56 - 1));
	if (providedKey & 0x0000000000000040) compressedKey |= (1ull << (56 - 9));
	if (providedKey & 0x0000000000000020) compressedKey |= (1ull << (56 - 17));
	if (providedKey & 0x0000000000000010) compressedKey |= (1ull << (56 - 25));
	if (providedKey & 0x0000000000000008) compressedKey |= (1ull << (56 - 45));
	if (providedKey & 0x0000000000000004) compressedKey |= (1ull << (56 - 37));
	if (providedKey & 0x0000000000000002) compressedKey |= (1ull << (56 - 29));
	//***********if( providedKey & 0x0000000000000002) firstNewKey |= (1ull << (64-8));

	return compressedKey;
}

/*
This method will turn the users key and turn it into its corresponding numeric value
*/
unsigned long long int convertKeyToNumber(char* key) {
	string keyStr = key;
	int keyLen = strlen(key);
	unsigned long long int keyValue = 0;


	//if the key is 8 chars then we convert it to a hex number
	if (keyLen == 16) {
		std::stringstream ss;
		ss << keyStr;
		ss >> hex >> keyValue;

	}
	else {
		if (keyLen == 10) {
			keyStr = keyStr.substr(1, 8);
		}

		if (keyLen == 12) {
			keyStr = keyStr.substr(2, 8);
		}
		unsigned int nextNum = 0;
		for (int i = 0; i < 8; i++) {
			keyValue <<= 8;
			nextNum = (unsigned int)keyStr[i];
			keyValue += nextNum;
		}

	}
	return keyValue;
}

/*
This method obtains the input files size and return that number
*/
unsigned int getFileSize(char* inputFile) {
	ifstream mySource;
	mySource.open(inputFile, ios_base::binary);
	mySource.seekg(0, ios_base::end);
	unsigned int size = mySource.tellg();
	mySource.close();
	return size;
}

/*
If the input file is not found then this method will let the program know
*/
bool inputFileNotValid(char* inputFile) {
	std::string fileLocation = inputFile;
	fstream myFile(fileLocation);
	if (!myFile) {
		return true;
	}
	return false;
}

/*
This method takes a string of 8 characters and turns it into its corresponding numerica value based off the hex values of the characters
*/
unsigned long long convertTextToLongForEncryption(string text) {
	unsigned long long returnNum = 0;
	unsigned int nextNum = 0;
	char nextch;
	//shift number over 8 bits (do it first prevents it happening on the last letter)
	//convert next letter in string to integer
	//add it to the running total

	for (int i = 0; i < 8; i++) {
		returnNum <<= 8;
		nextch = (unsigned char)text[i];
		nextNum = (unsigned int)nextch;
		nextNum &= 0x000000FF;
		returnNum += nextNum;
	}

	return returnNum;
}


//method to check for proper key formats, a valid key returns FALSE, an invalid key returns TRUE
//-- 10 characters means 8 characters with 2 single quotations
//-- 12 characters means 8 characters, with a space, with single and double quotations
//-- 16 characters means hex digits only
bool keyIsWrongFormat(char* key) {
	char spaceCharacter[] = " ";
	int keyLength = (unsigned)strlen(key);

	/*
	test:
	*/
	bool hasspace = (strstr(key, spaceCharacter) == nullptr);
	bool ssq = key[0] == '\'';
	bool esq = key[9] == '\'';


	if (keyLength == 10 && (strstr(key, spaceCharacter) == nullptr) && key[0] == '\'' && key[9] == '\'') {
		// key is 10 characters
		// key does not contain a space
		// first and last character are single quotations

		return false;
	}
	else if (keyLength == 12 & (strstr(key, spaceCharacter) != nullptr) && key[1] == '\'' && key[11] == '\'' && key[0] == '"' && key[11] == '"') {								//one space must be included

		return false;
	}
	else if (keyLength == 16) {								//hex digits only
		string keyAsString = key;
		std::transform(keyAsString.begin(), keyAsString.end(), keyAsString.begin(), ::tolower);
		for (int i = 0; i < keyAsString.size(); i++) {
			if ((keyAsString.at(i) < '0' || keyAsString.at(i) > '9') && (keyAsString.at(i) < 'a' || keyAsString.at(i) > 'f')) {
				return true;
			}
		}
		return false;
	}
	else {
		//not valid
		return true;
	}

}


/*
This method takes the command line arguments and checks them for correctness.  If there are not 6 arguments
then the arguments are not valid.  If there are 6 then each argument must be validated.

If there is any error in the arguments then the program will halt and alert the user via console.
*/
void checkArgumentsAreCorrect(int argCount, char* arguments[]) {
	if (argCount != 6) {										//check argument count, must be 6
		cout << "You have not provided enough arguments." << endl;
		system("pause");
		exit(1);
	}

	string modeString = arguments[3];						//pull the 'mode' argument to set as lowercase
	std::transform(modeString.begin(), modeString.end(), modeString.begin(), ::tolower);

	//check for a correct action
	if (strcmp("-e", arguments[1]) == 0 && strcmp("-E", arguments[1]) == 0 && strcmp("-d", arguments[1]) == 0 && strcmp("-D", arguments[1]) == 0) {
		cout << "You have provided an invalid action in your arguments.  Use -e to encrypt or -d to decrypt.";
	}
	//check for correct key format
	else if (keyIsWrongFormat(arguments[2])) {
		cout << "The key you have provided is in an incorrect format.  Please read below for the 3 acceptable formats." << endl;
		cout << "1. The key must 16 hex digits (case insensitive)" << endl;
		cout << "2. The key must be 8 characters long enclosed in single quotations." << endl;
		cout << "\tIf the key contains spaces as a character you must enclose the single-quotation-enclosed key" << endl;
		cout << "\tmust be enclosed in double quotations";
		//TODO : : change the text to be more readable
	}
	//check the mode is correct using the modestring we previously set to lowercase
	else if (modeString != "ecb") {
		cout << "You have entered an incorrect mode.  The mode must be ECB(case insensitive)";
	}
	//if the input file does not exist there is an error
	else if (inputFileNotValid(arguments[4])) {
		cout << "The input file was not found.  Please select a valid file." << endl;
	}
	else {

	}

}

int main(int argc, char* argv[])
{


	checkArgumentsAreCorrect(argc, argv);		//check user parameters are correct, if not then the method will halt the program

	string action = argv[1];
	char*  key = argv[2];
	char* mode = argv[3];
	char* infile = argv[4];
	outputFileName = argv[5];

	bool isEncryption = false;
	if (action == "-e" || action == "-E") {			//variable to know whether we are encryption or decrypting
		isEncryption = true;
	}

	unsigned unsigned long fileSize = getFileSize(infile);
	unsigned long long usersKey = convertKeyToNumber(key);
	unsigned long long usersCompressedKey = compressUsersKey(usersKey);
	unsigned long long setOf56BitKeys[16];

	//create 16 56-bit keys.  using the previous key as the one we next permute and using 'i' as the current round.
	//*****unsigned long long currentKeyToPermute = usersCompressedKey;
	unsigned long long currentKeyToPermute = usersCompressedKey;
	for (int i = 1; i < 17; i++) {
		setOf56BitKeys[i - 1] = generate56BitKeys(currentKeyToPermute, i);
		currentKeyToPermute = setOf56BitKeys[i - 1];
	}

	//create 16x 48-bit keys.
	currentKeyToPermute = setOf56BitKeys[0];
	for (int i = 1; i < 17; i++) {
		keys48Bits[i - 1] = generate48BitKeys(currentKeyToPermute);
		currentKeyToPermute = setOf56BitKeys[i];
	}
	//cout << hex << usersKey << " is now " << hex << usersCompressedKey;

	//read in file contents.  If the length isnt an even multiple of 8 (8bytes * 8 bits = 64 bit block)
	//then we pad with randomness
	std::ifstream ifs(infile, std::ios_base::binary);
	std::string fileContents((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());




	if (isEncryption) {

		//if we're encrypting, the file length needs to be a multiple of 8, here
		// the file contents get padded if it is not already a multiple of 8
		int paddingNeededInFile = 8 - (fileContents.length() % 8);
		if (paddingNeededInFile != 8) {
			fileContents += generateRandomGarbage(paddingNeededInFile);
		}

		//do the encryption
		//first part is the file length
		//generate the left half to mix with the file size, shift it left
		//and combine it with the file size, then send to encryption method
		string leftHalf = generateRandomGarbage(32);
		unsigned long long fileSizeBlockToEncrypt = convertTextToNumber(leftHalf);
		fileSizeBlockToEncrypt <<= 32;
		fileSizeBlockToEncrypt += fileSize;
		
		//start timer and perform encryption
		start = clock();
		unsigned long long encryptedFileSize = performDES(fileSizeBlockToEncrypt, 0);
		convertToText(encryptedFileSize);

		//parse through file text
		string nextStringToEncrypt;
		unsigned long long currentTextStrAsLong = 0;
		for (unsigned int i = 0; i < fileContents.length(); i += 8) {
			nextStringToEncrypt = fileContents.substr(i, 8);
			currentTextStrAsLong = convertTextToLongForEncryption(nextStringToEncrypt);
			convertToText(performDES(currentTextStrAsLong, 0));
		}

	}
	else {
		//do the decryption

		//reverse keyset
		unsigned long long temp;
		for (int i = 0; i < 8; i++) {
			temp = keys48Bits[15 - i];
			keys48Bits[15 - i] = keys48Bits[i];
			keys48Bits[i] = temp;
		}

		//decrypt
		string nextStringToEncrypt;

		unsigned int filelen = fileContents.length();
		unsigned long long currentTextStrAsLong = 0;
		unsigned int originalFileSize, numberOfBlocks, remainingChars, currentBlock;
		currentBlock = 0;

		//start clock 
		start = clock();

		/*
			Decrypting Loop:
				first we decrypt the first block and get the original file size.  From this we can determine how many blocks
				of 8 characters we will have to perform DES on to decrypt.

				If the original file had an even multiple of 8 bytes then the number of blocks will be spot on.

				If the original file did NOT have an even multiple of 8 bytes then when we divide the total bytes/8
				we will lose the remainder(remaining characters at th end) in the division.  That means the total number 
				of blocks we have to process is more than the calculated number of blocks.
				--This is accounted for in the loop by checking if the current block we are processing is greater than the # calculated.
				--If this is true we only want the correct number of characters.
		*/
		for (unsigned int i = 0; i < fileSize; i += 8) {
			nextStringToEncrypt = fileContents.substr(i, 8);
			currentTextStrAsLong = convertTextToLongForEncryption(nextStringToEncrypt);
			
			//First block is the original-file size.
			if (i == 0) {
				originalFileSize = performDES(currentTextStrAsLong, 1);
				originalFileSize &= 0x00000000FFFFFFFF;
				numberOfBlocks = originalFileSize / 8;
				remainingChars = originalFileSize %= 8;
			}
			else if (currentBlock > numberOfBlocks) {
				unsigned long long lastblock = performDES(currentTextStrAsLong, 0);
				writeRemainingChars(lastblock, remainingChars);
			}
			else {
				convertToText(performDES(currentTextStrAsLong, 0));
			}
			currentBlock++;
		}

	}

	//end the clock and print to console.
	clock_t timeElapsed = clock() - start;
	unsigned msElapsed = timeElapsed / CLOCKS_PER_MS;
	cout << endl << endl << (msElapsed/1000) << "." << msElapsed%1000 << "ms elapsed.";
	
	//write the final string to the file all at once
	std::ofstream outfile(outputFileName, std::ios::out | std::ios::binary);
	unsigned int strLength = textForFile.length();
	outfile.write(textForFile.c_str(), textForFile.length());
	outfile.close();







	return 0;
}



/*
	read 8 bytes at a time as an unsigned long long

	encrypt it 

	write the block






	------------------------------

	analyze the patterns on the input and the output









*/


