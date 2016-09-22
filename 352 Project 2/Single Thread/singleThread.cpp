#include <iostream>
#include <fstream>
using namespace std;

void countLetters(int (&countArray)[26], char* toCount, int toCountSize);
void printLetterArray(int countArray[26]);
void encryptBuffer(char* ibuff, char* obuff, int b_Size, int& encryptKey);
char encryptLetter(char toEncrypt, int encryptKey, bool& alpha);

int main(int argc, char* argv[]){
	
	//Variables:
	
	//File Variables:
	ifstream input;
	ofstream output;
	
	//String Variables:
	int b_Size = 0;
	char* ibuff;
	char* obuff;
	int writeSize = 0;
	
	//Count Variables:
	int inputCountArray[26];
	int outputCountArray[26];
	int (&inputCountArray_R)[26] = inputCountArray;
	int (&outputCountArray_R)[26] = outputCountArray;
	
	//Encryption Variables:
	int encryptKey = 1;
	int& encryptKey_R = encryptKey;
	
	//check to make sure there are the correct number of arguments
	if (argc !=  3){
		cout << "Improper number of arguments, you must specify input and output files" << endl;
		return 0;
	}
	
	//Initialize Count Arrays
	for (int i = 0; i < 26; i++){
		inputCountArray[i] = 0;
		outputCountArray[i] = 0;
	}
	
	//open the file input
	input.open(argv[1]);
	
	//check to make sure input is valid
	if (!input){
		input.close();
		cout << "Input file does not exist" << endl;
		return 0;
	}
	
	//open file output
	output.open(argv[2]);
	
	//check to make sure output is valid
	if(!output){
		output.close();
		cout << "Output file is not valid" << endl;
		return 0;
	}
	
	//Prompt user for buffer size
	cout << "Please enter a buffer size" << endl;
	cin >> b_Size;
	
	//Instantiate the buffers
	ibuff = new char[b_Size];
	obuff = new char[b_Size];
	for (int i = 0; i < b_Size; i++){
		ibuff[i] = '\0';
		obuff[i] = '\0';
	}
	
	//Read from input file:
	while(input.readsome(ibuff, b_Size)){
	  
		//Count letters
		countLetters(inputCountArray_R, ibuff, b_Size);
		
		//Encrypt
		encryptBuffer(ibuff, obuff, b_Size, encryptKey_R);
		
		//Make a count so that on the last run
		//when the buffer doesn't necessarily fill, it will not
		//print out \0 characters
		for (int i = 0; i < b_Size; i++){
			if (obuff[i] != '\0') writeSize++;
		}
	
		//Output
		output.write(obuff, writeSize);
	
		//Count letters
		countLetters(outputCountArray_R, obuff, b_Size);
		
		//Reset the buffer output size
		writeSize = 0;
		
		//Clear Buffers
		for (int i = 0; i < b_Size; i++){
			ibuff[i] = '\0';
			obuff[i] = '\0';
		}
	}
	
	//Print Letter counts
	cout << "Input file contains:" << endl;
	printLetterArray(inputCountArray);
	cout << "Output file contains:" << endl;
	printLetterArray(outputCountArray);
	
	//Delete Arrays
	delete[] ibuff;
	delete[] obuff;
	
	return 0;
}

//Counts the letters from a character array and stores them in a letter Array
void countLetters(int (&countArray)[26], char* toCount, int toCountSize){
	for (int i = 0; i < toCountSize; i++){
		switch(tolower(toCount[i])){
			case 'a':
				countArray[0]++;
				break;
			case 'b':
				countArray[1]++;
				break;
			case 'c':
				countArray[2]++;
				break;
			case 'd':
				countArray[3]++;
				break;
			case 'e':
				countArray[4]++;
				break;
			case 'f':
				countArray[5]++;
				break;
			case 'g':
				countArray[6]++;
				break;
			case 'h':
				countArray[7]++;
				break;
			case 'i':
				countArray[8]++;
				break;
			case 'j':
				countArray[9]++;
				break;
			case 'k':
				countArray[10]++;
				break;
			case 'l':
				countArray[11]++;
				break;
			case 'm':
				countArray[12]++;
				break;
			case 'n':
				countArray[13]++;
				break;
			case 'o':
				countArray[14]++;
				break;
			case 'p':
				countArray[15]++;
				break;
			case 'q':
				countArray[16]++;
				break;
			case 'r':
				countArray[17]++;
				break;
			case 's':
				countArray[18]++;
				break;
			case 't':
				countArray[19]++;
				break;
			case 'u':
				countArray[20]++;
				break;
			case 'v':
				countArray[21]++;
				break;
			case 'w':
				countArray[22]++;
				break;
			case 'x':
				countArray[23]++;
				break;
			case 'y':
				countArray[24]++;
				break;
			case 'z':
				countArray[25]++;
				break;
			default:
				break;
		}
	}
}

