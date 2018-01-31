// DES.cpp : 
/* DES.cpp is simply a program that encrypts a file using DES encrpytion.
*
*
*
*
*/

//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;


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
																																												// key is 12 characters
																																												// key contains a space
																																												// first and last character are double quotations
																																												// 2nd and 11th character are single quotation

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

bool inputFileNotValid(char* inputFile) {
	return false;
}


/*
This method takes the command line arguments and checks them for correctness.  If there are not 6 arguments
then the arguments are not valid.  If there are 6 then each argument must be validated.

If there is any error in the arguments then the program will halt and alert the user via console.
*/
void checkArgumentsAreCorrect(int argCount, char* arguments[]) {
	if (argCount != 6) {										//check argument count, must be 6
		cout << "You have not provided enough arguments.";
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
		"You have entered an incorrect mode.  The mode must be ECB(case insensitive)";
	}
	//if the input file does not exist there is an error
	else if (inputFileNotValid(arguments[6])) {

	}
	else {

	}

}

int main(int argc, char* argv[])
{
	checkArgumentsAreCorrect(argc, argv);

	return 0;
}