//Prints the contents of the letter array to the screen
void printLetterArray(int countArray[26]){
	if (countArray[0] != 0) cout << "A: " << countArray[0] << endl;
	if (countArray[1] != 0) cout << "B: " << countArray[1] << endl;
	if (countArray[2] != 0) cout << "C: " << countArray[2] << endl;
	if (countArray[3] != 0) cout << "D: " << countArray[3] << endl;
	if (countArray[4] != 0) cout << "E: " << countArray[4] << endl;
	if (countArray[5] != 0) cout << "F: " << countArray[5] << endl;
	if (countArray[6] != 0) cout << "G: " << countArray[6] << endl;
	if (countArray[7] != 0) cout << "H: " << countArray[7] << endl;
	if (countArray[8] != 0) cout << "I: " << countArray[8] << endl;
	if (countArray[9] != 0) cout << "J: " << countArray[9] << endl;
	if (countArray[10] != 0) cout << "K: " << countArray[10] << endl;
	if (countArray[11] != 0) cout << "L: " << countArray[11] << endl;
	if (countArray[12] != 0) cout << "M: " << countArray[12] << endl;
	if (countArray[13] != 0) cout << "N: " << countArray[13] << endl;
	if (countArray[14] != 0) cout << "O: " << countArray[14] << endl;
	if (countArray[15] != 0) cout << "P: " << countArray[15] << endl;
	if (countArray[16] != 0) cout << "Q: " << countArray[16] << endl;
	if (countArray[17] != 0) cout << "R: " << countArray[17] << endl;
	if (countArray[18] != 0) cout << "S: " << countArray[18] << endl;
	if (countArray[19] != 0) cout << "T: " << countArray[19] << endl;
	if (countArray[20] != 0) cout << "U: " << countArray[20] << endl;
	if (countArray[21] != 0) cout << "V: " << countArray[21] << endl;
	if (countArray[22] != 0) cout << "W: " << countArray[22] << endl;
	if (countArray[23] != 0) cout << "X: " << countArray[23] << endl;
	if (countArray[24] != 0) cout << "Y: " << countArray[24] << endl;
	if (countArray[25] != 0) cout << "Z: " << countArray[25] << endl;
}

//Takes in a buffer and encrypts each letter
void encryptBuffer(char* ibuff, char* obuff, int b_Size, int& encryptKey){
	bool alpha = false;
	bool& alpha_R = alpha;
	for (int i = 0; i < b_Size; i++){
		obuff[i] = encryptLetter(ibuff[i], encryptKey, alpha_R);
		
		//Update Encrypt Key
		if(alpha){
			switch(encryptKey){
				case 0:
					encryptKey = 1;
					break;
				case 1:
					encryptKey = -1;
					break;
				case -1:
					encryptKey = 0;
					break;
			}
			alpha = false;
		}
	}
}

//Encrypts a letter given the rules
char encryptLetter(char toEncrypt, int encryptKey, bool& alpha){
	if (isalpha(toEncrypt) == 0) return toEncrypt;
	
	char toReturn;
	
	//Calculate encyrpted character
	if (toEncrypt == 'a' && encryptKey == -1) toReturn = 'z';
	else if(toEncrypt == 'A' && encryptKey == -1) toReturn = 'Z';
	else if (toEncrypt == 'z' && encryptKey == 1) toReturn = 'a';
	else if (toEncrypt == 'Z' && encryptKey == 1) toReturn = 'A';
	else toReturn = toEncrypt + encryptKey;
	
	//Set flag so that Encrypt key gets changed
	alpha = true;
	
	return toReturn;
}
